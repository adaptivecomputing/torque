#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* pbs_attribute, batch_op */
#include "list_link.h" /* tlist_head */
#include "pbs_job.h"


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

int comp_depend(
    
  pbs_attribute *attr, 
  pbs_attribute *with)

  {
  return (-1);
  }

void free_depend(

  pbs_attribute *attr)

  {
  struct depend     *pdp;
  struct depend_job *pdjb;

  while ((pdp = (struct depend *)GET_NEXT(attr->at_val.at_list)))
    {
    for (unsigned int i = 0; i < pdp->dp_jobs.size(); i++)
      {
      pdjb = pdp->dp_jobs[i];

      free(pdjb);
      }

    delete_link(&pdp->dp_link);

    free(pdp);
    }

  attr->at_flags &= ~ATR_VFLAG_SET;

  return;
  }  /* END comp_depend() */

#define ATR_DFLAG_SSET  (ATR_DFLAG_SvWR | ATR_DFLAG_SvRD)

int encode_exec_host(

  pbs_attribute  *attr,    /* ptr to pbs_attribute */
  tlist_head     *phead,   /* head of attrlist */
  const char    *atname,  /* name of pbs_attribute */
  const char    *rsname,  /* resource name or null */
  int             mode,    /* encode mode, unused here */
  int             perm)    /* only used for resources */

  {
  char *old_str;
  char *export_str;
  char *pipe;
  int   rc;

  if (attr->at_val.at_str == NULL)
    return(PBSE_NONE);

  if ((export_str = strdup(attr->at_val.at_str)) == NULL)
    return(PBSE_SYSTEM);

  while ((pipe = strchr(export_str, '|')) != NULL)
    *pipe = '+';

  old_str = attr->at_val.at_str;
  attr->at_val.at_str = export_str;

  rc = encode_str(attr, phead, atname, rsname, mode, perm);
  attr->at_val.at_str = old_str;
  free(export_str);

  return(rc);
  } /* END encode_exec_host() */

/*
 * The entries for each pbs_attribute are (see attribute.h):
 * name,
 * decode function,
 * encode function,
 * set function,
 * compare function,
 * free value space function,
 * action function,
 * access permission flags,
 * value type,
 * parent object type
 */

/* sync w/enum job_atr in src/include/pbs_job.h */
/* sync w/ TJobAttr[] in src/resmom/requests.c */

