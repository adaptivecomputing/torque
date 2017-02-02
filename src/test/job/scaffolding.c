#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include "log.h"
#include "attribute.h"

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

