#include "license_pbs.h" /* See here for the software license */

#include "mcom.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* prototypes */

int MXMLGetChild(mxml_t *, char *, int *, mxml_t **);
int MXMLAddE(mxml_t *, mxml_t *);
int MXMLGetAttrF(mxml_t *, char *, int *, void *, enum MDataFormatEnum, int);
int MXMLGetAttr(mxml_t *, char *, int *, char *, int);
int MXMLToString(mxml_t *, char *, int, char **, mbool_t);

/* END prototypes */





int MXMLExtractE(

  mxml_t  *E,  /* I */
  mxml_t  *C,  /* I */
  mxml_t **CP) /* O (optional) */

  {
  int cindex;

  if ((E == NULL) || (C == NULL))
    {
    return(FAILURE);
    }

  for (cindex = 0;cindex < E->CCount;cindex++)
    {
    if (C != E->C[cindex])
      {
      if (MXMLExtractE(E->C[cindex], C, CP) == SUCCESS)
        {
        return(SUCCESS);
        }

      continue;
      }

    if (CP != NULL)
      *CP = E->C[cindex];

    E->C[cindex] = NULL;

    return(SUCCESS);
    }  /* END for (cindex) */

  return(FAILURE);
  }  /* MXMLExtractE() */





int MXMLSetChild(

  mxml_t  *E,     /* I */
  char    *CName, /* I */
  mxml_t **CE)    /* O */

  {
  if (CE == NULL)
    return(FAILURE);

  if (CE != NULL)
    *CE = NULL;

  if ((E == NULL) || (CName == NULL))
    {
    return(FAILURE);
    }

  if (MXMLGetChild(E, CName, NULL, CE) == SUCCESS)
    {
    /* located existing child */

    return(SUCCESS);
    }

  /* create new child */

  if ((*CE = (mxml_t *)calloc(1, sizeof(mxml_t))) == NULL)
    {
    return(FAILURE);
    }

  MXMLAddE(E, *CE);

  if (CName != NULL)
    {
    (*CE)->Name = strdup(CName);

    if ((*CE)->Name == NULL)
      {
      return(FAILURE);
      }
    }

  return(SUCCESS);
  }  /* END MXMLSetChild() */




int MXMLCreateE(

  mxml_t **E,    /* O */
  char    *Name) /* I (optional) */

  {
  /* NOTE:  should 'Name' be mandatory? */

  if (E == NULL)
    {
    return(FAILURE);
    }

  if ((*E = (mxml_t *)calloc(1, sizeof(mxml_t))) == NULL)
    {
    return(FAILURE);
    }

  if ((Name != NULL) && (Name[0] != '\0'))
    {
    (*E)->Name = strdup(Name);

    if ((*E)->Name == NULL)
      {
      return(FAILURE);
      }
    }

  return(SUCCESS);
  }  /* END MXMLCreateE() */




int MXMLDestroyE(

  mxml_t **EP)  /* I (modified) */

  {
  int index;

  mxml_t *E;

  if (EP == NULL)
    {
    return(FAILURE);
    }

  E = *EP;

  if (E == NULL)
    {
    return(SUCCESS);
    }

  if (E->C != NULL)
    {
    /* destroy children */

    for (index = 0;index < E->CCount;index++)
      {
      if (E->C[index] == NULL)
        continue;

      MXMLDestroyE(&E->C[index]);
      }  /* END for (index) */

    free(E->C);
    }  /* END if (E->C != NULL) */

  /* free attributes */

  if (E->AName != NULL)
    {
    for (index = 0;index < E->ACount;index++)
      {
      if (E->AName[index] == NULL)
        break;

      free(E->AName[index]);

      if ((E->AVal != NULL) && (E->AVal[index] != NULL))
        free(E->AVal[index]);
      }  /* END for (index) */

    if (E->AVal != NULL)
      {
      free(E->AVal);
      }

    if (E->AName != NULL)
      {
      free(E->AName);
      }
    }    /* END if (E->AName != NULL) */

  /* free name */

  if (E->Name != NULL)
    free(E->Name);

  if (E->Val != NULL)
    free(E->Val);

  free(E);

  *EP = NULL;

  return(SUCCESS);
  }  /* END MXMLDestroyE() */





