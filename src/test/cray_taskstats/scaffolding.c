#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "attribute.h"
#include "resource.h"
#include "pbs_job.h"

resource_def *svr_resc_def;
int           svr_resc_size = 0;

int LOGLEVEL = 7; /* force logging code to be exercised as tests run */

void log_err(int errnum, const char *routine, const char *text)
  { 
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  { 
  }

void init_attr(pbs_attribute *pattr)
  {
  memset(pattr, 0, sizeof(pbs_attribute));
  }

resource_def *find_resc_def(

  resource_def *rscdf, /* address of array of resource_def structs */
  const char   *name, /* name of resource */
  int           limit) /* number of members in resource_def array */

  {
  static resource_def mem_def;
  static resource_def cput_def;

  if (!strcmp(name, "mem"))
    {
    mem_def.rs_name = name;
    mem_def.rs_free = init_attr;
    return(&mem_def);
    }
  else
    {
    cput_def.rs_name = name;
    cput_def.rs_free = init_attr;
    return(&cput_def);
    }
  }


resource *add_resource_entry(

  pbs_attribute    *pattr,
  resource_def     *prdef)

  {
  resource  new_resource;

  if (pattr->at_val.at_ptr == NULL)
    pattr->at_val.at_ptr = new std::vector<resource>();

  std::vector<resource> *resources = (std::vector<resource> *)pattr->at_val.at_ptr;

  for (size_t i = 0; i < resources->size(); i++)
    {
    resource &r = resources->at(i);

    if (!strcmp(r.rs_defin->rs_name, prdef->rs_name))
      {
      return(&r);
      }
    }

  new_resource.rs_defin = prdef;
  new_resource.rs_value.at_type = prdef->rs_type;
  new_resource.rs_value.at_flags = 0;
  prdef->rs_free(&new_resource.rs_value);

  resources->push_back(new_resource);
  pattr->at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;
  
  // Return the element we just added
  resource &r = resources->at(resources->size() - 1);

  return(&r);
  } /* END add_resource_entry() */

pbs_attribute *job::get_attr(int index)
  {
  return(this->ji_wattr + index);
  }

job::job()
  {
  memset(this->ji_wattr, 0, sizeof(this->ji_wattr));
  }
job::~job() {}

const char *job::get_jobid() const
  {
  return(this->ji_qs.ji_jobid);
  }

void job::set_jobid(const char *jobid)
  {
  snprintf(this->ji_qs.ji_jobid, sizeof(this->ji_qs.ji_jobid), "%s", jobid);
  }

mom_job::mom_job() {}
mom_job::~mom_job() {}
