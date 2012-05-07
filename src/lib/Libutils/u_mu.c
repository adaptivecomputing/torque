

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include "utils.h"
#include "../Liblog/pbs_log.h"



unsigned int get_random_number()

  {
  struct timeval t_time;
  gettimeofday(&t_time, 0);
  return(unsigned int)t_time.tv_usec;
  } /* END get_random_number() */




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





char *trim(

  char *str) /* M */

  {
  char *front_ptr;
  char *end_ptr;

  if (str == NULL)
    return(NULL);

  front_ptr = str;
  end_ptr = str + strlen(str) - 1;

  while ((is_whitespace(*front_ptr) == TRUE) &&
         (front_ptr <= end_ptr))
    {
    front_ptr++;
    }

  while ((is_whitespace(*end_ptr) == TRUE) &&
         (end_ptr > front_ptr))
    {
    *end_ptr = '\0';
    end_ptr--;
    }

  return(front_ptr);
  } /* END trim() */





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



/*
 * a threadsafe tokenizing function - this also alters the input string
 * just like strtok does.
 *
 * @param str - a modified string pointer (advanced past the token)
 * @param delims - the delimiters to look for
 * @return - a pointer to the token
 */

char *threadsafe_tokenizer(

  char **str,    /* M */
  char  *delims) /* I */

  {
  char *current_char;
  char *start;

  if ((str == NULL) ||
      (*str == NULL))
    return(NULL);

  /* save start position */
  start = *str;

  /* return NULL at the end of the string */
  if (*start == '\0')
    return(NULL);

  /* begin at the start */
  current_char = start;

  /* advance to the end of the string or until you find a delimiter */
  while ((*current_char != '\0') &&
         (!strchr(delims, *current_char)))
    current_char++;

  /* advance str */
  if (*current_char != '\0')
    {
    /* not at the end of the string */
    *str = current_char + 1;
    *current_char = '\0';
    }
  else
    {
    /* at the end of the string */
    *str = current_char;
    }

  return(start);
  } /* END threadsafe_tokenizer() */



/*
 * strcats if there is enough space, otherwise returns -1
 */

int safe_strncat(

  char   *str,
  char   *to_append,
  size_t  space_remaining)

  {
  size_t len = strlen(to_append);

  /* not enough space */
  if (space_remaining < len)
    return(-1);
  else
    strcat(str, to_append);

  return(PBSE_NONE);
  } /* END safe_strncat() */





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




int write_buffer(

  char *buf,
  int   len,
  int   fds)

  {
  char *id = "write_buffer";
  int   written;

  while ((written = write(fds,buf,len)) != len)
    {
    if ((errno == EINTR) &&
        (written == -1))
      continue;
    else if (written > 0)
      {
      len -= written;
      buf += written;
      }
    else
      {
      log_err(errno,id,"Unable to write to file or socket");
      
      return(-1);
      }
    }

  return(PBSE_NONE);
  } /* END write_buffer */