int MXMLSetAttr(

  mxml_t *E,       /* I (modified) */
  char   *A,       /* I */
  void   *V,       /* I */
  enum MDataFormatEnum Format)  /* I */

  {
  int  aindex;
  int  iindex;

  int  rc;

  char tmpLine[MMAX_LINE];

  char *ptr;

  /* NOTE:  overwrite existing attr if found */

  if ((E == NULL) || (A == NULL) || (V == NULL))
    {
    return(FAILURE);
    }

  switch (Format)
    {

    case mdfString:

    default:

      ptr = (char *)V;

      break;

    case mdfInt:

      sprintf(tmpLine, "%d",
        *(int *)V);

      ptr = tmpLine;

      break;

    case mdfLong:

      sprintf(tmpLine, "%ld",
        *(long *)V);

      ptr = tmpLine;

      break;

    case mdfDouble:

      sprintf(tmpLine, "%f",
        *(double *)V);

      ptr = tmpLine;

      break;
    }  /* END switch (Format) */

  /* initialize attribute table */

  if (E->AName == NULL)
    {
    if ((E->AName = (char **)calloc(1, sizeof(char *) * MMAX_XMLATTR)) == NULL)
      {
      fprintf(stderr,"ERROR: calloc() failed!\n");
       
      return(FAILURE);
      }

    if ((E->AVal  = (char **)calloc(1, sizeof(char *) * MMAX_XMLATTR)) == NULL)
      {
      fprintf(stderr,"ERROR: calloc() failed!\n");

      return(FAILURE);
      }

    E->ASize = MMAX_XMLATTR;
    E->ACount = 0;
    }  /* END if (E->AName == NULL) */

  /* insert in alphabetical order */

  /* overwrite existing attribute if found */

  iindex = 0;

  rc     = 0;

  for (aindex = 0;aindex < E->ACount;aindex++)
    {
    rc = strcmp(E->AName[aindex], A);

    if (rc > 0)
      break;

    if (rc == 0)
      {
      iindex = aindex;

      break;
      }

    iindex = aindex + 1;
    }  /* END for (aindex) */

  if (aindex >= E->ACount)
    {
    iindex = aindex;

    if (aindex >= E->ASize)
      {
      char **tmpAName;
      char **tmpAVal;

      /* allocate memory */

      tmpAName = (char **)realloc(E->AName,sizeof(char *) * MAX(16,E->ASize << 1));

      if (tmpAName == NULL)
        {
        /* FAILURE - cannot allocate memory */

        return(FAILURE);
        }

      E->AName = tmpAName;
     
      tmpAVal = (char **)realloc(E->AVal,sizeof(char *) * MAX(16,E->ASize << 1));

      if (tmpAVal == NULL)
        {
        /* FAILURE - cannot allocate memory */

        return(FAILURE);
        }

      E->AVal = tmpAVal;
 

      E->ASize <<= 1;
      }  /* END if (aindex >= E->ASize) */
    }    /* END if (aindex >= E->ACount) */

  if ((ptr == NULL) && (aindex >= E->ACount))
    {
    /* no action required for empty attribute */

    return(SUCCESS);
    }

  /* prepare insertion point */

  if (rc != 0)
    {
    for (aindex = E->ACount - 1;aindex >= iindex;aindex--)
      {
      E->AVal[aindex + 1]  = E->AVal[aindex];
      E->AName[aindex + 1] = E->AName[aindex];
      }  /* END for (aindex) */

    E->AVal[aindex + 1]  = NULL;

    E->AName[aindex + 1] = NULL;
    }  /* END if (rc != 0) */

  if (E->AVal[iindex] != NULL)
    free(E->AVal[iindex]);

  E->AVal[iindex] = strdup((ptr != NULL) ? ptr : "");

  if (E->AVal[iindex] == NULL)
    {
    /* FAILURE - cannot alloc memory */

    return(FAILURE);
    }

  if ((rc != 0) || (E->AName[iindex] == NULL))
    {
    E->AName[iindex] = strdup(A);

    if (E->AName[iindex] == NULL)
      {
      /* FAILURE - cannot alloc memory */

      return(FAILURE);
      }

    E->ACount++;
    }

  return(SUCCESS);
  }  /* END MXMLSetAttr() */




