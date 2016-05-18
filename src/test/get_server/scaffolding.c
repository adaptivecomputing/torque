#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pbs_ifl.h"

extern "C"
{
int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg)
  {
  return(0);
  }

char *pbs_default()
  {
  return(strdup("napali"));
  }
}

