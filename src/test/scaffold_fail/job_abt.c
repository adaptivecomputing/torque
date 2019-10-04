/*
 *
 *  Created on: Jan 24, 2013
 *      Author: bdaw
 */

#include "scaffold_fail.h"

 int job_abt(struct job **pjobp, const char *text, boost::shared_ptr<mutex_mgr>& job_mutex, bool b=false)
  {
  fprintf(stderr, "The call to job_abt needs to be mocked!!\n");
  exit(1);
  }

