/*
 *
 *  Created on: Jan 24, 2013
 *      Author: bdaw
 */

#include "scaffold_fail.h"

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue needs to be mocked!!\n");
  exit(1);
  }

