#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "log.h"
#include "attribute.h"
#include "complete_req.hpp"
#include "resource.h"
#include "pbs_error.h"
#include "attr_req_info.hpp"


void log_event(

  int         eventtype,
  int         objclass,
  const char *objname,
  const char *text)

  {
  return;
  }

int csv_length(const char *csv_str)
  {
  return(0);
  }

char *csv_nth(const char *csv_str, int n)
  
  {
  return(NULL);
  }

char *csv_find_string(
    
  const char *csv_str,
  const char *search_str)

  {
  return(NULL);
  }

int ctnodes(

  const char *spec)
  {
  return(0);
  }

char *remove_from_csv(

  char* src,            /* I - line with csv values*/
  char* model_pattern   /* I - pattern with models*/)

  {
  return(NULL);
  }

int comp_nodestr(pbs_attribute *attr, pbs_attribute *with)
  {
  return(0);
  }

int set_resc(

  pbs_attribute *old,
  pbs_attribute *new_attr,
  enum batch_op  op)
  {
  return(0);
  }

int comp_resc(

  pbs_attribute *attr,  /* I queue's min/max attributes */
  pbs_attribute *with)  /* I job's current requirements/attributes */
  {
  return(0);
  }

int decode_depend(

  pbs_attribute *patr,
  const char    *name,  /* attribute name */
  const char    *rescn, /* resource name, unused here */
  const char    *val,   /* attribute value */
  int            perm)  /* only used for resources */
  {
  return(0);
  }

int action_resc(

  pbs_attribute *pattr,
  void          *pobject,
  int            actmode)
  {
  return(0);
  }

int encode_resc(

  pbs_attribute  *attr,    /* ptr to pbs_attribute to encode */
  tlist_head     *phead,   /* head of attrlist list */
  const char    *atname,  /* pbs_attribute name */
  const char    *rsname,  /* resource name, null on call */
  int             mode,    /* encode mode */
  int             ac_perm) /* access permissions */
  {
  return(0);
  }

int set_uacl(
    
  pbs_attribute *attr,
  pbs_attribute *uacl_new,
  enum batch_op  op)
  {
  return(0);
  }

int depend_on_que(

  pbs_attribute *pattr,
  void          *pj,
  int            mode)
  {
  return(0);
  }

int decode_resc(

  pbs_attribute *patr,  /* Modified on Return */
  const char    *name,  /* pbs_attribute name */
  const char    *rescn, /* I resource name - is used here */
  const char    *val,   /* resource value */
  int            perm)  /* access permissions */
  {
  return(0);
  }

int set_complete_req(
    
  pbs_attribute *attr,
  pbs_attribute *new_attr,
  enum batch_op  op)
  {
  return(0);
  }

void free_resc(pbs_attribute *pattr) {}

int encode_depend(

  pbs_attribute *attr,   /* ptr to attribute to encode */
  tlist_head    *phead,  /* ptr to head of attrlist list */
  const char   *atname, /* attribute name */
  const char   *rsname, /* resource name or null */
  int            mode,   /* encode mode, unused here */
  int            perm)   /* only used for resources */
  {
  return(0);
  }

void free_complete_req(pbs_attribute *patr) {}

void free_depend(pbs_attribute *attr) {}

int comp_checkpoint(

  pbs_attribute *attr,
  pbs_attribute *with)
  {
  return(0);
  }

int set_depend(

  pbs_attribute *attr,
  pbs_attribute *new_attr,
  enum batch_op  op)
  {
  return(0);
  }

int comp_complete_req(
   
  pbs_attribute *attr,
  pbs_attribute *with)
  {
  return(0);
  }

int  decode_complete_req(
    
  pbs_attribute *patr,
  const char    *name,
  const char    *rescn,
  const char    *val,
  int            perm)
  {
  return(0);
  }

int encode_complete_req(
    
  pbs_attribute *attr,
  tlist_head    *phead,
  const char    *atname,
  const char    *rsname,
  int            mode,
  int            perm)
  {
  return(0);
  }

int comp_depend(
    
  pbs_attribute *attr, 
  pbs_attribute *with)
  {
  return(0);
  }

int job_set_wait(

  pbs_attribute *pattr,
  void          *j,    /* a (job *) cast to void * */
  int            mode) /* unused, do it for all action modes */
  {
  return(0);
  }

