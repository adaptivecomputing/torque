#include "utils.h"




int is_whitespace(

  char c)

  {
  if ((c == ' ')  ||
      (c == '\n') ||
      (c == '\t') ||
      (c == '\r') ||
      (c == '\f'))
    return(TRUE);
  else
    return(FALSE);
  } /* END is_whitespace */





/**
 * similar to MUSNPrintF in moab
*/
int MUSNPrintF(

  char **BPtr,   /* I */
  int   *BSpace, /* I */
  char  *Format, /* I */
  ...)           /* I */

  {
  int len;

  va_list Args;

  if ((BPtr == NULL) ||
      (BSpace == NULL) ||
      (Format == NULL) ||
      (*BSpace <= 0))
    {
    return(FAILURE);
    }

  va_start(Args, Format);

  len = vsnprintf(*BPtr, *BSpace, Format, Args);

  va_end(Args);

  if (len <= 0)
    {
    return(FAILURE);
    }

  *BPtr += len;

  *BSpace -= len;

  return(SUCCESS);
  }  /* END MUSNPrintF() */




char *MUStrTok(

  char  *Line,  /* I (optional) */
  char  *DList, /* I */
  char **Ptr)   /* O point to termination character of returned token */

  {
  static char *Head = NULL;

  int dindex;

  mbool_t ignchar;

  /* NOTE:  no handling if Line is NULL and Ptr is uninitialized */

  if (Ptr == NULL)
    {
    /* tokPtr not specified */

    return(NULL);
    }

  if (Line != NULL)
    {
    *Ptr = Line;

    if (Line[0] == '\0')
      {
      /* line is empty */

      if (Ptr != NULL)
        *Ptr = NULL;

      return(NULL);
      }
    }
  else if (*Ptr == NULL)
    {
    return(NULL);
    }

  ignchar = FALSE;

  Head = NULL;

  while (**Ptr != '\0')
    {
    for (dindex = 0;DList[dindex] != '\0';dindex++)
      {
      if (**Ptr == DList[dindex])
        {
        **Ptr = '\0';

        (*Ptr)++;

        if (Head != NULL)
          {
          return(Head);
          }
        else
          {
          ignchar = TRUE;

          break;
          }
        }
      }    /* END for (dindex) */

    if ((ignchar != TRUE) && (**Ptr != '\0'))
      {
      if (Head == NULL)
        Head = *Ptr;

      (*Ptr)++;
      }

    ignchar = FALSE;
    }  /* END while (**Ptr != '\0') */

  return(Head);
  }  /* END MUStrTok() */




/**
 * similar to MUStrNCat in moab
*/

int MUStrNCat(

  char **BPtr,   /* I (modified) */
  int   *BSpace, /* I (modified) */
  char  *Src)    /* I */

  {
  int index;

  if ((BPtr == NULL) || (BSpace == NULL) || (*BSpace <= 0))
    {
    return(FAILURE);
    }

  if ((Src == NULL) || (Src[0] == '\0'))
    {
    return(SUCCESS);
    }

  for (index = 0;index < *BSpace - 1;index++)
    {
    if (Src[index] == '\0')
      break;

    (*BPtr)[index] = Src[index];
    }  /* END for (index) */

  (*BPtr)[index] = '\0';

  *BPtr   += index;

  *BSpace -= index;

  return(SUCCESS);
  }  /* END MUStrNCat() */



/**
 * similar to MUSleep in moab
*/
int MUSleep(

  long SleepDuration) /* I (in us) */

  {

  struct timeval timeout;

  timeout.tv_sec  = SleepDuration / 1000000;
  timeout.tv_usec = SleepDuration % 1000000;

  select(0, (fd_set *)NULL, (fd_set *)NULL, (fd_set *)NULL, &timeout);

  return(0);
  }  /* END MUSleep() */




/**
 * similar to MUReadPipe in moab
*/
int MUReadPipe(

  char *Command,  /* I */
  char *Buffer,   /* O */
  int   BufSize)  /* I */

  {
  FILE *fp;
  int   rc;

  int   rcount;
  int   ccount;

  if ((Command == NULL) || (Buffer == NULL))
    {
    return(1);
    }

  if ((fp = popen(Command, "r")) == NULL)
    {
    return(1);
    }

  ccount = 0;

  rcount = 0;

  do
    {
    rc = fread(Buffer + ccount, 1, BufSize - ccount, fp);

    /* NOTE:  ferror may create false failures */

    if (rc > 0)
      {
      ccount += rc;
      }

    if ((ccount >= BufSize) || (rcount++ > 10))
      {
      /* full buffer loaded or too many attempts */

      break;
      }
    }
  while (!feof(fp));

  if (ferror(fp))
    {
    /* FAILURE */

    pclose(fp);

    return(1);
    }

  /* SUCCESS - terminate buffer */

  Buffer[MIN(BufSize - 1,ccount)] = '\0';

  pclose(fp);

  return(0);
  }  /* END MUReadPipe() */





