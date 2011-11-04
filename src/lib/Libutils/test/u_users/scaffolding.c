#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "log.h" /* LOG_BUF_SIZE */

char log_buffer[LOG_BUF_SIZE];

void log_ext(int errnum, char *routine, char *text, int severity)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

