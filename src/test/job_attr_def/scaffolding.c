#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* pbs_attribute, batch_op */
#include "list_link.h" /* tlist_head */
#include "pbs_error.h"


int set_arst(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_arst needs to be mocked!!\n");
  exit(1);
  }

int comp_arst(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_arst needs to be mocked!!\n");
  exit(1);
  }

void free_arst(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_arst needs to be mocked!!\n");
  exit(1);
  }

int encode_arst(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_arst needs to be mocked!!\n");
  exit(1);
  }

int decode_arst(struct pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_arst needs to be mocked!!\n");
  exit(1);
  }

int set_str(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_str needs to be mocked!!\n");
  exit(1);
  }

int comp_str(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_str needs to be mocked!!\n");
  exit(1);
  }

void free_str(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_str needs to be mocked!!\n");
  exit(1);
  }

int encode_str(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_str needs to be mocked!!\n");
  exit(1);
  }

int decode_str(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_str needs to be mocked!!\n");
  exit(1);
  }

int set_resc(struct pbs_attribute *old, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_resc needs to be mocked!!\n");
  exit(1);
  }

int comp_resc(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_resc needs to be mocked!!\n");
  exit(1);
  }

void free_resc(pbs_attribute *pattr)
  {
  fprintf(stderr, "The call to free_resc needs to be mocked!!\n");
  exit(1);
  }

int encode_resc(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int ac_perm)
  {
  fprintf(stderr, "The call to encode_resc needs to be mocked!!\n");
  exit(1);
  }

int decode_resc(struct pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_resc needs to be mocked!!\n");
  exit(1);
  }

int action_resc(pbs_attribute *pattr, void *pobject, int actmode)
  {
  fprintf(stderr, "The call to action_resc needs to be mocked!!\n");
  exit(1);
  }

int set_unkn(struct pbs_attribute *old, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_unkn needs to be mocked!!\n");
  exit(1);
  }

int comp_unkn(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_unkn needs to be mocked!!\n");
  exit(1);
  }

void free_unkn(pbs_attribute *pattr)
  {
  fprintf(stderr, "The call to free_unkn needs to be mocked!!\n");
  exit(1);
  }

int encode_unkn(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_unkn needs to be mocked!!\n");
  exit(1);
  }

int decode_unkn(pbs_attribute *patr, const char *name,const char *rescn, const char *value, int perm)
  {
  fprintf(stderr, "The call to decode_unkn needs to be mocked!!\n");
  exit(1);
  }

int set_depend(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_depend needs to be mocked!!\n");
  exit(1);
  }

int comp_depend(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_depend needs to be mocked!!\n");
  exit(1);
  }

void free_depend(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_depend needs to be mocked!!\n");
  exit(1);
  }

int encode_depend(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_depend needs to be mocked!!\n");
  exit(1);
  }

int decode_depend(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_depend needs to be mocked!!\n");
  exit(1);
  }

int comp_hold(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_hold needs to be mocked!!\n");
  exit(1);
  }

int encode_hold(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_hold needs to be mocked!!\n");
  exit(1);
  }

int decode_hold(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_hold needs to be mocked!!\n");
  exit(1);
  }

int set_l(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_l needs to be mocked!!\n");
  exit(1);
  }

int comp_l(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_l needs to be mocked!!\n");
  exit(1);
  }

int encode_l(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_l needs to be mocked!!\n");
  exit(1);
  }

int decode_l(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_l needs to be mocked!!\n");
  exit(1);
  }

int set_b(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_b needs to be mocked!!\n");
  exit(1);
  }

int comp_b(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_b needs to be mocked!!\n");
  exit(1);
  }

int encode_b(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_b needs to be mocked!!\n");
  exit(1);
  }

int decode_b(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_b needs to be mocked!!\n");
  exit(1);
  }

int set_c(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_c needs to be mocked!!\n");
  exit(1);
  }

int comp_c(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_c needs to be mocked!!\n");
  exit(1);
  }

int encode_c(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_c needs to be mocked!!\n");
  exit(1);
  }

int decode_c(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_c needs to be mocked!!\n");
  exit(1);
  }

int set_tv(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_tv needs to be mocked!!\n");
  exit(1);
  }

int comp_tv(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_tv needs to be mocked!!\n");
  exit(1);
  }

int encode_tv(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_tv needs to be mocked!!\n");
  exit(1);
  }

int decode_tv(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_tv needs to be mocked!!\n");
  exit(1);
  }

int encode_inter(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_inter needs to be mocked!!\n");
  exit(1);
  }

int depend_on_que(pbs_attribute *pattr, void *pjob, int mode)
  {
  fprintf(stderr, "The call to depend_on_que needs to be mocked!!\n");
  exit(1);
  }

int comp_checkpoint(pbs_attribute *attr, pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_checkpoint needs to be mocked!!\n");
  exit(1);
  }

int job_set_wait(pbs_attribute *pattr, void *pjob, int mode)
  {
  fprintf(stderr, "The call to job_set_wait needs to be mocked!!\n");
  exit(1);
  }

int job_radix_action (pbs_attribute *new_attr, void *pobj, int actmode)
  {
  fprintf(stderr, "The call to job_radix_action  needs to be mocked!!\n");
  exit(1);
  }

int set_uacl(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_uacl needs to be mocked!!\n");
  exit(1);
  }

void free_null(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_null needs to be mocked!!\n");
  exit(1);
  }

int decode_ll(

  pbs_attribute *patr,
  const char   *name,  /* pbs_attribute name */
  const char *rescn, /* resource name, unused here */
  const char    *val,   /* pbs_attribute value */
  int            perm)  /* only used for resources */

  {
  return(0);
  }

int set_ll(

  pbs_attribute *attr,
  pbs_attribute *new_attr,
  enum batch_op  op)

  {
  return(0);
  }

int encode_ll(

  pbs_attribute  *attr,   /* ptr to pbs_attribute */
  tlist_head     *phead,   /* head of attrlist list */
  const char    *atname,  /* pbs_attribute name */
  const char    *rsname,  /* resource name or null */
  int             mode,   /* encode mode, unused here */
  int             perm)  /* only used for resources */

  {
  return(0);
  }

int comp_ll(

  pbs_attribute *attr,
  pbs_attribute *with)

  {
  return(0);
  }

int set_complete_req(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op op)
  {
  return(PBSE_NONE);
  }

void free_complete_req( pbs_attribute *patr)
  {
  return;
  }

int encode_complete_req(
    pbs_attribute *attr, 
    tlist_head    *phead, 
    const char    *atname,
    const char    *rsname,
    int            mode,
    int            perm)
  {
  return(PBSE_NONE);
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
  return(PBSE_NONE);
  }
