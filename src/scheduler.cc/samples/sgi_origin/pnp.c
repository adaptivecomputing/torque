/*
*         OpenPBS (Portable Batch System) v2.3 Software License
* 
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
* 
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
* 
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
* 
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
* 
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
* 
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
* 
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
* 
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
* 
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
* 
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
* 
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information 
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
* 
* 7. DISCLAIMER OF WARRANTY
* 
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
* 
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/
/* $Id$ */

/*
 * It is important to be able to accurately determine when it is prime and 
 * non-prime time.  This is accomplished by reading the holidays file and 
 * creating a table of prime and non-prime times.
 * A quick search of the table can then determine whether we are in prime 
 * or non-prime time.
 */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pbs_ifl.h"
#include "log.h"

#include "toolkit.h"
#include "gblxvars.h"

#define	SUNDAY   0
#define SATURDAY 1	/* XXX Saturday is 6 in timeval */
#define WEEKDAY  2
#define	PRIME    0
#define NONPRIME 1

#define MAX_HOLIDAYS	256

/*
 * Range of valid primetime times is 0 - 86399 (hr * 3600 + min * 60 + sec).
 * Valid times cannot be negative.
 */
#define	ALL      -1
#define NONE     -2

static int Num_holidays;
static int holidays[MAX_HOLIDAYS][2];
static int weekdays[3][2];
static int day_prime_time (int, int);
static void init_holidays (void);

/*
 * Given a time_t, determine if it is prime or nonprime time then.  If
 * the 'when' argument is 0, find out if it is primetime *now*.
 * Return 1 for prime time, 0 for nonprime time
 */
int 
schd_prime_time(time_t when)
{
    struct tm *tmptr;
    int     cur_time;	/* Now (decimal - 9:30AM == 930, 6:45PM == 1845) */
    int     h_idx;
    int     rcode;

#ifdef DEBUG
    if (!(schd_ENFORCE_PRIME_TIME && schd_TimeNow >= schd_ENFORCE_PRIME_TIME)) {
	fprintf(stderr,"WARNING!!!  ");
	fprintf(stderr,"prime_time() called but ENFORCE_PRIME_TIME not set\n");
	fprintf(stderr,"WARNING!!!  ");
	fprintf(stderr,"This is almost certainly a bug!\n");
	return (0);
    }
#endif /* DEBUG */

	/* If no time specified, use the scheduler's "TimeNow" time_t. */
    if (when == 0)
	when = schd_TimeNow;
    tmptr = localtime(&when);

    cur_time = (tmptr->tm_hour * 3600) + (tmptr->tm_min * 60) + tmptr->tm_sec;

    /* 
     * Check holidays array to determine if today is a holiday.  Holidays
     * are not primetime.  Note that days in struct timeval's are counted 
     * starting at 0, not at the first like on the calendar.
     */
    for (h_idx = 0; h_idx < Num_holidays; h_idx++) {
	if (tmptr->tm_yday + 1 == holidays[h_idx][0])
	    return (0);
    }

    /* Today isn't a holiday so is it SATURDAY, SUNDAY or WEEKDAY? */
    switch (tmptr->tm_wday) {
    case 0:
	rcode = day_prime_time(SUNDAY, cur_time);
	break;

    case 6:
	rcode = day_prime_time(SATURDAY, cur_time);
	break;

    default:
	rcode = day_prime_time(WEEKDAY, cur_time);
	break;
    }

#if 0
    DBPRT(("schd_prime_time: it %s primetime at %s", rcode ? "is" : "is not",
	asctime(tmptr)));
#endif /* 0 */

    return (rcode);
}

