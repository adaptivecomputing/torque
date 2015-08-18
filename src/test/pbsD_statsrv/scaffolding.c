#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attrl */

int pbs_errno = 0;

struct batch_status *PBSD_status(int c, int function, int *local_errno, char *id, struct attrl *attrib, char *extend)
 {
 fprintf(stderr, "The call to PBSD_manager needs to be mocked!!\n");
 exit(1);
 }
