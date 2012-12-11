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

#include <pbs_config.h>   /* the master config generated by configure */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif  /* _POSIX_SOURCE */

/* System headers */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* local headers */
#include "portability.h"
#include "af_cnodemap.h"
#include "log.h"

/* Macros */
/* File Scope Variables */
static char ident[] = "@(#) $RCSfile$ $Revision$";

static struct    Resource    *Res = NULL;
/* list of resources as specified in */
/* job requirement or mom query */
/* External Variables */
/* External Functions */

/* Structures and Unions */
/* NONE */
/* Signal catching functions */
/* NONE */
/* Functions */

/* nodeAttrCmpNoTag: compares 2 attributes (ignoring the tags for */
/*              vector attributes) and returns 0 if they are the same, */
/*  NOTE: This will understand if it is comparing 2 vector */
/*        attributes. */
/*        Ex. a no-tag CNodeDiskSpaceReserved[] will match */
/*     a tagged CNodeDiskSpaceReserved[fast] */
int
nodeAttrCmpNoTag(char *attr1, char *attr2)
  {
  char *str1 = NULLSTR;
  char *str2 = NULLSTR;
  char *name1, *name2;
  char *ptr1, *ptr2, *ptr3;
  int  namecmp;
  int  retval = 1;

  assert(attr1 != NULLSTR && attr2 != NULLSTR);

#ifdef DEBUG
  printf("dynamic_strcpy(%x,%x=%s)\n", &str1, attr1, attr1);
#endif
  dynamic_strcpy(&str1, attr1); /* make a copy */

  name1 = str1;
  ptr1 = strchr(str1, '[');

  if (ptr1 != NULLSTR)
    {
    *ptr1 = '\0';
    ptr2 = ptr1 + 1;
    ptr3 = strchr(ptr2, ']');

    if (ptr3 == NULLSTR)    /* no matching right bracket */
      {
      varstrFree(str1);
      return(1); /* str1 not properly constructed */
      }

    ptr3++;

    if (*ptr3 != '\0')    /* extra characters in the end*/
      {
      varstrFree(str1);
      return(1); /* str1 not properly constructed */
      }

    }

#ifdef DEBUG
  printf("dynamic_strcpy(%x,%x=%s)\n", &str2, attr2, attr2);

#endif
  dynamic_strcpy(&str2, attr2); /* make a copy */

  name2 = str2;

  ptr1 = strchr(str2, '[');

  if (ptr1 != NULLSTR)
    {
    *ptr1 = '\0';
    ptr2 = ptr1 + 1;
    ptr3 = strchr(ptr2, ']');

    if (ptr3 == NULLSTR)    /* no matching right bracket */
      {
      varstrFree(str1);
      varstrFree(str2);
      return(1); /* str2 not properly constructed */
      }

    ptr3++;

    if (*ptr3 != '\0')    /* no matching right bracket */
      {
      varstrFree(str1);
      varstrFree(str2);
      return(1); /* str2 not properly constructed */
      }
    }

  /* at this point, we have name2[].member2 or simply ptr1 */

  /* Compare names */
  if (name1 != NULLSTR && name2 != NULLSTR && \
      *name1 != '\0' && *name2 != '\0')
    {
    namecmp = strcmp(name1, name2);

    if (namecmp == 0)
      {
      retval = 0;
      }
    }

  varstrFree(str1);

  varstrFree(str2);
  return(retval);
  }

/* parseAttrForTag: given an attribute of the form "name[tag]", return
   "tag". If no tag, NULL is returned.
   Storage returned is malloc-ed. Be sure to call varstrFree. */

char *
parseAttrForTag(char *attName)
  {
  char *str = NULLSTR;
  char *nstr;
  char *tstr;
  char *rstr = NULLSTR; /* return string */

  assert(attName != NULLSTR);

  dynamic_strcpy(&str, attName); /* make a copy */

  nstr = (char *)strchr(str, '[');

  if (nstr == NULLSTR)          /* did not find a left bracket */
    {
    varstrFree(str);
    return(NULLSTR);
    }

  nstr++;

  tstr = (char *)strchr(nstr, ']');

  if (tstr == NULLSTR)
    {
    varstrFree(str);
    return(NULLSTR);
    }

  *tstr = '\0';

  dynamic_strcpy(&rstr, nstr);
  varstrFree(str); /* must call free from the original */
  return(rstr);
  }