/* Based on time of day and day of week, is it prime time now? */
static int 
day_prime_time(int dow, int now)
{
    int     prime_time;
    int     not_prime_time;

    prime_time = weekdays[dow][PRIME];
    not_prime_time = weekdays[dow][NONPRIME];

    switch (prime_time) {
    case ALL:
	return (1);		/* Prime time all day */

    case NONE:
	return (0);		/* No prime time today */

    default:	/* XXX document this case */
	if (not_prime_time > prime_time) {
	    if ((now >= prime_time) && (now < not_prime_time))
		return (1);
	    else
		return (0);
	} else {
	    if ((now >= not_prime_time) && (now < prime_time))
		return (0);
	    else
		return (1);
	}
    }
}

/* Initialize the holidays[] array from the contents of the holidays file */
int 
schd_read_holidays(void)
{
    char   *id = "schd_read_holidays";
    FILE   *fd;
    char    buf[255];
    char   *ptr, *err;
    int     h_idx = 0;
    long    day, line = 0;

    init_holidays();
    Num_holidays = 0;

    if ((fd = fopen(HOLIDAYS_FILE, "r")) == NULL) {
        (void)sprintf(log_buffer,
	    "Warning: holidays file (%s) is unreadable. Ignoring...",
	    HOLIDAYS_FILE);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));

	return (-1);
    }

    /* Read lines from the holidays file one by one. */
    while (fgets(buf, sizeof (buf), fd)) {

	line ++;

	if (strlen(buf) == (sizeof(buf) - 1)) {
	    (void)sprintf(log_buffer,
		"Line %ld of %s is too long.", line, HOLIDAYS_FILE);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s: %s\n", id, log_buffer));
	    return (-1);
	}

	/* Trim off comments before anything else is done. */
	if ((ptr = strchr(buf, '*')) != NULL)
	    *ptr = '\0';

	/* Find first whitespace-separated token in the remaining string. */
	ptr = strtok(buf, " \t");

	/* Skip blank lines (may have been only a comment). */
	if (ptr == NULL)
	    continue;

	/* Skip 'YEAR ...' entries */
	if (!strncmp(ptr, "YEAR", 4))
	    continue;

	/* 
	 * Line was not parsed otherwise -- must be a holiday.  Holidays 
	 * are non prime, so set current holiday list to the day number 
	 * of the current year on which the holiday is celebrated.
	 * Note a syntax error if the number doesn't parse nicely, or it
	 * is nonsensical.
	 */

	day = strtol(ptr, &err, 10);
	if ((*err != '\0') || (day < 0) || (day > 365)) {
	    (void)sprintf(log_buffer,
		"Syntax error in line %ld of %s", line, HOLIDAYS_FILE);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s: %s\n", id, log_buffer));
	    return (-1);
	}

	/* SGI uses a day of 0 to indicate "ANY YEAR" - ignore these. */
	if (day == 0)
	    continue;

	/* No loss of precision, since 1 <= day <= 365 (see above). */
	holidays[h_idx][0] = (int)day;
	holidays[h_idx][NONPRIME] = ALL;
	h_idx++;
	Num_holidays++;

	if (Num_holidays == MAX_HOLIDAYS) {
	    (void)sprintf(log_buffer,
		"Only %d holidays allowed - skipping any remaining in %s",
		MAX_HOLIDAYS, HOLIDAYS_FILE);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s: %s\n", id, log_buffer));
	    break;
	}
    }

    fclose(fd);

    return (Num_holidays);
}

/*
 * Initialize weekdays array.  Default is "no primetime anytime", and no 
 * holidays.
 *
 * Holidays are loaded from the file ''/usr/lib/acct/holidays'' on the NAS
 * Origin2000s.
 */
static void 
init_holidays(void)
{
    /* Default is no primetime anytime. */

    weekdays[SATURDAY][PRIME]    = NONE;
    weekdays[SATURDAY][NONPRIME] = ALL;
    weekdays[SUNDAY][PRIME]      = NONE;
    weekdays[SUNDAY][NONPRIME]   = ALL;
    weekdays[WEEKDAY][PRIME]     = schd_PRIME_TIME_START;
    weekdays[WEEKDAY][NONPRIME]  = schd_PRIME_TIME_END;

    Num_holidays = 0;	/* No holidays to start out. */
}