int MXMLAppendAttr(

  mxml_t *E,      /* I root element */
  char   *AName,  /* I */
  char   *AVal,   /* I */
  char    Delim)  /* I */

  {
  int   ATok;
  int   len;

  char  VBuf[MMAX_LINE];

  if ((E == NULL) || (AName == NULL) || (AVal == NULL))
    {
    return(FAILURE);
    }

  ATok = -1;

  if (MXMLGetAttr(E, AName, &ATok, VBuf, sizeof(VBuf)) == SUCCESS)
    {
    char DString[2];

    char *tmpAVal;

    len = strlen(E->AVal[ATok]) + strlen(AVal) + 2;

    tmpAVal = realloc(E->AVal[ATok], len);

    if (tmpAVal == NULL)
      {
      return(FAILURE);
      }

    E->AVal[ATok] = tmpAVal;

    DString[0] = Delim;

    DString[1] = '\0';

    /* just allocated adequate space, should not be able to overflow */

    strcat(E->AVal[ATok], DString);

    strcat(E->AVal[ATok], AVal);
    }
  else if (MXMLSetAttr(E, AName, AVal, mdfString) == FAILURE)
    {
    return(FAILURE);
    }

  return(SUCCESS);
  }  /* END MXMLAppendAttr() */





int MXMLSetVal(

  mxml_t *E,      /* I (modified) */
  void   *V,      /* I */
  enum MDataFormatEnum Format) /* I */

  {
  char *outbuf = NULL;
  char *ptr;
  char  tmpLine[MMAX_LINE];

  int Vlen;
  int final_len = 0;
  int buf_size = MMAX_LINE;
  int i;

  if ((E == NULL) || (V == NULL))
    {
    return(FAILURE);
    }

  if (E->Val != NULL)
    {
    free(E->Val);

    E->Val = NULL;
    }

  switch (Format)
    {

    case mdfString:

    default:

      /* temporarily use ptr as the input string */
      ptr = (char *)V;
      Vlen = strlen(ptr);
    
      outbuf = calloc(buf_size, (sizeof(char)));
      
      if (outbuf == NULL)
        {
        fprintf(stderr,"Unable to allocate memory for XML output\n");
        
        return(FAILURE);
        }

      for (i = 0; i < Vlen; i++)
        {
        /* escape characters &"'<> */

        /* check 7 because we could add 6 character and a NULL terminator */
        if (final_len + 7 >= buf_size)
          {
          char *tmp;

          buf_size += MMAX_LINE;
          tmp = (char *)realloc(outbuf,buf_size);

          if (tmp == NULL)
            {
            fprintf(stderr,"Unable to allocate memeory for XML output\n");
            free(outbuf);
            return(FAILURE);
            }

          outbuf = tmp;
          }

        /* escape characters &"'<> */
        switch (ptr[i])
          {
          case '<':

            outbuf[final_len++] = '&';
            outbuf[final_len++] = 'l';
            outbuf[final_len++] = 't';
            outbuf[final_len++] = ';';

            break;

          case '>':

            outbuf[final_len++] = '&';
            outbuf[final_len++] = 'g';
            outbuf[final_len++] = 't';
            outbuf[final_len++] = ';';

            break;
            
          case '&':

            outbuf[final_len++] = '&';
            outbuf[final_len++] = 'a';
            outbuf[final_len++] = 'm';
            outbuf[final_len++] = 'p';
            outbuf[final_len++] = ';';

            break;

          case '"':
  
            outbuf[final_len++] = '&';
            outbuf[final_len++] = 'q';
            outbuf[final_len++] = 'u';
            outbuf[final_len++] = 'o';
            outbuf[final_len++] = 't';
            outbuf[final_len++] = ';';

            break;

          case '\'':

            outbuf[final_len++] = '&';
            outbuf[final_len++] = 'a';
            outbuf[final_len++] = 'p';
            outbuf[final_len++] = 'o';
            outbuf[final_len++] = 's';
            outbuf[final_len++] = ';';

            break;

          default:
            
            outbuf[final_len++] = ptr[i];

            break;
          } /* END switch(ptr[i]) */

        } /* END for (i < Vlen) */

      outbuf[final_len] = '\0';

      break;

    case mdfInt:

      sprintf(tmpLine, "%d",
              *(int *)V);

      ptr = tmpLine;

      break;

    case mdfLong:

      sprintf(tmpLine, "%ld",
              *(long *)V);

      ptr = tmpLine;

      break;

    case mdfDouble:

      sprintf(tmpLine, "%f",
              *(double *)V);

      ptr = tmpLine;

      break;
    }  /* END switch (Format) */

  if (outbuf != NULL)
    {
    E->Val = outbuf;
    }
  else
    {
    E->Val = strdup(ptr);
    }

  return(SUCCESS);
  }  /* END MXMLSetVal() */




