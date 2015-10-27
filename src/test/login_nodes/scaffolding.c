#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#include "pbs_nodes.h"
#include "job_usage_info.hpp"

int LOGLEVEL = 5;


int lock_node(struct pbsnode *pnode, const char *caller, const char *msg, int level) {return(0);}
int unlock_node(struct pbsnode *pnode, const char *caller, const char *msg, int level) {return(0);}

int hasprop(

  struct pbsnode *pnode,
  struct prop    *props)

  {
  struct  prop    *need;

  for (need = props;need;need = need->next)
    {

    struct prop *pp;

    if (need->mark == 0) /* not marked, skip */
      continue;

    for (pp = pnode->nd_first;pp != NULL;pp = pp->next)
      {
      if (strcmp(pp->name, need->name) == 0)
        break;  /* found it */
      }

    if (pp == NULL)
      {
      return(0);
      }
    }

  return(1);
  }  /* END hasprop() */


int number(

  char **ptr,
  int   *num)

  {
  char  holder[80];
  int   i = 0;
  char *str = *ptr;

  while (isdigit(*str) && (unsigned int)(i + 1) < sizeof holder)
    holder[i++] = *str++;

  if (i == 0)
    {
    return(1);
    }

  holder[i] = '\0';

  if ((i = atoi(holder)) <= 0)
    {
    return(-1);
    }

  *ptr = str;

  *num = i;

  return(0);
  }  /* END number() */



int property(

  char **ptr,
  char **prop)

  {
  char        *str = *ptr;
  char        *dest = *prop;
  int          i = 0;

  while (isalnum(*str) || *str == '-' || *str == '.' || *str == '=' || *str == '_')
    dest[i++] = *str++;

  dest[i] = '\0';

  /* skip over "/vp_number" */

  if (*str == '/')
    {
    do
      {
      str++;
      }
    while (isdigit(*str));
    }

  *ptr = str;

  return(0);
  }  /* END property() */

int proplist(char **str, struct prop **plist, int *node_req, int *gpu_req)
  {
  struct prop *pp;
  char         name_storage[80];
  char        *pname;
  char        *pequal;

  *node_req = 1; /* default to 1 processor per node */

  pname  = name_storage;
  *pname = '\0';

  for (;;)
    {
    if (property(str, &pname))
      {
      return(1);
      }

    if (*pname == '\0')
      break;

    if ((pequal = strchr(pname, (int)'=')) != NULL)
      {
      /* special property */

      /* identify the special property and place its value */
      /* into node_req       */

      *pequal = '\0';

      if (strcmp(pname, "ppn") == 0)
        {
        pequal++;

        if ((number(&pequal, node_req) != 0) || (*pequal != '\0'))
          {
          return(1);
          }
        }
      else if (strcmp(pname, "gpus") == 0)
        {
        pequal++;

        if ((number(&pequal, gpu_req) != 0) || (*pequal != '\0'))
          {
          return(1);
          }
        }
      else
        {
        return(1); /* not recognized - error */
        }
      }
    else
      {
      pp = (struct prop *)calloc(1, sizeof(struct prop));

      pp->mark = 1;
      pp->name = strdup(pname);
      pp->next = *plist;

      *plist = pp;
      }

    if (**str != ':')
      break;

    (*str)++;
    }  /* END for(;;) */

  return(PBSE_NONE);
  } /* END proplist() */


pbsnode::pbsnode(): nd_properties(), nd_mutex(), nd_state(INUSE_FREE), nd_needed(0),
                    nd_np_to_be_used(0),
                    nd_power_state(0)
  {}

pbsnode::~pbsnode() {}

int pbsnode::lock_node(const char *id, const char *msg, int level)
  {
  return(0);
  }

int pbsnode::unlock_node(const char *id, const char *msg, int level)
  {
  return(0);
  }

void pbsnode::change_name(const char *name)
  {
  this->nd_name = name;
  this->nd_properties.push_back(this->nd_name);
  }



bool pbsnode::hasprop(

  struct prop *props) const

  {
  struct  prop    *need;

  for (need = props;need;need = need->next)
    {
    if (need->mark == 0) /* not marked, skip */
      continue;
    
    bool found = false;

    for (unsigned int i = 0; i < this->nd_properties.size(); i++)
      {
      if (!strcmp(this->nd_properties[i].c_str(), need->name))
        {
        found = true;
        break;
        }
      }

    if (found == false)
      return(found);
    }

  return(true);
  }

void pbsnode::add_property(

  const std::string &prop)

  {
  this->nd_properties.push_back(prop);
  }

job_usage_info::job_usage_info(int internal_id)
  {
  this->internal_job_id = internal_job_id;
  }

bool job_usage_info::operator ==(

  const job_usage_info &jui)

  {
  if (this->internal_job_id == jui.internal_job_id)
    return(true);
  else
    return(false);
  }
   
job_usage_info &job_usage_info::operator= (
    
  const job_usage_info &other_jui)

  {
  if (this == &other_jui)
    return(*this);

  this->internal_job_id = other_jui.internal_job_id;
  this->est = other_jui.est;

  return(*this);
  }