#define MIDNIGHT 86400
/* 
 * Calculate the number of seconds until the next prime time begins.
 * If the 'when' argument is non-zero, determine how long from the
 * time given.  Otherwise, determine how long it is from now.
 */
int 
schd_secs_til_prime(time_t when)
{
    struct tm *tmptr;
    time_t  now;	/* Number of seconds elapsed since midnight */
    int     next_prime;
    int     prime_start;
    int     days;
    int     p;

    if (when == 0) {
	/* No time specified -- find out what time it is now. */
	time(&when);
    }
    tmptr = localtime(&when);

    /*
     * Calculate number of seconds elapsed since last midnight (00:00:00).
     */

    now = ((tmptr->tm_hour * 60 * 60) + 
	   (tmptr->tm_min * 60) + 
	    tmptr->tm_sec);

    /*
     * Is today a FRIDAY or SATURDAY?  Assume there is no primetime on
     * weekends, and calculate number of days forward from that.
     */

    switch (tmptr->tm_wday) {
    case 5: /* Friday */
	/* 
	 * If it's before the start of prime time, then the next prime time
	 * is in a matter of hours.  Otherwise, prime time starts on the
	 * following Monday (in 2 days).
	 */

	p = weekdays[WEEKDAY][PRIME];
	if (now < p)
	    days = 0;
	else
	    days = 2;
	break;

    case 6: /* Saturday */
	/*
	 * Primetime won't start until Monday, so we know we have at least
	 * one day to wait (all on Sunday).
	 */
	days = 1;
	break;

    default: /* Sunday through Thursday, expect primetime to start "soon" */
	days = 0;
	break;
    }

    /*
     * 'days' tells us how many days we need to get to a weekday (either
     * this day, or the following Monday morning).  This will be the first
     * prime time.  This will need a special case if a specific weekday 
     * does not have a prime time.
     */

    prime_start = weekdays[WEEKDAY][PRIME];

    /*
     * If we are already past the start of this prime time, than the *next*
     * prime time will begin tomorrow morning.  To get the time until then,
     * take the number of seconds in a day, and subtract how far into today
     * you've gotten.  XXX is this right in all cases?
     */
    if (now >= prime_start)
	next_prime = MIDNIGHT - (now - prime_start);
    else
	next_prime = prime_start - now;

    /* Add in the time for any full days without primetime. */
    next_prime += (days * MIDNIGHT);	

    return (next_prime);
}

/* 
 * Calculate the number of seconds until the next non-prime time begins.
 * If the 'when' argument is non-zero, determine how long from the
 * time given.  Otherwise, determine how long it is from now.
 */
int 
schd_secs_til_nonprime(time_t when)
{
    struct tm *tmptr;
    time_t  now;	/* Number of seconds elapsed since midnight */
    int     prime_end;
    int     non_prime_start;

    if (when == 0) {
	/* No time specified -- find out what time it is now. */
	time(&when);
    }
    tmptr = localtime(&when);

    /*
     * Calculate number of seconds elapsed since last midnight (00:00:00).
     */

    now = ((tmptr->tm_hour * 60 * 60) + 
	   (tmptr->tm_min * 60) + 
	    tmptr->tm_sec);


    /* Find how many seconds from midnight until the next nonprime time. */

    non_prime_start = weekdays[WEEKDAY][NONPRIME];

    prime_end = non_prime_start - now; 

    return (prime_end);
}

int 
schd_reset_observed_pt (QueueList *qlist)
{
    QueueList *qptr;
    int   nqueues;

    nqueues = 0;
    for (qptr = qlist; qptr != NULL; qptr = qptr->next) {
	qptr->queue->observe_pt = 
	  (schd_ENFORCE_PRIME_TIME && schd_TimeNow >= schd_ENFORCE_PRIME_TIME);
	++ nqueues;
    }

    return (nqueues);
}