int MXMLAddE(

  mxml_t *E, /* I (modified) */
  mxml_t *C) /* I (required) */

  {
  if ((E == NULL) || (C == NULL))
    {
    return(FAILURE);
    }

  if (E->CCount >= E->CSize)
    {
    if (E->C == NULL)
      {
      E->C = (mxml_t **)calloc(1, sizeof(mxml_t *) * MDEF_XMLICCOUNT);

      E->CSize = MDEF_XMLICCOUNT;
      }
    else
      {
      mxml_t **tmpC;

      tmpC = (mxml_t **)realloc(E->C,sizeof(mxml_t *) * MAX(16,E->CSize << 1));
 
      if (tmpC == NULL)
        {
        /* FAILURE - cannot alloc memory */

        return(FAILURE);
        }

      E->C = tmpC;
      E->CSize <<= 1;
      }

    if (E->C == NULL)
      {
      /* FAILURE - cannot alloc memory */

      return(FAILURE);
      }  /* END if (E->C == NULL) */
    }    /* END if (E->CCount >= E->CSize) */

  E->C[E->CCount] = C;

  E->CCount++;

  return(SUCCESS);
  }  /* END MXMLAddE() */





/**
 * NOTE:  Buf may be allocated on FAILURE 
 */

int MXMLToXString(

  mxml_t   *E,             /* I */
  char    **Buf,           /* O (alloc/populated/modified) */
  int      *BufSize,       /* I/O */
  int       MaxBufSize,    /* I */
  char    **Tail,          /* O */
  mbool_t   IsRootElement) /* I */

  {
  int NewSize;

  /* NOTE:  MXMLToString() only fails due to lack of space */

  if ((E == NULL) || (Buf == NULL))
    {
    return(FAILURE);
    }

  /* allocate initial memory if required */

  if (*Buf == NULL)
    {
    NewSize = MMAX_BUFFER;

    if ((*Buf = calloc(1, NewSize)) == NULL)
      {
      /* cannot allocate buffer */

      return(FAILURE);
      }

    if (BufSize != NULL)
      *BufSize = MMAX_BUFFER;
    }
  else
    {
    if (BufSize == NULL)
      {
      return(FAILURE);
      }

    NewSize = *BufSize;
    }

  while (MXMLToString(
           E,
           *Buf,
           NewSize,
           Tail,
           IsRootElement) == FAILURE)
    {
    char *tmpBuf;

    if (NewSize >= MaxBufSize)
      {
      return(FAILURE);
      }

    NewSize = MIN(NewSize << 1, MaxBufSize);

    tmpBuf = (char *)realloc(*Buf,NewSize);

    if (tmpBuf == NULL)
      {
      /* cannot allocate buffer */

      return(FAILURE);
      }

    *Buf = tmpBuf;

    if (BufSize != NULL)
      *BufSize = NewSize;
    }    /* END while (MXMLToString() == FAILURE) */

  return(SUCCESS);
  }  /* END MXMLToXString() */