int
getAttrType(char *attName)
  {
  int i = 0;
  assert(attName != NULLSTR);

  while (static_attrinfo_map[i].name != NULLSTR)
    {
    if (nodeAttrCmpNoTag(static_attrinfo_map[i].name, attName) == 0)
      return(static_attrinfo_map[i].type);

    i++;
    }

  i = 0;

  while (dynamic_attrinfo_map[i].name != NULLSTR)
    {
    if (nodeAttrCmpNoTag(dynamic_attrinfo_map[i].name, attName) == 0)
      return(dynamic_attrinfo_map[i].type);

    i++;
    }

  return(-1);
  }

void (*
      getAttrPutFunc(char *attName))(void)
  {
  int i = 0;

  assert(attName != NULLSTR);

  while (static_attrinfo_map[i].name != NULLSTR)
    {
    if (nodeAttrCmpNoTag(static_attrinfo_map[i].name, attName) == 0)
      return(static_attrinfo_map[i].attrPutFunc);

    i++;
    };

  i = 0;

  while (dynamic_attrinfo_map[i].name != NULLSTR)
    {
    if (nodeAttrCmpNoTag(dynamic_attrinfo_map[i].name, attName) == 0)
      return(dynamic_attrinfo_map[i].attrPutFunc);

    i++;
    };

  return(NULL);
  }

/* The following 2 functions reads the elements of array at 'index'. */
/* NOTE: This returns NULL if end of the array is reached. */
char *
getStaticAttrAtIndex(int index, int *type, void (**putfunc)(void))

  {
  char   *val;

  if ((val = static_attrinfo_map[index].name) != NULLSTR)
    {
    *type     = static_attrinfo_map[index].type;
    *putfunc  = static_attrinfo_map[index].attrPutFunc;
    }

  return(val);
  }

char *
getDynamicAttrAtIndex(int index, int *type, void (**putfunc)(void))
  {
  char *val;

  if ((val = dynamic_attrinfo_map[index].name) != NULLSTR)
    {
    *type     = dynamic_attrinfo_map[index].type;
    *putfunc  = dynamic_attrinfo_map[index].attrPutFunc;
    }

  return(val);
  }

void attrInfoMapPrint(void)
  {
  int    cnt;
  char   *attrib;
  int    type;
  void (*putfunc)();

  cnt = 0;

  while ((attrib = getStaticAttrAtIndex(cnt, &type, &putfunc)) != NULLSTR)
    {
    printf("ATTRIB: %s TYPE: %d PUTFUNC: %d\n", attrib, type, putfunc);
    cnt++;
    }

  cnt = 0;

  while ((attrib = getDynamicAttrAtIndex(cnt, &type, &putfunc)) != NULLSTR)
    {
    printf("ATTRIB: %s TYPE: %d PUTFUNC: %d\n",
           attrib, type, putfunc);
    cnt++;
    }
  }

/* addRes: add the specified 4-tuple:

        <archType, nodeAttr, hostQuery_keyword>

   to the global static dynamic array Res. If an entry in the array exists
   having the same <archType, nodeAttr,> value, then that entry's
   <,,hostQuery_keyword> is updated to the NEW value. Otherwise, a new entry
   is added. */