int str_to_attr(

  const char           *name,   /* I */
  const char           *val,    /* I */
  pbs_attribute        *attr,   /* O */
  struct attribute_def *padef,  /* I */
  int                   limit,  /* I */
  int                   index)

  {
  if ((name == NULL) ||
      (val  == NULL) ||
      (attr == NULL))
    {
    return(-10);
    }

  if (index < 0)
    index = find_attr(padef, name, limit);

  if (index < 0)
    return(ATTR_NOT_FOUND);

  switch (padef[index].at_type)
    {
    case ATR_TYPE_LONG:

      attr[index].at_val.at_long = strtol(val, NULL, 10);

      break;

    case ATR_TYPE_CHAR:

      attr[index].at_val.at_char = *val;

      break;

    case ATR_TYPE_STR:

      // Free the old value, if present
      if (attr[index].at_val.at_str != NULL)
        free(attr[index].at_val.at_str);

      attr[index].at_val.at_str = strdup(val);

      if (attr[index].at_val.at_str == NULL)
        {
        return(PBSE_SYSTEM);
        }

      break;

    case ATR_TYPE_SIZE:

      {
      unsigned long number;

      const char *unit;

      number = strtol(val, NULL, 10);

      attr[index].at_val.at_size.atsv_units = ATR_SV_BYTESZ;
      attr[index].at_val.at_size.atsv_num = number;
      attr[index].at_val.at_size.atsv_shift = 0;

      /* the string always ends with kb,mb if it has units */
      unit = val + strlen(val) - 2;

      if (unit < val)
        break;
      else if (isdigit(*val))
        break;

      switch (*unit)
        {
        case 'k':

          attr[index].at_val.at_size.atsv_shift = 10;

          break;

        case 'm':

          attr[index].at_val.at_size.atsv_shift = 20;

          break;

        case 'g':

          attr[index].at_val.at_size.atsv_shift = 30;

          break;

        case 't':

          attr[index].at_val.at_size.atsv_shift = 40;

          break;

        case 'p':

          attr[index].at_val.at_size.atsv_shift = 50;

          break;

        }
      }

      break;
    
    case ATR_TYPE_LL:
      
      attr[index].at_val.at_ll = strtoll(val, NULL, 10);

      break;

    /* NYI */
    case ATR_TYPE_LIST:
    case ATR_TYPE_SHORT:
    case ATR_TYPE_JINFOP:

      break;

    case ATR_TYPE_BOOL:

      if ((val[0] == 'T') ||
          (val[0] == 't'))
        attr[index].at_val.at_bool = true;
      else
        attr[index].at_val.at_bool = false;

      break;

    default:

      break;
    } /* END switch (pbs_attribute type) */

  attr[index].at_flags |= ATR_VFLAG_SET;

  return(PBSE_NONE);
  }


int attr_to_str(

  std::string      &ds,     /* O */
  attribute_def    *at_def, /* I */
  pbs_attribute    &attr,   /* I */
  bool              XML)    /* I */

  {
  /* used to print numbers and chars as strings */
  char local_buf[MAXLINE];

  if ((attr.at_flags & ATR_VFLAG_SET) == FALSE)
    return(NO_ATTR_DATA);

  switch (at_def->at_type)
    {

    case ATR_TYPE_BOOL:
     
      snprintf(local_buf, sizeof(local_buf), attr.at_val.at_bool ? "true" : "false");
      ds += local_buf;
      
      break;

    case ATR_TYPE_LONG:

      snprintf(local_buf, sizeof(local_buf), "%ld", attr.at_val.at_long);
      ds += local_buf;

      break;

    case ATR_TYPE_CHAR:

      sprintf(local_buf, "%c", attr.at_val.at_char);
      ds += local_buf;

      break;

    case ATR_TYPE_STR:

      if (attr.at_val.at_str == NULL)
        return(NO_ATTR_DATA);

      if (strlen(attr.at_val.at_str) == 0)
        return(NO_ATTR_DATA);

      ds += attr.at_val.at_str;

      break;

    case ATR_TYPE_ARST:
    case ATR_TYPE_ACL:

      {
      int j;
      struct array_strings *arst = attr.at_val.at_arst;

      if (arst == NULL)
        return(NO_ATTR_DATA);

      /* concatenate all of the array strings into one string */
      for (j = 0; j < arst->as_usedptr; j++)
        {
        if (j > 0)
          ds += ",";

        ds += arst->as_string[j];
        }
      }

      break;

    case ATR_TYPE_RESC:

      {
      if (attr.at_val.at_ptr == NULL)
        return(NO_ATTR_DATA);

      std::vector<resource> *resources = (std::vector<resource> *)attr.at_val.at_ptr;

      // print all of the resources
      for (size_t i = 0; i < resources->size(); i++)
        {
        resource &r = resources->at(i);

        /* there are only 3 resource types used */
        switch (r.rs_value.at_type)
          {
          case ATR_TYPE_LONG:

            ds += "\t\t<";
            ds += r.rs_defin->rs_name;
            ds += ">";

            snprintf(local_buf, sizeof(local_buf), "%ld", r.rs_value.at_val.at_long);
            ds += local_buf;

            ds += "</";
            ds += r.rs_defin->rs_name;
            ds += ">";

            break;

          case ATR_TYPE_STR:

            /* Patch provided by Martin Siegert to fix seg-fault
             * when r.rs_value.at_val.at_str is NULL 
             * Bugzilla bug 101 
             */

            if (r.rs_value.at_val.at_str == NULL)
              break;

            if (strlen(r.rs_value.at_val.at_str) == 0)
              break;

            ds += "\t\t<";
            ds += r.rs_defin->rs_name;
            ds += ">";

            
            ds += r.rs_value.at_val.at_str;

            ds += "</";
            ds += r.rs_defin->rs_name;
            ds += ">";

            break;
          }

        ds += "\n";
        } // END for each resource
      }

      break;

    case ATR_TYPE_TV:
      /* Record seconds and milliseconds */
      sprintf(local_buf, "%ld.%ld", attr.at_val.at_timeval.tv_sec, attr.at_val.at_timeval.tv_usec);
      ds += local_buf;
      break;
    
    case ATR_TYPE_LL:

      snprintf(local_buf, sizeof(local_buf), "%lld", attr.at_val.at_ll);
      ds += local_buf;

      break;

    /* NYI */
    case ATR_TYPE_LIST:
    case ATR_TYPE_SHORT:
    case ATR_TYPE_JINFOP:

      break;

    default:

      break;
    } /* END switch pbs_attribute type */

  return(PBSE_NONE);
  }


