#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attropl */

int pbs_errno = 0;

int PBSD_jscript(int c, const char *script_file, const char *jobid)
  {
  fprintf(stderr, "The call to PBSD_jscript needs to be mocked!!\n");
  exit(1);
  }

int PBSD_commit(int connect, char *jobid)
  {
  fprintf(stderr, "The call to PBSD_commit needs to be mocked!!\n");
  exit(1);
  }

int PBSD_rdytocmt(int connect, char *jobid)
  {
  fprintf(stderr, "The call to PBSD_rdytocmt needs to be mocked!!\n");
  exit(1);
  }

char *PBSD_queuejob(int connect, int *local_errno, const char *jobid, const char *destin, struct attropl *attrib, char *extend)
  {
  fprintf(stderr, "The call to PBSD_queuejob needs to be mocked!!\n");
  exit(1);
  }

