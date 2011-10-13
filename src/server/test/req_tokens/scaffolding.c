#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute, batch_op */

int set_arst(struct attribute *attr, struct attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_arst to be mocked!!\n");
  exit(1);
  }

