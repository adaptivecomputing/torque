#ifndef ARRAY_H
#define ARRAY_H



#include "pbs_ifl.h"
#include "log.h"
#include "list_link.h"
#include "attribute.h"
#include "server_limits.h"
#include "job.h"
#include "log.h"
#include "pbs_error.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

int is_array(char *id);
job_array *get_array(char *id);
int array_save(job_array *pa);
void get_parent_id(char *job_id, char *parent_id);
job_array *recover_array_struct(char *path);
int delete_array_struct(job_array *pa);
int setup_array_struct(job *pjob);



#endif
