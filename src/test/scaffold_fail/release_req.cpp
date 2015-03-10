/*
 *
 *  Created on: Jan 24, 2013
 *      Author: bdaw
 */

#include "scaffold_fail.h"

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

