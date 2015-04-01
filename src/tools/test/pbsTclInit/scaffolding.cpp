#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include <tcl.h> /* Tcl_Interp */

int chk_file_sec_stderr;

void add_cmds(Tcl_Interp *interp)
  { 
  fprintf(stderr, "The call to add_cmds needs to be mocked!!\n");
  exit(1);
  }

void fullresp(int flag)
  { 
  fprintf(stderr, "The call to fullresp needs to be mocked!!\n");
  exit(1);
  }

