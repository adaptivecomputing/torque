#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

int chk_file_sec_stderr;

int chk_file_sec(const char *path, int isdir, int sticky, int disallow, int fullpath, char *SEMsg)
  { 
  fprintf(stderr, "The call to chk_file_sec needs to be mocked!!\n");
  exit(1);
  }



