#include <pbs_config.h>
#include "pbs_job.h"
#include "log.h"


/**
 * Default constructor
 */

job::job() : ji_momstat(0), ji_modified(0), ji_momhandle(-1), ji_radix(0),
             ji_has_delete_nanny(false), ji_qhdr(NULL), ji_lastdest(0),
             ji_retryok(0), ji_rejectdest(), ji_is_array_template(false),
             ji_have_nodes_request(false), ji_external_clone(NULL),
             ji_cray_clone(NULL), ji_parent_job(NULL), ji_internal_id(-1),
             ji_being_recycled(false), ji_last_reported_time(0), ji_mod_time(0),
             ji_queue_counted(0), ji_being_deleted(false), ji_commit_done(false)

  {
  memset(this->ji_arraystructid, 0, sizeof(ji_arraystructid));
  memset(&this->ji_qs, 0, sizeof(this->ji_qs));

  this->ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(this->ji_mutex,NULL);
  lock_ji_mutex(this, __func__, NULL, LOGLEVEL);

  this->ji_qs.qs_version = PBS_QS_VERSION;

  /* set the working attributes to "unspecified" */
  job_init_wattr();
  } // END constructor()



/*
 * job_init_wattr - initialize job working pbs_attribute array
 * set the types and the "unspecified value" flag
 */

void job::job_init_wattr()

  {
  for (int i = 0; i < JOB_ATR_LAST; i++)
    {
    clear_attr(this->ji_wattr + i, &job_attr_def[i]);
    }
  }   /* END job_init_wattr() */



/*
 * A helper function for the destructor
 */

void job::free_job_allocation()

  {
  if (this->ji_cray_clone != NULL)
    {
    lock_ji_mutex(this->ji_cray_clone, __func__, NULL, LOGLEVEL);
    delete this->ji_cray_clone;
    }

  if (this->ji_external_clone != NULL)
    {
    lock_ji_mutex(this->ji_external_clone, __func__, NULL, LOGLEVEL);
    delete this->ji_external_clone;
    }

  /* remove any calloc working pbs_attribute space */
  for (int i = 0;i < JOB_ATR_LAST;i++)
    job_attr_def[i].at_free(&this->ji_wattr[i]);
  } // END free_job_allocation()



/*
 * The destructor
 */

job::~job()
  {
  free_job_allocation();
  pthread_mutex_destroy(this->ji_mutex);
  free(this->ji_mutex);
  } // END destructor()