attribute_def job_attr_def[] =
  {

  /* JOB_ATR_jobname */
    { (char *)ATTR_N,   /* "Job_Name" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_ALTRUN | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
    },
  /* JOB_ATR_job_owner */
  { (char *)ATTR_owner,  /* "Job_Owner" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SSET | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_resc_used */
  { (char *)ATTR_used,  /* "Resources_Used" */
    decode_resc,
    encode_resc,
    set_resc,
    comp_resc,
    free_resc,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SvWR,
    ATR_TYPE_RESC,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_state */
  { (char *)ATTR_state,  /* "job_state" */
    decode_c,
    encode_c,
    set_c,
    comp_c,
    free_null,
    NULL_FUNC,
    READ_ONLY,
    ATR_TYPE_CHAR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_in_queue */
  { (char *)ATTR_queue,  /* "Queue" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM /* | ATR_DFLAG_ALTRUN */,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_at_server */
  { (char *)ATTR_server,  /* "Server" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_account */
  { (char *)ATTR_A,   /* "Account_Name" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_checkpoint */
  { (char *)ATTR_c,   /* "Checkpoint" */
    decode_str,
    encode_str,
    set_str,
#ifdef PBS_MOM
    comp_str,
#else /* PBS_MOM - server side */
    comp_checkpoint,
#endif /* PBS_MOM */
    free_str,
#ifdef PBS_MOM
    NULL_FUNC,
#else /* PBS_MOM - server side */
#if 0
    ck_checkpoint,
#else
    0,
#endif
#endif /* PBS_MOM */
    READ_WRITE | ATR_DFLAG_MOM | ATR_DFLAG_ALTRUN,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_ctime */    /* create time, set when the job is queued */
  { (char *)ATTR_ctime,  /* "ctime" */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SSET,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_depend */
  { (char *)ATTR_depend,   /* "depend" */
#ifndef PBS_MOM
    decode_depend,
    encode_depend,
    set_depend,
    comp_depend,
    free_depend,
    depend_on_que,
#else
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
#endif /* PBS_MOM */
    READ_WRITE,
    ATR_TYPE_LIST,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_errpath */
  { (char *)ATTR_e,   /* "Error_Path" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_ALTRUN | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_exec_host */
  { (char *)ATTR_exechost,  /* "exec_host" */
    decode_str,
    encode_exec_host,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_MOM | ATR_DFLAG_OPWR | ATR_DFLAG_SvWR,  /* allow operator/scheduler to modify exec_host */
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_exec_port */
  { (char *)ATTR_execport,  /* "exec_port" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_MOM | ATR_DFLAG_OPWR | ATR_DFLAG_SvWR,  /* allow operator/scheduler to modify exec_port */
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_exec_gpus */
  { (char *)ATTR_exec_gpus,  /* exec_gpus */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_MOM | ATR_DFLAG_OPWR | ATR_DFLAG_SvWR,  /* allow operator/scheduler to modify exec_host */
    ATR_TYPE_STR,
    PARENT_TYPE_JOB,
  },

  /* JOB_ATR_exectime */
  { (char *)ATTR_a,   /* "Execution_Time" (aka release_date) */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
#ifndef PBS_MOM
    job_set_wait,
#else
    NULL_FUNC,
#endif
    READ_WRITE | ATR_DFLAG_ALTRUN,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_grouplst */
  { (char *)ATTR_g,   /* "group_list" */
    decode_arst,
    encode_arst,
    set_arst,
    comp_arst,
    free_arst,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_SELEQ,
    ATR_TYPE_ARST,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_hold */
  { (char *)ATTR_h,   /* "Hold_Types" */
    decode_hold,
    encode_hold,
    set_b,
    comp_hold,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_ALTRUN | ATR_DFLAG_SELEQ,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_interactive */
  { (char *)ATTR_inter,  /* "interactive" */
    decode_l,
    encode_inter,
    set_l,
    comp_b,
    free_null,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SvRD | ATR_DFLAG_Creat | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_join */
  { (char *)ATTR_j,   /* "Join_Path" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_keep */
  { (char *)ATTR_k,   /* "Keep_Files" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_mailpnts */
  { (char *)ATTR_m,   /* "Mail_Points" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_ALTRUN | ATR_DFLAG_SELEQ,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_mailuser */
  { (char *)ATTR_M,   /* "Mail_Users" */
    decode_arst,
    encode_arst,
    set_arst,
    comp_arst,
    free_arst,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_ALTRUN | ATR_DFLAG_SELEQ,
    ATR_TYPE_ARST,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_mtime */
  { (char *)ATTR_mtime,  /* "mtime" */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SSET,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_outpath */
  { (char *)ATTR_o,   /* "Output_Path" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_ALTRUN | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_priority */
  { (char *)ATTR_p,   /* "Priority" */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_ALTRUN,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_qtime */
  { (char *)ATTR_qtime,  /* "qtime"  (time entered queue) */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_ONLY,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_rerunable */
  { (char *)ATTR_r,   /* "Rerunable" */
    decode_b,
    encode_b,
    set_b,
    comp_b,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_ALTRUN | ATR_DFLAG_SELEQ,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_resource */
  { (char *)ATTR_l,   /* "Resource_List" */
    decode_resc,
    encode_resc,
    set_resc,
    comp_resc,
    free_resc,
    action_resc,
    READ_WRITE | ATR_DFLAG_ALTRUN | ATR_DFLAG_MOM,
    ATR_TYPE_RESC,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_session_id */
  { (char *)ATTR_session,  /* "session_id" */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SvWR,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_shell */
  { (char *)ATTR_S,   /* "Shell_Path_List" */
    decode_arst,
    encode_arst,
    set_arst,
    comp_arst,
    free_arst,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_ALTRUN | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_ARST,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_stagein */
  { (char *)ATTR_stagein,  /* "stagein" */
    decode_arst,
    encode_arst,
    set_arst,
    comp_arst,
    free_arst,
    NULL_FUNC,
    READ_WRITE,
    ATR_TYPE_ARST,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_stageout */
  { (char *)ATTR_stageout,  /* "stageout" */
    decode_arst,
    encode_arst,
    set_arst,
    comp_arst,
    free_arst,
    NULL_FUNC,
    READ_WRITE,
    ATR_TYPE_ARST,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_substate */
  { (char *)ATTR_substate,  /* "substate" */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    ATR_DFLAG_OPRD | ATR_DFLAG_MGRD,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_userlst */
  { (char *)ATTR_u,   /* "User_List" */
    decode_arst,
    encode_arst,
#ifndef PBS_MOM
    set_uacl,
#else
    set_arst,
#endif
    comp_arst,
    free_arst,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_SELEQ,
    ATR_TYPE_ARST,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_variables (allow to be dynamically modifiable) */
  { (char *)ATTR_v,   /* "Variable_List" */
    decode_arst,
    encode_arst,
    set_arst,
    comp_arst,
    free_arst,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM | ATR_DFLAG_PRIVR | ATR_DFLAG_ALTRUN,
    ATR_TYPE_ARST,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_euser    */
  { (char *)ATTR_euser,  /* "euser" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    ATR_DFLAG_MGRD | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_egroup  */
  { (char *)ATTR_egroup,  /* "egroup" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    ATR_DFLAG_MGRD | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_hashname */
  { (char *)ATTR_hashname,  /* "hashname" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    ATR_DFLAG_MGRD | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_hopcount */
  { (char *)ATTR_hopcount,  /* "hop_count" */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    ATR_DFLAG_SSET,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_queuerank */
  { (char *)ATTR_qrank,  /* "queue_rank" */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    ATR_DFLAG_MGRD,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_queuetype */
  { (char *)ATTR_qtype,  /*"queue_type" - exists for Scheduler select */
    decode_c,
    encode_c,
    set_c,
    comp_c,
    free_null,
    NULL_FUNC,
    ATR_DFLAG_MGRD | ATR_DFLAG_SELEQ,
    ATR_TYPE_CHAR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_sched_hint */
  { (char *)ATTR_sched_hint,        /* "sched_hint" - inform scheduler re sync */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    ATR_DFLAG_SSET | NO_USER_SET | ATR_DFLAG_ALTRUN,  /*ATR_DFLAG_MGRD | ATR_DFLAG_MGWR */
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_security */
  { (char *)ATTR_security,  /* "security" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    ATR_DFLAG_SSET,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_Comment */
  { (char *)ATTR_comment,  /* "comment" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    NO_USER_SET | ATR_DFLAG_SvWR | ATR_DFLAG_ALTRUN,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_Cookie */
  { (char *)ATTR_cookie,  /* "cookie" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    ATR_DFLAG_SvRD | ATR_DFLAG_SvWR | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_altid */
  { (char *)ATTR_altid,  /* "alt_id" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SvWR,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_etime */
  { (char *)ATTR_etime,  /* "etime" */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SSET,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_exitstat */
  { (char *)ATTR_exitstat,  /* "exit_status" */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_ONLY,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },
  /* JOB_ATR_forwardx11 */
  { (char *)ATTR_forwardx11, /* "forward_x11" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    ATR_DFLAG_USWR | ATR_DFLAG_MGRD | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_submit_args */
  { (char *)ATTR_submit_args,
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SvRD | ATR_DFLAG_Creat,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_job_array_id */
  { (char *)ATTR_array_id,
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    ATR_DFLAG_SvRD | READ_ONLY,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_job_array_request */
  { (char *)ATTR_t,
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    ATR_DFLAG_Creat | ATR_DFLAG_SvRD | READ_ONLY,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_umask */
  { (char *)ATTR_umask,
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_start_time */
  { (char *)ATTR_start_time,
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SSET | ATR_DFLAG_MOM,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_start_count */
  { (char *)ATTR_start_count,
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SSET,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_checkpoint_dir */
  { (char *)ATTR_checkpoint_dir,  /* "checkpoint_dir" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_checkpoint_name */
  { (char *)ATTR_checkpoint_name,  /* "checkpoint_name" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM | ATR_DFLAG_ALTRUN,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_checkpoint_time */
  { (char *)ATTR_checkpoint_time,  /* "checkpoint_time" */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    NO_USER_SET | ATR_DFLAG_SvWR | ATR_DFLAG_ALTRUN,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_checkpoint_restart_status */
  { (char *)ATTR_checkpoint_restart_status,  /* "checkpoint_restart_status" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_SvWR | ATR_DFLAG_ALTRUN,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_restart_name */
  { (char *)ATTR_restart_name,  /* "restart_name" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    NO_USER_SET | ATR_DFLAG_SvWR | ATR_DFLAG_ALTRUN | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_f (fault_tolerant)*/
  { (char *)ATTR_f,
    decode_b,
    encode_b,
    set_b,
    comp_b,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_ALTRUN | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_comp_time */
  { (char *)ATTR_comp_time, /* completion time */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SSET,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_reported */
  {	(char *)ATTR_reported,			/* "Reported" */
  	decode_b,
  	encode_b,
  	set_b,
  	comp_b,
  	free_null,
  	NULL_FUNC,
  	READ_ONLY | ATR_DFLAG_SSET,
  	ATR_TYPE_LONG,
  	PARENT_TYPE_JOB
  },

  /* JOB_ATR_jobtype */
  { (char *)ATTR_jobtype,   /* "Job_Type" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_inter_cmd */
  { (char *)ATTR_intcmd,   /* "Interactive_Cmd" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_job_radix */
  { (char *)ATTR_job_radix,  /* "job_radix" */
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    job_radix_action,
    READ_WRITE | ATR_DFLAG_SSET | ATR_DFLAG_MOM,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_total_runtime */
  { (char *)ATTR_total_runtime, /* total time from start_time to comp_time in milliseconds */
    decode_tv,
    encode_tv,
    set_tv,
    comp_tv,
    free_null,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SSET,
    ATR_TYPE_TV,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_sister_list */
  { (char *)ATTR_sister_list, /* sister_list */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_MOM | ATR_DFLAG_OPWR | ATR_DFLAG_SvWR,  /* allow operator/scheduler to modify exec_host */
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_proxy_user */
  { (char *)ATTR_P, /* "proxy_user" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    ATR_DFLAG_Creat | ATR_DFLAG_MGRD | ATR_DFLAG_USRD | ATR_DFLAG_OPRD,
    ATR_TYPE_STR,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_node_exclusive */
  { (char *)ATTR_node_exclusive, /* node_exclusive */
    decode_b,
    encode_b,
    set_b,
    comp_b,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM,
    ATR_TYPE_LONG,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_submit_host */
  {(char *)ATTR_submit_host,		/* "submit_host - undocumented */
   decode_str,
   encode_str,
   set_str,
   comp_str,
   free_str,
   NULL_FUNC,
   READ_ONLY | ATR_DFLAG_SvRD | ATR_DFLAG_Creat,
   ATR_TYPE_STR,
   PARENT_TYPE_JOB},

  /* JOB_ATR_init_work_dir */
  {(char *)ATTR_init_work_dir,		/* "init_work_dir - undocumented */
   decode_str,
   encode_str,
   set_str,
   comp_str,
   free_str,
   NULL_FUNC,
   READ_ONLY | ATR_DFLAG_SvRD | ATR_DFLAG_Creat,
   ATR_TYPE_STR,
   PARENT_TYPE_JOB},

  /* JOB_ATR_pagg_id */
  { (char *)ATTR_pagg,  /* "pagg_id" */
    decode_ll,
    encode_ll,
    set_ll,
    comp_ll,
    free_null,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_SvWR,
    ATR_TYPE_LL,
    PARENT_TYPE_JOB
  },

  /* JOB_ATR_gpu_flags */
  {(char *)ATTR_gpu_flags,		/* "gpu_flags" - mode and reset flags */
   decode_l,
   encode_l,
   set_l,
   comp_l,
   free_null,
   NULL_FUNC,
   READ_ONLY | ATR_DFLAG_MOM,
   ATR_TYPE_LONG,
   PARENT_TYPE_JOB},

  /* JOB_ATR_job_id */
  {(char *)ATTR_J,      /* "job_id" */
   decode_str,
   encode_str,
   set_str,
   comp_str,
   free_str,
   NULL_FUNC,
   ATR_DFLAG_Creat | ATR_DFLAG_MGRD | ATR_DFLAG_USRD | ATR_DFLAG_OPRD,
   ATR_TYPE_STR,
   PARENT_TYPE_JOB},

  /* JOB_ATR_arguments*/
  {(char *)ATTR_args,		/* "job_arguments" */
   decode_str,
   encode_str,
   set_str,
   comp_str,
   free_str,
   NULL_FUNC,
   READ_WRITE | ATR_DFLAG_SELEQ | ATR_DFLAG_MOM,
   ATR_TYPE_STR,
   PARENT_TYPE_JOB},

  /* JOB_ATR_reservation_id */
  {(char *)ATTR_reservation_id, /* "reservation_id" */
   decode_str,
   encode_str,
   set_str,
   comp_str,
   free_str,
   NULL_FUNC,
   READ_WRITE |  ATR_DFLAG_MOM,
   ATR_TYPE_STR,
   PARENT_TYPE_JOB},

  /* JOB_ATR_login_node_id */
  {(char *)ATTR_login_node_id,  /* "login_node_id" */
   decode_str,
   encode_str,
   set_str,
   comp_str,
   free_str,
   NULL_FUNC,
   READ_WRITE,
   ATR_TYPE_STR,
   PARENT_TYPE_JOB},

  /* JOB_ATR_login_prop */
  {(char *)ATTR_login_prop, /* "login_property" */
   decode_str,
   encode_str,
   set_str,
   comp_str,
   free_str,
   NULL_FUNC,
   READ_WRITE,
   ATR_TYPE_STR,
   PARENT_TYPE_JOB},

  /* JOB_ATR_external_nodes */
  {(char *)ATTR_external_nodes, /* external_nodes */
   decode_str,
   encode_str,
   set_str,
   comp_str,
   free_str,
   NULL_FUNC,
   READ_WRITE,
   ATR_TYPE_STR,
   PARENT_TYPE_JOB},

  /* JOB_ATR_multi_req_alps */
  {(char *)ATTR_multi_req_alps, /* "multi_req_alps" */
   decode_str,
   encode_str,
   set_str,
   comp_str,
   free_str,
   NULL_FUNC,
   READ_WRITE | ATR_DFLAG_MOM,
   ATR_TYPE_STR,
   PARENT_TYPE_JOB},

  /* JOB_ATR_exec_mics */
  { (char *)ATTR_exec_mics,  /* "exec_mics" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_ONLY | ATR_DFLAG_MOM | ATR_DFLAG_OPWR | ATR_DFLAG_SvWR,  /* allow operator/scheduler to modify exec_host */
    ATR_TYPE_STR,
    PARENT_TYPE_JOB,
  },

  /* JOB_ATR_system_start_time */
  {ATTR_system_start_time, /* start time as encoded in the proc/pid directory */
   decode_l,
   encode_l,
   set_l,
   comp_l,
   free_null,
   NULL_FUNC,
   READ_ONLY | ATR_DFLAG_MOM,
   ATR_TYPE_LONG,
   PARENT_TYPE_JOB},

  /* JOB_ATR_nppcu */
  {ATTR_nppcu, /* how to handle compute units (on Cray system) */
   decode_l,
   encode_l,
   set_l,
   comp_l,
   free_null,
   NULL_FUNC,
   READ_ONLY | ATR_DFLAG_MOM | ATR_DFLAG_SvWR,
   ATR_TYPE_LONG,
   PARENT_TYPE_JOB},


/* Site defined attributes if any, see site_job_attr_*.h  */
#include "site_job_attr_def.h"

  /* JOB_ATR_UNKN - THIS MUST BE THE LAST ENTRY */
  { (char *)"_other_",
    decode_unkn,
    encode_unkn,
    set_unkn,
    comp_unkn,
    free_unkn,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_SELEQ,
    ATR_TYPE_LIST,
    PARENT_TYPE_JOB
  }
  };
