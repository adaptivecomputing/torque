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
#ifndef _AF_H
#define _AF_H
/* Feature test switches */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif  /* _POSIX_SOURCE */

/* System headers */ 

/* Local Headers */

/* Macros */
#define EMPTYSTR ""
#define STRCMP(a, R, b) (strcmp((a?a:EMPTYSTR), (b?b:EMPTYSTR)) R 0)
#define DATECMP(a, R, b) (datecmp(a, b) R 0)
#define TIMECMP(a, R, b) (timecmp(a, b) R 0)
#define DATETIMECMP(a, R, b) (datetimecmp(a, b) R 0)
#define SIZECMP(a, R, b) (sizecmp(a, b) R 0)

/* special return values to some of the assist functions */
#define SUCCESS		1
#define FAIL		0
#define NULLSTR		(char *)NULL
#define NOSERVER	(Server *)NULL
#define NOQUE		(Que *)NULL
#define NOJOB		(Job *)NULL
#define NOCNODE		(CNode *)NULL
#define EMPTYSETSERVER  (SetServer *)NULL
#define EMPTYSETJOB	(struct SetJobElement *)NULL
#define EMPTYSETQUE	(SetQue *)NULL
#define EMPTYSETCNODE	(SetCNode *)NULL

#define paste2(a, b)	a ## b
#define paste3(a, b, c) a ## b ## c
#define FALSE 0
#define TRUE  1

/* order in sorting */
#define ASC   9
#define DESC  10

#define SIZE_BADVAL    1
#define SIZE_BADSUFFIX 2
/*   NONE */
/* File Scope Variables */
/*  NONE  */
/* External Variables */

struct time_struct {
	int  h;
	int  m;
	int  s;
};

typedef struct time_struct Time;

struct date_struct {
	int  m;
	int  d;
	int  y;		/* full year please */
};

typedef struct date_struct Date;

struct datetime_struct {
	Time t;
	Date d;
};

typedef struct datetime_struct DateTime;

struct size_struct {
	long 	 int num;	/* numeric part */
	unsigned int shift;	/* K=10, M=20, G=30, T=40, P=50 */
	unsigned int units;	/* BYTES=0, WORD=1 */
};
typedef struct size_struct Size;

#define BYTES 0
#define WORDS 1

enum action_type { SYNCRUN, ASYNCRUN, DELETE, RERUN, HOLD, RELEASE, SIGNAL,                                                             MODIFYATTR, MODIFYRES };
typedef enum action_type Action;

enum cpr_type { OP_EQ, OP_NEQ, OP_GT, OP_GE, OP_LT, OP_LE, OP_MAX, OP_MIN };
typedef enum cpr_type Comp;

enum dayofweek_type { SUN, MON, TUE, WED, THU, FRI, SAT };
typedef enum dayofweek_type Dayofweek;

/* Range Types */
struct intRange_struct {
        int lo;
        int hi;
};

typedef struct intRange_struct IntRange;

struct floatRange_struct {
        float lo;
        float hi;
};

typedef struct floatRange_struct FloatRange;

struct dayofweekRange_struct {
        Dayofweek lo;
        Dayofweek hi;
};

typedef struct dayofweekRange_struct DayofweekRange;

struct timeRange_struct {
        Time lo;
        Time hi;
};

typedef struct timeRange_struct TimeRange;

struct dateRange_struct {
        Date lo;
        Date hi;
};

typedef struct dateRange_struct DateRange;

struct datetimeRange_struct {
        DateTime lo;
        DateTime hi;
};

typedef struct datetimeRange_struct DateTimeRange;

struct sizeRange_struct {
        Size lo;
        Size hi;
};

struct IntRes {
	struct IntRes   *nextptr;
	char		*name;
	int		value;
};

struct SizeRes {
	struct SizeRes  *nextptr;
	char		*name;
	Size		value;
};

struct StringRes {
	struct StringRes  *nextptr;
	char		  *name;
	char		  *value;
};

typedef struct sizeRange_struct SizeRange;

/* External Variables */

/* External Functions */
extern void
dynamic_strcpy A_((char **str1_ptr, const char *str2));

extern void
dynamic_strcat A_((char **str1_ptr, const char *str2));

extern int
strtimeToSecs A_((char *times));

extern int
strToInt A_((char *str));

extern double
strToFloat A_((char *str));

extern int
strToDayofweek A_((char *str));

extern Date
strToDate A_((char *str));

extern Time
strToTime A_((char *str));

extern DateTime
strToDateTime A_((char *str));

extern Size
strToSize A_((char *str));

extern IntRange
strToIntRange A_((char *str));

extern FloatRange
strToFloatRange A_((char *str));

extern DayofweekRange
strToDayofweekRange A_((char *str));

extern DateRange
strToDateRange A_((char *str));

extern TimeRange
strToTimeRange A_((char *str));

extern DateTimeRange
strToDateTimeRange A_((char *str));

