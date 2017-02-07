#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_job.h"

task::~task() {}
attribute_def job_attr_def[100];

void nodes_free(mom_job *pj) {}
