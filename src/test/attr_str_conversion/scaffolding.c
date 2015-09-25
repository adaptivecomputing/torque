#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"
#include "resource.h"

int svr_resc_size = 0;
resource_def *svr_resc_def;

int get_parent_and_child(

  char  *start,       /* I */
  char **parent,      /* O */
  char **child,       /* O */
  char **end)         /* O */

  {
  return(PBSE_NONE);
  }

int unescape_xml(

  char *in,   /* I */ 
  char *out,  /* O */
  int   size) /* I */

  {
  return(PBSE_NONE);
  }

void log_err(

  int         errnum,  /* I (errno or PBSErrno) */
  const char *routine, /* I */
  const char *text)    /* I */

  {
  }

