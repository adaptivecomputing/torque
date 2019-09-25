/*
 *
 *  Created on: Jan 24, 2013
 *      Author: bdaw
 */

#include "scaffold_fail.h"

void account_jobend( job *pjob, char *used, boost::shared_ptr<mutex_mgr>& job_mutex)
  {
  fprintf(stderr, "The call to account_jobend to be mocked!!\n");
  exit(1);
  }