int MXMLToString(

  mxml_t   *E,             /* I */
  char     *Buf,           /* O */
  int       BufSize,       /* I */
  char    **Tail,          /* O */
  mbool_t   IsRootElement) /* I */

  {
  int index;

  int BSpace;

  char *BPtr;

  char *tail;

  int   len;

  if (Buf != NULL)
    {
    Buf[0] = '\0';
    }

  if ((E == NULL) || (Buf == NULL))
    {
    return(FAILURE);
    }

  if (BufSize < MMAX_NAME)
    {
    return(FAILURE);
    }

  BPtr   = Buf;

  BSpace = BufSize;

  /* display header */

  BPtr[0] = '<';

  BPtr++;
  BSpace--;

  if (E->Name != NULL)
    {
    len = strlen(E->Name);

    if (len >= BSpace)
      {
      /* insufficient space */

      return(FAILURE);
      }

    strcpy(BPtr, E->Name);

    BSpace -= len;
    BPtr   += len;
    }
  else
    {
    strcpy(BPtr, "NA");

    len = strlen("NA");

    BPtr   += len;
    BSpace -= len;
    }

  /* display attributes */

  for (index = 0;index < E->ACount;index++)
    {
    /* FORMAT:  <NAME>="<VAL>" */

    BPtr[0] = ' ';

    BPtr++;
    BSpace--;

    len = strlen(E->AName[index]);

    if (len >= BSpace)
      {
      /* insufficient space */

      return(FAILURE);
      }

    strcpy(BPtr, E->AName[index]);

    BSpace -= len;
    BPtr   += len;

    BPtr[0] = '=';

    BPtr++;
    BSpace--;

    BPtr[0] = '"';

    BPtr++;
    BSpace--;

    len = strlen(E->AVal[index]);

    if (len >= BSpace)
      {
      /* insufficient space */

      return(FAILURE);
      }

    strcpy(BPtr, E->AVal[index]);

    BSpace -= len;
    BPtr   += len;

    BPtr[0] = '"';

    BPtr++;
    BSpace--;
    }  /* END for (index) */

  BPtr[0] = '>';

  BPtr++;

  BSpace--;

  if (E->Val != NULL)
    {
    len = strlen(E->Val);

    if (len >= BSpace)
      {
      /* insufficient space */

      return(FAILURE);
      }

    strcpy(BPtr, E->Val);

    BSpace -= len;
    BPtr   += len;
    }

  /* display children */

  for (index = 0;index < E->CCount;index++)
    {
    if (E->C[index] == NULL)
      continue;

    if (MXMLToString(E->C[index], BPtr, BSpace, &tail, FALSE) == FAILURE)
      {
      return(FAILURE);
      }

    len = strlen(BPtr);

    BSpace -= len;
    BPtr   += len;
    }  /* END for (index) */

  /* display footer */

  if (E->Name != NULL)
    {
    len = strlen(E->Name);
    }
  else
    {
    len = strlen("NA");
    }

  if (BSpace < len + 4)
    {
    return(FAILURE);
    }

  BPtr[0] = '<';

  BPtr++;
  BSpace--;

  BPtr[0] = '/';

  BPtr++;
  BSpace--;

  if (E->Name != NULL)
    {
    strcpy(BPtr, E->Name);
    }
  else
    {
    strcpy(BPtr, "NA");
    }

  BSpace -= len;

  BPtr   += len;

  BPtr[0] = '>';

  BPtr++;
  BSpace--;

  /* terminate string */

  BPtr[0] = '\0';

  if (Tail != NULL)
    *Tail = BPtr;

  return(SUCCESS);
  }  /* END MXMLToString() */




int MXMLGetAttrF(

  mxml_t *E,     /* I */
  char   *AName, /* I/O */
  int    *ATok,  /* I (optional) */
  void   *AVal,  /* O */
  enum MDataFormatEnum DFormat, /* I */
  int     VSize) /* I */

  {
  char tmpLine[MMAX_LINE];
  int  rc;

  if (AVal == NULL)
    {
    return(FAILURE);
    }

  switch (DFormat)
    {

    case mdfString:

      rc = MXMLGetAttr(
             E,
             AName,
             ATok,
             (char *)AVal,
             VSize);

      return(rc);

      /* NOTREACHED*/

      break;

    case mdfInt:

    case mdfLong:

    case mdfDouble:

      if (MXMLGetAttr(
            E,
            AName,
            ATok,
            tmpLine,
            sizeof(tmpLine)) == FAILURE)
        {
        /* clear response */

        switch (DFormat)
          {

          case mdfInt:
            *(int *)AVal    = 0;
            break;

          case mdfLong:
            *(long *)AVal   = 0;
            break;

          case mdfDouble:
            *(double *)AVal = 0.0;
            break;

          default:
            break;
          }  /* END switch(DFormat) */

        return(FAILURE);
        }

      break;

    default:

      return(FAILURE);

      /*NOTREACHED*/

      break;
    }  /* END switch(DFormat) */

  switch (DFormat)
    {

    case mdfInt:

      *(int *)AVal = (int)strtol(tmpLine, NULL, 10);

      break;

    case mdfLong:

      *(long *)AVal = strtol(tmpLine, NULL, 10);

      break;

    case mdfDouble:

      *(double *)AVal = strtod(tmpLine, NULL);

      break;

    default:

      *(char **)AVal = NULL;

      return(FAILURE);

      /*NOTREACHED*/

      break;
    }  /* END switch (DFormat) */

  return(SUCCESS);
  }  /* END MXMLGetAttrF() */