int
addRes(char *archType, char *nodeAttr, char *hostQuery)
  {
  static  char    id[] = "addRes";
  int     i;
  int     beforecnt, aftercnt;


  assert(archType != NULLSTR && nodeAttr != NULLSTR && \
         hostQuery != NULLSTR);


  /*      if an entry already exists, then simply modify it */
  beforecnt = dynamicArraySize(Res);

  for (i = 0; i < beforecnt; i++)
    {
    if (STRCMP(Res[i].archType, == , archType) && \
        STRCMP(Res[i].nodeAttr, == , nodeAttr))
      {

      if (STRCMP(Res[i].hostQuery_keyword, != , hostQuery))
        {
        (void)sprintf(log_buffer,
                      "modified res(%s, %s, %s->%s)\n",
                      archType, nodeAttr,
                      Res[i].hostQuery_keyword ? Res[i].hostQuery_keyword : "null",
                      hostQuery ? hostQuery : "null");

        dynamic_strcpy(&Res[i].hostQuery_keyword, hostQuery);
        varstrModPptr(Res[i].hostQuery_keyword, (void *)1);

        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        return(0);
        }
      else   /* same hostQuery, must be another job Require */
        {

        (void)sprintf(log_buffer, "ignored duplicate res(%s, %s, %s)\n",
                      archType, nodeAttr,
                      hostQuery ? hostQuery : "null");
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   log_buffer);
        return(0);
        }
      }
    }

  Res = (struct Resource *)extendDynamicArray(Res, (size_t) beforecnt + 1,

        sizeof(struct Resource));

  aftercnt = dynamicArraySize(Res);

  if (beforecnt == aftercnt)
    {
    log_err(-1, id, (char *)"Unable to realloc Res");
    return(-1);
    }

  Res[beforecnt].archType = NULLSTR;

  Res[beforecnt].nodeAttr = NULLSTR;
  Res[beforecnt].hostQuery_keyword  = NULLSTR;

  dynamic_strcpy(&Res[beforecnt].archType, archType);
  dynamic_strcpy(&Res[beforecnt].nodeAttr, nodeAttr);
  dynamic_strcpy(&Res[beforecnt].hostQuery_keyword,  hostQuery);

  varstrModPptr(Res[beforecnt].archType, (void *)1);
  varstrModPptr(Res[beforecnt].nodeAttr, (void *)1);
  varstrModPptr(Res[beforecnt].hostQuery_keyword, (void *)1);

  (void)sprintf(log_buffer, "added res(%s, %s, %s)", archType,
                nodeAttr, hostQuery ? hostQuery : "null");
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
  return(0);
  }

/* returns the index to resptrs that contains 'nodeAttr'. Otherwise, -1 is
   returned. */
static int
findResPtrGivenNodeAttr(struct Resource **resptrs, char *nodeAttr)
  {
  int k;

  for (k = 0; k < dynamicArraySize(resptrs); k++)
    {
    if (STRCMP(getNodeAttrGivenResPtr(resptrs[k]), == , nodeAttr))
      return(k);
    }

  return(-1);
  }

/* getResPtr: returns an array of ptrs to Res containing entries that match
                <archType,nodeAttr,,> with a non-NULLSTR or non-"" entry for
  hostQuery_keyword.  */

struct Resource **
      getResPtr(char *archType, char *nodeAttr)
  {
  int i, j, k;

  struct Resource **resptrs = NULL;   /* a dynamic array of ptrs to Res */

  assert(archType != NULLSTR && nodeAttr != NULLSTR);

  for (i = 0, j = 0; i < dynamicArraySize(Res); i++)
    {

    if ((STRCMP(Res[i].archType, == , archType) || \
         STRCMP(Res[i].archType, == , "*")) && \
        nodeAttrCmpNoTag(nodeAttr, Res[i].nodeAttr) == 0 && \
        Res[i].hostQuery_keyword != NULLSTR && \
        *Res[i].hostQuery_keyword != '\0' && \
        STRCMP(Res[i].hostQuery_keyword, != , "\"\""))
      {

      k = findResPtrGivenNodeAttr(resptrs, nodeAttr);

      if (k == -1)
        {
        resptrs = (struct Resource **)extendDynamicArray(resptrs,
                  (size_t) j + 1, sizeof(struct Resource *));
        resptrs[j] = &Res[i];
        j++;
        }
      else
        {
        resptrs[k] = &Res[i];
        }
      }
    }

  return(resptrs);
  }

char *
getNodeAttrGivenResPtr(struct Resource *resptr)
  {
  return(resptr->nodeAttr);
  }

char *
getHostQueryKeywordGivenResPtr(struct Resource *resptr)
  {
  return(resptr->hostQuery_keyword);
  }

void ResPrint(void)
  {
  int i;

  printf("Res Table %x\n", Res);

  for (i = 0; i < dynamicArraySize(Res); i++)
    {
    (void)printf("%s %s %s\n", Res[i].archType, Res[i].nodeAttr,
                 Res[i].hostQuery_keyword ? Res[i].hostQuery_keyword : "null");
    }
  }

void ResFree(void)
  {
  varstrFreeByPptr((void *)1); /* free up all the strings associated with Res*/
  freeDynamicArray(Res);
  Res = NULL;
  }
