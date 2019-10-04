/*
 *
 *  Created on: Jan 24, 2013
 *      Author: bdaw
 */

#include "scaffold_fail.h"

int svr_movejob(job *jobp, char *destination, int *i, struct batch_request *req, boost::shared_ptr<mutex_mgr>& job_mutex)
  {
  fprintf(stderr, "The call to svr_movejob needs to be mocked!!\n");
  exit(1);
  }