int MXMLGetAttr(

  mxml_t *E,     /* I */
  char   *AName, /* I/O */
  int    *ATok,  /* I (optional) */
  char   *AVal,  /* O (optional) */
  int     VSize) /* I */

  {
  /* NOTE:  set AName to empty string to get Name */

  int aindex;
  int astart;

  int EVSize;

  if (AVal != NULL)
    AVal[0] = '\0';

  if (E == NULL)
    {
    return(FAILURE);
    }

  EVSize = (VSize > 0) ? VSize : MMAX_LINE;

  if (ATok != NULL)
    astart = *ATok;
  else
    astart = -1;

  for (aindex = astart + 1;aindex < E->ACount;aindex++)
    {
    if ((AName == NULL) ||
        (AName[0] == '\0') ||
        !strcmp(AName, E->AName[aindex]))
      {
      if ((AName != NULL) && (AName[0] == '\0'))
        {
        strncpy(AName, E->AName[aindex], MMAX_NAME);
        AName[MMAX_NAME - 1] = '\0';
        }

      if (AVal != NULL)
        {
        strncpy(AVal, E->AVal[aindex], EVSize);
        AVal[EVSize - 1] = '\0';
        }

      if (ATok != NULL)
        *ATok = aindex;

      return(SUCCESS);
      }
    }    /* END for (aindex) */

  return(FAILURE);
  }  /* END MXMLGetAttr() */




int MXMLGetChild(

  mxml_t  *E,     /* I */
  char    *CName, /* I (optional) */
  int     *CTok,  /* I (optional) */
  mxml_t **C)     /* O */

  {
  int cindex;
  int cstart;

  if (C != NULL)
    *C = NULL;

  if ((E == NULL) || (C == NULL))
    {
    return(FAILURE);
    }

  if (CTok != NULL)
    cstart = *CTok;
  else
    cstart = -1;

  for (cindex = cstart + 1;cindex < E->CCount;cindex++)
    {
    if (E->C[cindex] == NULL)
      continue;

    if ((CName == NULL) || !strcmp(CName, E->C[cindex]->Name))
      {
      *C = E->C[cindex];

      if (CTok != NULL)
        *CTok = cindex;

      return(SUCCESS);
      }
    }    /* END for (cindex) */

  return(FAILURE);
  }  /* END MXMLGetChild() */




int MXMLGetChildCI(

  mxml_t  *E,     /* I */
  char    *CName, /* I (optional) */
  int     *CTok,  /* I (optional) */
  mxml_t **CP)    /* O (optional) */

  {
  int cindex;
  int cstart;

  int SLen;

  if (CP != NULL)
    *CP = NULL;

#ifndef __MOPT
  if (E == NULL)
    {
    return(FAILURE);
    }

#endif /* __MOPT */

  if (CTok != NULL)
    cstart = *CTok;
  else
    cstart = -1;

  if (CName != NULL)
    SLen = strlen(CName) + 1;
  else
    SLen = 0;

  for (cindex = cstart + 1;cindex < E->CCount;cindex++)
    {
    if (E->C[cindex] == NULL)
      continue;

    if ((CName == NULL) || !strncasecmp(CName, E->C[cindex]->Name, SLen))
      {
      if (CP != NULL)
        *CP = E->C[cindex];

      if (CTok != NULL)
        *CTok = cindex;

      return(SUCCESS);
      }
    }    /* END for (cindex) */

  return(FAILURE);
  }  /* END MXMLGetChildCI() */





#define MMAX_XBUFFER 131072

