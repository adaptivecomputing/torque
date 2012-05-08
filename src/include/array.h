#ifndef ARRAY_H
#define ARRAY_H



/* these are required if you include array.h */
#include "pbs_ifl.h"
#include "log.h"
#include "list_link.h"
#include "attribute.h"
#include "server_limits.h"
#include "pbs_error.h"
#include "batch_request.h"
#include "pbs_job.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define NO_SLOT_LIMIT      -1
#define ARRAY_TOO_LARGE    -5
#define INVALID_SLOT_LIMIT -6
#define INITIAL_NUM_ARRAYS  50
#define NO_JOBS_IN_ARRAY   -21

#define ARRAY_FILE_SUFFIX ".AR"

enum ArrayEventsEnum {
  aeQueue = 0,
  aeRun,
  aeTerminate
}; /* END ArrayEventsEnum */

typedef struct
  {
  list_link request_tokens_link;

  int start;
  int end;
  } array_request_node;


#define ARRAY_QS_STRUCT_VERSION 4

/* This structure is here to make it possible to move job arrays created in 
   TORQUE version 2.5.9 and 3.0.3 to later versions of TORQUE. In verison
   2.5.9 and 3.0.3 the element num_purged was added to the structure. However,
   this was done without consideration to backward compatibility. */
struct job_array_259
  {
  tlist_head request_tokens; /* head of linked list of request tokens, used 
                                during cloning (cloning is the process of 
                                copying the "template" job to generate all of the 
                                jobs in the array)*/

  job **jobs; /* a pointer to the job pointers in this array */

  int jobs_recovered; /* on server restart we track the number of array tasks
                         that have been recovered. this is incase the server
                         is restarted (cleanly) before the array is 
                         completely setup */

  job *template_job; /* pointer to the template job */

  /* this info is saved in the array file */
  struct array_info_259
    {
    /* NOTE, struct_version _must_ come first in the struct */
    int  struct_version; /* version of this struct */
    int  array_size;     /* size of the array used to track the jobs */
    int  num_jobs;       /* total number of jobs in the array */
    int  slot_limit;     /* number of jobs in the array that can be run at one time */
    int  jobs_running;   /* number of jobs in the array currently running */
    int  jobs_done;      /* number of jobs that have been deleted, etc. */
    int  num_cloned;     /* number of jobs out of the array that have been created */
    int  num_started;    /* number of jobs that have begun execution */
    int  num_failed;     /* number of jobs that exited with status != 0 */
    int  num_successful; /* number of jobs that exited with status == 0 */
    int  num_purged;     /* number of jobs which have called purge_job */

    /* dependency info */
    tlist_head deps;
    
    /* max user name, server name, 1 for the @, and one for the NULL */
    char owner[PBS_MAXUSER + PBS_MAXSERVERNAME + 2];
    char parent_id[PBS_MAXSVRJOBID + 1];
    char fileprefix[PBS_JOBBASE + 1];
    char submit_host[PBS_MAXSERVERNAME +1];
    } ai_qs;
  };

typedef struct job_array_259 job_array_259;


/* pbs_server will keep a list of these structs, with one struct per job array*/

struct job_array
  {
  tlist_head request_tokens; /* head of linked list of request tokens, used 
                                during cloning (cloning is the process of 
                                copying the "template" job to generate all of the 
                                jobs in the array)*/

  char **job_ids; /* a pointer to the job pointers in this array */

  int    jobs_recovered; /* on server restart we track the number of array tasks
                         that have been recovered. this is incase the server
                         is restarted (cleanly) before the array is 
                         completely setup */

  pthread_mutex_t *ai_mutex;

  /* this info is saved in the array file */
  struct array_info
    {
    /* NOTE, struct_version _must_ come first in the struct */
    int  struct_version; /* version of this struct */
    int  array_size;     /* size of the array used to track the jobs */
    int  num_jobs;       /* total number of jobs in the array */
    int  slot_limit;     /* number of jobs in the array that can be run at one time */
    int  jobs_running;   /* number of jobs in the array currently running */
    int  jobs_done;      /* number of jobs that have been deleted, etc. */
    int  num_cloned;     /* number of jobs out of the array that have been created */
    int  num_started;    /* number of jobs that have begun execution */
    int  num_failed;     /* number of jobs that exited with status != 0 */
    int  num_successful; /* number of jobs that exited with status == 0 */

    /* dependency info */
    tlist_head deps;
    
    /* max user name, server name, 1 for the @, and one for the NULL */
    char owner[PBS_MAXUSER + PBS_MAXSERVERNAME + 2];
    char parent_id[PBS_MAXSVRJOBID + 1];
    char fileprefix[PBS_JOBBASE + 1];
    char submit_host[PBS_MAXSERVERNAME +1];
    int  num_purged;     /* number of jobs which have called purge_job */
    } ai_qs;
  };

typedef struct job_array job_array;

struct all_arrays
  {
  resizable_array *ra;

  pthread_mutex_t *allarrays_mutex;
  };

int  is_array(char *id);
int  array_delete(job_array *pa);
int  array_save(job_array *pa);
int  array_save(job_array *pa);
void array_get_parent_id(char *job_id, char *parent_id);

job *find_array_template(char *arrayid);

job_array *get_array(char *id);
int array_recov(char *path, job_array **pa);

int delete_array_range(job_array *pa, char *range);
int delete_whole_array(job_array *pa);
int attempt_delete(void *);

int hold_array_range(job_array *,char *,pbs_attribute *);
void hold_job(pbs_attribute *,void *);

int modify_array_range(job_array *,char *,svrattrl *,struct batch_request *,int);
int modify_job(void **,svrattrl *,struct batch_request *,int, int);

void update_array_values(job_array *,int,enum ArrayEventsEnum, char *job_id, long job_atr_hold, int job_exit_status);

int register_array_depend(job_array*,struct batch_request *,int,int);
void set_array_depend_holds(job_array *);

int release_job(struct batch_request *,void *);
int release_array_range(job_array *,struct batch_request *,char *);

int first_job_index(job_array *);

void update_array_statuses(job_array *);

int num_array_jobs(char *);

int        insert_array(job_array *);
int        remove_array(job_array *);
job_array *next_array(int *);
void       initialize_all_arrays_array();

job_array *get_jobs_array(job **);

#endif