extern SizeRange
strToSizeRange A_((char *str));

extern DateTime
strsecsToDateTime A_((char *val));

extern int
strToBool A_((char *val));

extern void
sizeToStr A_((Size sizeval, char *cvnbuf));

extern int
datecmp A_((Date d1, Date d2));

extern int
timecmp A_((Time t1, Time t2));

extern int
datetimecmp A_((DateTime dt1, DateTime dt2));

extern DateTime
datetimeGet A_((void));

extern int
datetimeToSecs A_((DateTime dt));

extern int
sizecmp A_((Size a, Size w));

extern Size
sizeAdd A_((Size a, Size w));

extern Size
sizeSub A_((Size a, Size w));

extern Size
sizeMul A_((Size a, Size w));

extern Size
sizeDiv A_((Size a, Size w));

extern Size
sizeUminus A_((Size sz));

extern int
dynamicArraySize A_((void *array));

extern void *
initDynamicArray A_((size_t numElems, size_t elementSize));

extern void *
extendDynamicArray A_((void *array, size_t numElems, size_t elementSize));

extern void
printDynamicArrayTable A_((void));

extern void
freeDynamicArray A_((void *array));

extern void
datePrint A_((Date d));

extern void
timePrint A_((Time t));

extern void
datetimePrint A_((DateTime dt));

extern void
sizePrint A_((Size s, int readable));

extern void
intRangePrint A_((IntRange r));

extern void
floatRangePrint A_((FloatRange r));

extern void
dayofweekPrint A_((Dayofweek dow));

extern void
actPrint A_((Action act));

extern void
cprPrint A_((Comp cpr));

extern void
dayofweekRangePrint A_((DayofweekRange r));

extern void
dateRangePrint A_((DateRange d));

extern void
timeRangePrint A_((TimeRange t));

extern void
datetimeRangePrint A_((DateTimeRange dt));

extern void
sizeRangePrint A_((SizeRange s, int readable));

extern int
sizeRangecmp A_((SizeRange r1, SizeRange r2));

extern int
sizeStrcmp A_((char *a, char *w));

extern int
sizeRangeStrcmp A_((char *str1, char *str2));

extern IntRange
toIntRange A_((int i1, int i2));

extern FloatRange
toFloatRange A_((double f1, double f2));

extern DayofweekRange
toDayofweekRange A_((Dayofweek dow1, Dayofweek dow2));

extern DateRange
toDateRange A_((Date d1, Date d2));

extern TimeRange
toTimeRange A_((Time t1, Time t2));

extern DateTimeRange
toDateTimeRange A_((DateTime dt1, DateTime dt2));

extern SizeRange
toSizeRange A_((Size sz1, Size sz2));

extern char *
strCat A_((char *str1, char *str2));

extern void
varstrAdd A_((void *ptr, int scope, void *pptr));

extern void
varstrRemove A_((void *ptr));

extern void
varstrModScope A_((void *ptr, int newscope));

extern void
varstrModPptr A_((void *ptr, void *newpptr));

extern void
varstrFree A_((void *ptr));

extern void
varstrFreeByScope A_((int scope));

extern void
varstrFreeByPptr A_((void *pptr));

extern void
varstrPrint A_((void));

extern void
mallocTableAdd A_((void *ptr, void *pptr, int scope));

extern void
mallocTablePrint A_((void));

extern void
mallocTableFree A_((void *ptr));

extern void
mallocTableFreeByPptr A_((void *pptr));

extern void
mallocTableFreeByScope A_((int scope, void (*freefunc)()));

extern void
mallocTableModScope A_((void *ptr, int scope));

extern void
mallocTableSafeModScope A_((void *ptr, int scope));

extern int
inMallocTable A_((void *ptr));

extern int
normalizeSize A_((Size *a, Size *b, Size *ta, Size *tb));

/* functions related to IntRes and SizeRes */

extern int
IntResValueGet A_((struct IntRes *head, char *name));

extern struct IntRes *
IntResValuePut A_((struct IntRes *head, char *name, int value, void *pptr));

extern void
IntResListPrint A_((struct IntRes *head, char *descr));

extern void
IntResListFree A_((struct IntRes *head));

extern Size
SizeResValueGet A_((struct SizeRes *head, char *name));

extern struct SizeRes *
SizeResValuePut A_((struct SizeRes *head, char *name, Size value, void *pptr));

extern void
SizeResListPrint A_((struct SizeRes *head, char *descr));

extern void
SizeResListFree A_((struct SizeRes *head));

extern char *
StringResValueGet A_((struct StringRes *head, char *name));

extern struct StringRes *
StringResValuePut A_((struct StringRes *head, char *name, char *value,
                                                                void *pptr));

extern void
StringResListPrint A_((struct StringRes *head, char *descr));

extern void
StringResListFree A_((struct StringRes *head));

#endif		/* _AF_H */