int MXMLFromString(

  mxml_t **EP,        /* O (populate or create) */
  char    *XMLString, /* I */
  char   **Tail,      /* O (optional) */
  char    *EMsg)      /* O (optional) */

  {
  mxml_t  *E;
  char    *ptr;

  char    *tail;

  int      index;

  mbool_t  ElementIsClosed = FALSE;

  mbool_t  DoAppend = FALSE;

  char tmpNLine[MMAX_LINE + 1];
  char tmpVLine[MMAX_XBUFFER + 1];

  if (EP != NULL)
    *EP = NULL;

  if (EMsg != NULL)
    EMsg[0] = '\0';

  if ((XMLString == NULL) || (EP == NULL))
    {
    if (EMsg != NULL)
      strcpy(EMsg, "invalid arguments");

    return(FAILURE);
    }

  if ((ptr = strchr(XMLString, '<')) == NULL)
    {
    if (EMsg != NULL)
      strcpy(EMsg, "no XML in string");

    return(FAILURE);
    }

  if (ptr[1] == '/')
    {
    /* located tail marker */

    if (EMsg != NULL)
      strcpy(EMsg, "premature termination marker");

    return(FAILURE);
    }

  /* NOTE:  should support append/overlay parameter (NYI) */

  /* ignore 'meta' elements */

  while ((ptr[1] == '?') || (ptr[1] == '!'))
    {
    ptr++;

    /* ignore 'meta' elements */

    if (*ptr == '?')
      {
      ptr++;

      if ((ptr = strstr(ptr, "?>")) == NULL)
        {
        /* cannot locate end of meta element */

        return(FAILURE);
        }

      if ((ptr = strchr(ptr, '<')) == NULL)
        {
        /* cannot locate next element */

        if (EMsg != NULL)
          strcpy(EMsg, "cannot locate post-meta XML");

        return(FAILURE);
        }
      }    /* END if (*ptr == '?') */

    /* ignore 'comment' element */

    if (!strncmp(ptr, "!--", 3))
      {
      ptr += 3;

      if ((ptr = strstr(ptr, "-->")) == NULL)
        {
        /* cannot locate end of comment element */

        if (EMsg != NULL)
          strcpy(EMsg, "cannot locate comment termination marker");

        return(FAILURE);
        }

      if ((ptr = strchr(ptr, '<')) == NULL)
        {
        /* cannot locate next element */

        if (EMsg != NULL)
          strcpy(EMsg, "cannot locate post-comment XML");

        return(FAILURE);
        }
      }    /* END if (!strncmp(ptr,"!--",3)) */
    }      /* END while ((ptr[1] == '?') || (ptr[1] == '!')) */

  /* remove whitespace */

  while (isspace(*ptr))
    ptr++;

  /* extract root element */

  if (*ptr != '<')
    {
    /* cannot located start of element */

    if (EMsg != NULL)
      strcpy(EMsg, "cannot locate start of root element");

    return(FAILURE);
    }

  ptr++;  /* ignore '<' */

  index = 0;

  while ((*ptr != ' ') && (*ptr != '>'))
    {
    if ((ptr[0] == '/') && (ptr[1] == '>'))
      {
      ElementIsClosed = TRUE;

      break;
      }

    tmpNLine[index++] = *(ptr++);

    if ((index >= MMAX_LINE) || (ptr[0] == '\0'))
      {
      if (EMsg != NULL)
        sprintf(EMsg, "element name is too long - %.10s",
                tmpNLine);

      return(FAILURE);
      }
    }

  tmpNLine[index] = '\0';

  if ((*EP == NULL) && (MXMLCreateE(EP, tmpNLine) == FAILURE))
    {
    if (EMsg != NULL)
      sprintf(EMsg, "cannot create XML element '%s'",
              tmpNLine);

    return(FAILURE);
    }

  E = *EP;

  if ((E->ACount > 0) || (E->CCount > 0))
    {
    DoAppend = TRUE;
    }

  if (ElementIsClosed == TRUE)
    {
    ptr += 2; /* skip '/>' */

    if (Tail != NULL)
      *Tail = ptr;

    return(SUCCESS);
    }

  while (*ptr == ' ')
    ptr++;

  while (*ptr != '>')
    {
    /* extract attributes */

    /* FORMAT:  <ATTR>="<VAL>" */

    index = 0;

    while ((*ptr != '=') && (*ptr != '\0'))
      {
      tmpNLine[index++] = *(ptr++);

      if (index >= MMAX_LINE)
        break;
      }

    tmpNLine[index] = '\0';

    if (*ptr != '\0')
      ptr++;  /* skip '=' */

    if (*ptr != '\0')
      ptr++;  /* skip '"' */

    if (*ptr == '\0')
      {
      if (EMsg != NULL)
        sprintf(EMsg, "string is corrupt - early termination");

      return(FAILURE);
      }

    index = 0;

    while ((*ptr != '"') ||
           ((ptr > XMLString) && (*(ptr - 1) == '\\')))
      {
      tmpVLine[index++] = *(ptr++);

      if ((index >= MMAX_XBUFFER) || (*ptr == '\0'))
        {
        MXMLDestroyE(EP);

        /* locate tail */

        if (Tail != NULL)
          *Tail = ptr + strlen(ptr);

        if (EMsg != NULL)
          {
          sprintf(EMsg, "attribute name is too long - %.10s",
            tmpVLine);
          }

        return(FAILURE);
        }
      }

    tmpVLine[index] = '\0';

    MXMLSetAttr(E, tmpNLine, (void *)tmpVLine, mdfString);

    ptr++; /* ignore '"' */

    while (*ptr == ' ')
      ptr++;

    if ((ptr[0] == '/') && (ptr[1] == '>'))
      {
      /* element terminator reached */

      ptr += 2; /* skip '/>' */

      if (Tail != NULL)
        *Tail = ptr;

      return(SUCCESS);
      }
    }  /* END while (*ptr != '>') */

  ptr++; /* ignore '>' */

  /* skip whitespace */

  while (isspace(*ptr))
    ptr++;

  /* extract value */

  if (*ptr != '<')
    {
    char *ptr2;

    index = 0;

    while (*ptr != '<')
      {
      tmpVLine[index++] = *(ptr++);

      if (index >= MMAX_XBUFFER)
        break;
      }

    tmpVLine[index] = '\0';

    E->Val = strdup(tmpVLine);

    if (E->Val == NULL)
      {
      if (EMsg != NULL)
        {
        sprintf(EMsg,"cannot alloc memory for value - %.10s",
          tmpVLine);
        }

      return(FAILURE);
      }

    /* restore '<' symbols */

    for (ptr2 = strchr(E->Val, (char)14);ptr2 != NULL;ptr2 = strchr(ptr2, (char)14))
      *ptr2 = '<';
    }  /* END if (*ptr != '<') */

  /* extract children */

  while (ptr[1] != '/')
    {
    mxml_t *C;

    C = NULL;

    if (DoAppend == TRUE)
      {
      char *ptr2;
      char  tmpCName[MMAX_NAME];

      int   index;

      /* FORMAT:  <NAME>... */

      /* locate name */

      ptr2 = ptr + 1;  /* ignore '<' */

      index = 0;

      while ((*ptr2 != ' ') && (*ptr2 != '>'))
        {
        if ((ptr2[0] == '/') && (ptr2[1] == '>'))
          {
          break;
          }

        tmpCName[index++] = *(ptr2++);

        if ((index >= MMAX_LINE) || (ptr2[0] == '\0'))
          {
          if (EMsg != NULL)
            {
            sprintf(EMsg, "element name is too long - %.10s",
              tmpCName);
            }

          return(FAILURE);
          }
        }

      tmpCName[index] = '\0';

      MXMLGetChild(E, tmpCName, NULL, &C);
      }

    if ((MXMLFromString(&C, ptr, &tail, EMsg) == FAILURE) ||
        (MXMLAddE(E, C) == FAILURE))
      {
      break;
      }

    ptr = tail;

    if ((ptr == NULL) || (ptr[0] == '\0'))
      {
      /* XML is corrupt */

      if (Tail != NULL)
        *Tail = ptr;

      if ((EMsg != NULL) && (EMsg[0] == '\0'))
        strcpy(EMsg, "cannot extract child");

      return(FAILURE);
      }
    }  /* END while (ptr[1] != '/') */

  /* ignore whitespace */

  while (isspace(*ptr))
    ptr++;

  /* process tail */

  if (*ptr == '/')
    {
    /* process '/>' */

    ptr++; /* ignore '/' */
    }
  else
    {
    ptr++; /* ignore '<' */

    ptr++; /* ignore '/' */

    ptr += strlen(E->Name);
    }

  ptr++; /* ignore '>' */

  if (Tail != NULL)
    *Tail = ptr;

  return(SUCCESS);
  }  /* END MXMLFromString() */

/* END MXML.c */


