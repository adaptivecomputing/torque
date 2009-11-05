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
#define SUCCESS  1
#define FAIL  0
#define NULLSTR  (char *)NULL
#define NOSERVER (Server *)NULL
#define NOQUE  (Que *)NULL
#define NOJOB  (Job *)NULL
#define NOCNODE  (CNode *)NULL
#define EMPTYSETSERVER  (SetServer *)NULL
#define EMPTYSETJOB (struct SetJobElement *)NULL
#define EMPTYSETQUE (SetQue *)NULL
#define EMPTYSETCNODE (SetCNode *)NULL

#define paste2(a, b) a ## b
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

struct time_struct
  {
  int  h;
  int  m;
  int  s;
  };

typedef struct time_struct Time;

struct date_struct
  {
  int  m;
  int  d;
  int  y;  /* full year please */
  };

typedef struct date_struct Date;

struct datetime_struct
  {
  Time t;
  Date d;
  };

typedef struct datetime_struct DateTime;

struct size_struct
  {
  long   int num; /* numeric part */
  unsigned int shift; /* K=10, M=20, G=30, T=40, P=50 */
  unsigned int units; /* BYTES=0, WORD=1 */
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

struct intRange_struct
  {
  int lo;
  int hi;
  };

typedef struct intRange_struct IntRange;

struct floatRange_struct
  {
  float lo;
  float hi;
  };

typedef struct floatRange_struct FloatRange;

struct dayofweekRange_struct
  {
  Dayofweek lo;
  Dayofweek hi;
  };

typedef struct dayofweekRange_struct DayofweekRange;

struct timeRange_struct
  {
  Time lo;
  Time hi;
  };

typedef struct timeRange_struct TimeRange;

struct dateRange_struct
  {
  Date lo;
  Date hi;
  };

typedef struct dateRange_struct DateRange;

struct datetimeRange_struct
  {
  DateTime lo;
  DateTime hi;
  };

typedef struct datetimeRange_struct DateTimeRange;

struct sizeRange_struct
  {
  Size lo;
  Size hi;
  };

struct IntRes
  {

  struct IntRes   *nextptr;
  char  *name;
  int  value;
  };

struct SizeRes
  {

  struct SizeRes  *nextptr;
  char  *name;
  Size  value;
  };

struct StringRes
  {

  struct StringRes  *nextptr;
  char    *name;
  char    *value;
  };

typedef struct sizeRange_struct SizeRange;

/* External Variables */

/* External Functions */
extern void
  dynamic_strcpy(char **str1_ptr, const char *str2);

extern void
  dynamic_strcat(char **str1_ptr, const char *str2);

extern int
  strtimeToSecs(char *times);

extern int
  strToInt(char *str);

extern double
  strToFloat(char *str);

extern int
  strToDayofweek(char *str);

extern Date
  strToDate(char *str);

extern Time
  strToTime(char *str);

extern DateTime
  strToDateTime(char *str);

extern Size
  strToSize(char *str);

extern IntRange
  strToIntRange(char *str);

extern FloatRange
  strToFloatRange(char *str);

extern DayofweekRange
  strToDayofweekRange(char *str);

extern DateRange
  strToDateRange(char *str);

extern TimeRange
  strToTimeRange(char *str);

extern DateTimeRange
  strToDateTimeRange(char *str);

extern SizeRange
  strToSizeRange(char *str);

extern DateTime
  strsecsToDateTime(char *val);

extern int
  strToBool(char *val);

extern void
  sizeToStr(Size sizeval, char *cvnbuf);

extern int
  datecmp(Date d1, Date d2);

extern int
  timecmp(Time t1, Time t2);

extern int
  datetimecmp(DateTime dt1, DateTime dt2);

extern DateTime
  datetimeGet(void);

extern int
  datetimeToSecs(DateTime dt);

extern int
  sizecmp(Size a, Size w);

extern Size
  sizeAdd(Size a, Size w);

extern Size
  sizeSub(Size a, Size w);

extern Size
  sizeMul(Size a, Size w);

extern Size
  sizeDiv(Size a, Size w);

extern Size
  sizeUminus(Size sz);

extern int
  dynamicArraySize(void *array);

extern void *
  initDynamicArray(size_t numElems, size_t elementSize);

extern void *
  extendDynamicArray(void *array, size_t numElems, size_t elementSize);

extern void
  printDynamicArrayTable(void);

extern void
  freeDynamicArray(void *array);

extern void
  datePrint(Date d);

extern void
  timePrint(Time t);

extern void
  datetimePrint(DateTime dt);

extern void
  sizePrint(Size s, int readable);

extern void
  intRangePrint(IntRange r);

extern void
  floatRangePrint(FloatRange r);

extern void
  dayofweekPrint(Dayofweek dow);

extern void
  actPrint(Action act);

extern void
  cprPrint(Comp cpr);

extern void
  dayofweekRangePrint(DayofweekRange r);

extern void
  dateRangePrint(DateRange d);

extern void
  timeRangePrint(TimeRange t);

extern void
  datetimeRangePrint(DateTimeRange dt);

extern void
  sizeRangePrint(SizeRange s, int readable);

extern int
  sizeRangecmp(SizeRange r1, SizeRange r2);

extern int
  sizeStrcmp(char *a, char *w);

extern int
  sizeRangeStrcmp(char *str1, char *str2);

extern IntRange
  toIntRange(int i1, int i2);

extern FloatRange
  toFloatRange(double f1, double f2);

extern DayofweekRange
  toDayofweekRange(Dayofweek dow1, Dayofweek dow2);

extern DateRange
  toDateRange(Date d1, Date d2);

extern TimeRange
  toTimeRange(Time t1, Time t2);

extern DateTimeRange
  toDateTimeRange(DateTime dt1, DateTime dt2);

extern SizeRange
  toSizeRange(Size sz1, Size sz2);

extern char *
  strCat(char *str1, char *str2);

extern void
  varstrAdd(void *ptr, int scope, void *pptr);

extern void
  varstrRemove(void *ptr);

extern void
  varstrModScope(void *ptr, int newscope);

extern void
  varstrModPptr(void *ptr, void *newpptr);

extern void
  varstrFree(void *ptr);

extern void
  varstrFreeByScope(int scope);

extern void
  varstrFreeByPptr(void *pptr);

extern void
  varstrPrint(void);

extern void
  mallocTableAdd(void *ptr, void *pptr, int scope);

extern void
  mallocTablePrint(void);

extern void
  mallocTableFree(void *ptr);

extern void
  mallocTableFreeByPptr(void *pptr);

extern void
  mallocTableFreeByScope(int scope, void (*freefunc)());

extern void
  mallocTableModScope(void *ptr, int scope);

extern void
  mallocTableSafeModScope(void *ptr, int scope);

extern int
  inMallocTable(void *ptr);

extern int
  normalizeSize(Size *a, Size *b, Size *ta, Size *tb);

/* functions related to IntRes and SizeRes */

extern int
  IntResValueGet(struct IntRes *head, char *name);

extern struct IntRes *
        IntResValuePut(struct IntRes *head, char *name, int value, void *pptr);

extern void
  IntResListPrint(struct IntRes *head, char *descr);

extern void
  IntResListFree(struct IntRes *head);

extern Size
  SizeResValueGet(struct SizeRes *head, char *name);

extern struct SizeRes *
        SizeResValuePut(struct SizeRes *head, char *name, Size value, void *pptr);

extern void
  SizeResListPrint(struct SizeRes *head, char *descr);

extern void
  SizeResListFree(struct SizeRes *head);

extern char *
  StringResValueGet(struct StringRes *head, char *name);

extern struct StringRes *
        StringResValuePut(struct StringRes *head, char *name, char *value,
                              void *pptr);

extern void
  StringResListPrint(struct StringRes *head, char *descr);

extern void
  StringResListFree(struct StringRes *head);

#endif  /* _AF_H */
