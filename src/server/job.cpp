#include <pbs_config.h>
#include "pbs_job.h"
#include "log.h"
#include "json/json.h"


/**
 * Default constructor
 */

job::job() : ji_plugin_usage_info(), ji_momstat(0), ji_modified(0), ji_momhandle(-1), ji_radix(0),
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
 * job_init_wattr() 
 *
 * Initialize job attribute array
 * Sets the types and the "unspecified value" flag
 */

void job::job_init_wattr()

  {
  for (int i = 0; i < JOB_ATR_LAST; i++)
    {
    clear_attr(this->ji_wattr + i, &job_attr_def[i]);
    }
  }   /* END job_init_wattr() */



/*
 * free_job_allocation()
 *
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



/*
 * add_plugin_resource_usage()
 *
 * Adds resource usage information to the string for the accounting log from the resource plugin,
 * if any
 *
 * @param acct_data - 
 */

void job::add_plugin_resource_usage(

  std::string &acct_data) const

  {
  for (std::map<std::string, std::string>::const_iterator it = this->ji_plugin_usage_info.begin();
       it != this->ji_plugin_usage_info.end();
       it++)
    acct_data += " resources_used." + it->first + "=" + it->second;
  } // END add_plugin_resource_usage()



/*
 * encode_plugin_resource_usage
 *
 * Adds resource usage information from the plugin to the list for qstat output
 * @param phead - The linked list we add the plugin usage to
 */

void job::encode_plugin_resource_usage(
    
  tlist_head *phead) const

  {
  for (std::map<std::string, std::string>::const_iterator it = this->ji_plugin_usage_info.begin();
       it != this->ji_plugin_usage_info.end();
       it++)
    {
    svrattrl *pal = attrlist_create(ATTR_used, it->first.c_str(), it->second.size() + 1);
    strcpy(pal->al_value, it->second.c_str());
    pal->al_flags = ATR_VFLAG_SET;
    append_link(phead, &pal->al_link, pal);
    }

  } // END encode_plugin_resource_usage()



/*
 * set_plugin_resource_usage()
 *
 * Records a resource from the resource plugin in our resources used map
 */

void job::set_plugin_resource_usage(

  const char *name,
  const char *value)

  {
  this->ji_plugin_usage_info[name] = value;
  }
  


/*
 * set_plugin_resource_usage_from_json()
 *
 * Records the plugin reported resource usage from the json string
 */

void job::set_plugin_resource_usage_from_json(
    
  Json::Value &resources)

  {
  std::vector<std::string>keys = resources.getMemberNames();

  for (size_t i = 0; i < keys.size(); i++)
    this->ji_plugin_usage_info[keys[i]] = resources[keys[i]].asString();
  }



void job::set_plugin_resource_usage_from_json(

  const char *json_str)

  {
  Json::Value  resources;
  Json::Reader reader;

  if (reader.parse(json_str, resources) == true)
    this->set_plugin_resource_usage_from_json(resources);
  }
