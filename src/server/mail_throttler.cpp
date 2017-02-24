#include "mail_throttler.hpp"
#include "pbs_error.h"


// Default contstructor
mail_info::mail_info() : mailto(), exec_host(), jobid(), jobname(),
                         text(), errFile(), outFile(), mail_point(0)
  {
  }

// Constructor from job - update the default values with 
mail_info::mail_info(

  job *pjob) : mailto(), exec_host(), jobid(), jobname(), text(), errFile(), outFile(), mail_point(0),
               queue_name(), owner(), working_directory(), resources_requested(), resources_used()

  {
  if (pjob != NULL)
    {
    if (pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str != NULL)
      this->exec_host = pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str;

    this->jobid = pjob->ji_qs.ji_jobid;

    if (pjob->ji_wattr[JOB_ATR_jobname].at_val.at_str != NULL)
      this->jobname = pjob->ji_wattr[JOB_ATR_jobname].at_val.at_str;

    // Initialize the output files
    if (pjob->ji_wattr[JOB_ATR_join].at_flags & ATR_VFLAG_SET)
      {
      char *join_val = pjob->ji_wattr[JOB_ATR_join].at_val.at_str;
      if (!strcmp(join_val, "oe"))
        {
        this->errFile = pjob->ji_wattr[JOB_ATR_outpath].at_val.at_str;
        this->outFile = pjob->ji_wattr[JOB_ATR_outpath].at_val.at_str;
        }
      else if (!strcmp(join_val, "eo"))
        {
        this->errFile = pjob->ji_wattr[JOB_ATR_errpath].at_val.at_str;
        this->outFile = pjob->ji_wattr[JOB_ATR_errpath].at_val.at_str;
        }
      }

    if (this->outFile.size() == 0)
      this->outFile = pjob->ji_wattr[JOB_ATR_outpath].at_val.at_str;

    if (this->errFile.size() == 0)
      this->errFile = pjob->ji_wattr[JOB_ATR_errpath].at_val.at_str;

    this->queue_name = pjob->ji_qs.ji_queue;
    this->owner = pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str;
    this->working_directory = pjob->ji_wattr[JOB_ATR_init_work_dir].at_val.at_str;

    if (pjob->ji_wattr[JOB_ATR_resource].at_val.at_ptr != NULL)
      this->resources_requested = *((std::vector<resource> *)pjob->ji_wattr[JOB_ATR_resource].at_val.at_ptr);

    if (pjob->ji_wattr[JOB_ATR_resc_used].at_val.at_ptr != NULL)
      this->resources_used = *((std::vector<resource> *)pjob->ji_wattr[JOB_ATR_resc_used].at_val.at_ptr);
    }
  }

// Copy constructor
mail_info::mail_info(

  const mail_info &other) : mailto(other.mailto), exec_host(other.exec_host), jobid(other.jobid),
                            jobname(other.jobname), text(other.text), errFile(other.errFile),
                            outFile(other.outFile), mail_point(other.mail_point),
                            queue_name(other.queue_name), owner(other.owner),
                            working_directory(other.working_directory), 
                            resources_requested(other.resources_requested),
                            resources_used(other.resources_used)
  {
  }

// = assignment operator
mail_info &mail_info::operator =(
    
  const mail_info &other)

  {
  this->mailto = other.mailto;
  this->exec_host = other.exec_host;
  this->jobid = other.jobid;
  this->jobname = other.jobname;
  this->text = other.text;
  this->errFile = other.errFile;
  this->outFile = other.outFile;
  this->mail_point = other.mail_point;
  this->queue_name = other.queue_name;
  this->owner = other.owner;
  this->working_directory = other.working_directory;
  this->resources_requested = other.resources_requested;
  this->resources_used = other.resources_used;
  return(*this);
  }


/*
 * Default constructor
 */

mail_throttler::mail_throttler() : pending()
  {
  }



/*
 * get_email_list()
 *
 * Gets and removes any pending emails to the specified addressee
 * @param addressee - the addressee whose emails we seek
 * @param list - the list to be populated with said emails
 * @return PBSE_NONE if address has pending emails, -1 otherwise
 */

int mail_throttler::get_email_list(
    
  const std::string      &addressee,
  std::vector<mail_info> &list)

  {
  int                                                      rc = PBSE_NONE;
  std::map<std::string, std::vector<mail_info> >::iterator it;

  // set lock
  pthread_mutex_lock(&this->mt_mutex);

  it = this->pending.find(addressee);

  if (it != this->pending.end())
    {
    list = this->pending[addressee];
    this->pending.erase(it);
    }
  else
    {
    // Not found, nothing to do
    rc = -1;
    list.clear();
    }

  // unlock
  pthread_mutex_unlock(&this->mt_mutex);

  return(rc);
  } // END get_email_list()



/*
 * add_email_entry()
 *
 * Adds mi as an email pending to be sent
 * @param mi - the information for the pending email
 * @return true if this is a new recipient or false if this recipient already exists
 */

bool mail_throttler::add_email_entry(
    
  mail_info &mi)

  {
  bool                                                     is_first = false;
  std::map<std::string, std::vector<mail_info> >::iterator it;

  // set lock
  pthread_mutex_lock(&this->mt_mutex);

  it = this->pending.find(mi.mailto);

  if (it == this->pending.end())
    {
    // Create a new pending list
    std::vector<mail_info> v;
    v.push_back(mi);
    this->pending[mi.mailto] = v;
    is_first = true;
    }
  else
    {
    // Append this to the pending list
    this->pending[mi.mailto].push_back(mi);
    is_first = this->pending[mi.mailto].size() == 1;
    }

  // unlock
  pthread_mutex_unlock(&this->mt_mutex);

  return(is_first);
  } // END add_email_entry()

