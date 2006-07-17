#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#ifdef HAVE_CPALIB_H
#include <cpalib.h>
#endif
#include "portability.h"
#include "server_limits.h"
#include "list_link.h"
#include "attribute.h"
#include "job.h"
#include "log.h"
#include "mom_func.h"
#include "resource.h"


#ifdef ENABLE_CPA

extern int LOGLEVEL;
extern char mom_host[];


/* CPACreatePartition - Cray's cpalib support
 * gets the "size" resource from the job, converts that to a CPA Node Request,
 * creates a partition, assigns it to JobID string, stores the partid and
 * cookies in the job, and sets some env vars in vtable */
int CPACreatePartition(job *pjob, struct var_table *vtab)

  {
  char id[]="CPACreatePartition";

  cpa_node_req_t *NodeReq;

  int rc;

  char *Value;

  char *Spec;

  int   PPN;
  int   Flags;
  int   Size=0;
  int   UID;
  char *AcctID = NULL;
  char *JobID;
  unsigned long      ParID;       /* O - partition id */
  unsigned long long AdminCookie; /* O - admin cookie */
  unsigned long long AllocCookie; /* O - alloc cookie */
  char longbuf[1024];

  resource              *presc;         /* Requested Resource List */
  resource_def          *prd;
  attribute            *pattr;

  cpa_nid_list_t Wanted;

  /* first, get the size, uid, and jobid from the job */
  pattr = &pjob->ji_wattr[(int)JOB_ATR_resource];
  prd = find_resc_def(svr_resc_def,"size",svr_resc_size);
  presc = find_resc_entry(pattr,prd);

  if (presc != NULL)
    {
    Size = presc->rs_value.at_val.at_long;
    }

  UID = pjob->ji_qs.ji_un.ji_momt.ji_exuid;

  if ((Size <= 0) || (UID < 0))
    {
    /* FAILURE */

    sprintf(log_buffer,"ERROR:  invalid parameters:  Size: %d  UID: %d  \n",
      Size,
      UID);

    log_err(-1,id,log_buffer);

    return(1);
    }

  if (pjob->ji_wattr[(int)JOB_ATR_account].at_flags & ATR_VFLAG_SET)
    {
    AcctID=pjob->ji_wattr[(int)JOB_ATR_account].at_val.at_str;
    }

  JobID = pjob->ji_qs.ji_jobid;

  PPN = 1;       /* NOTE:  not really supported w/in CPA, always use 1 */
  Flags = 0;     /* NOTE:  only allocate compute hosts, always use 0 */
  Spec = NULL;   /* NOTE: required node specification, not used */
  Wanted = NULL; /* NOTE: list of nodes from which to select resources, not used */

  NodeReq = cpa_new_node_req(
    Size, /* number of procs/nodes required by job */
    PPN,
    Flags,
    Spec,
    Wanted);

  if (NodeReq == NULL)
    {
    /* FAILURE:  cannot alloc memory for node req */

    sprintf(log_buffer,"cpa_new_node_req: NULL\n");

    log_err(-1,id,log_buffer);

    return(1);
    }

  rc = cpa_create_partition(
    NodeReq,
    CPA_BATCH,
    CPA_NOT_SPECIFIED,
    UID,
    (AcctID != NULL) ? AcctID : "DEFAULT",
    (cpa_partition_id_t *)&ParID,   /* O */
    (cpa_cookie_t *)&AdminCookie,   /* O */
    (cpa_cookie_t *)&AllocCookie);  /* O */

  if (rc != 0)
    {
    /* FAILURE */

    sprintf(log_buffer,"cpa_create_partition: rc=%d (%s)\n",
      rc,
      cpa_rc2str(rc));

    log_err(-1,id,log_buffer);

    return(1);
    }

  rc = cpa_assign_partition(
    (cpa_partition_id_t)ParID,
    (cpa_cookie_t)AdminCookie,
    JobID,
    1);     /* NOT CURRENTLY USED - should be set to NID of 'master host' */

  if (rc != 0)
    {
    /* FAILURE */

    sprintf(log_buffer,"cpa_assign_partition: rc=%d (%s)\n",
      rc,
      cpa_rc2str(rc));

    log_err(-1,id,log_buffer);

    return(1);
    }         

  /* save the partition and cookies in the job and vtab */

  prd = find_resc_def(svr_resc_def,"cpapartition",svr_resc_size);

  if (prd == NULL)
    {
    return(PBSE_SYSTEM);
    }

  if ((presc = find_resc_entry(pattr,prd)) == NULL)
    {
    if ((presc = add_resource_entry(pattr,prd)) == NULL)
      {
      return(PBSE_SYSTEM);
      }
    }
  else
    {
    prd->rs_free(&presc->rs_value);
    }

  snprintf(longbuf,1023,"%lu",ParID);
  prd->rs_decode(&presc->rs_value,NULL,NULL,longbuf);
  presc->rs_value.at_flags |= ATR_VFLAG_SET;
  bld_env_variables(vtab,"BATCH_PARTITION_ID",longbuf);

  prd = find_resc_def(svr_resc_def,"cpaadmincookie",svr_resc_size);

  if (prd == NULL)
    {
    return(PBSE_SYSTEM);
    }

  if ((presc = find_resc_entry(pattr,prd)) == NULL)
    {
    if ((presc = add_resource_entry(pattr,prd)) == NULL)
      {
      return(PBSE_SYSTEM);
      }
    }
  else
    {
    prd->rs_free(&presc->rs_value);
    }

  snprintf(longbuf,1023,"%llu",AdminCookie);
  prd->rs_decode(&presc->rs_value,NULL,NULL,longbuf);
  presc->rs_value.at_flags |= ATR_VFLAG_SET;
  /* admincookie doesn't go into job env */

  prd = find_resc_def(svr_resc_def,"cpaalloccookie",svr_resc_size);

  if (prd == NULL)
    {
    return(PBSE_SYSTEM);
    }

  if ((presc = find_resc_entry(pattr,prd)) == NULL)
    {
    if ((presc = add_resource_entry(pattr,prd)) == NULL)
      {
      return(PBSE_SYSTEM);
      }
    }
  else
    {
    prd->rs_free(&presc->rs_value);
    }

  snprintf(longbuf,1023,"%llu",AllocCookie);
  prd->rs_decode(&presc->rs_value,NULL,NULL,longbuf);
  presc->rs_value.at_flags |= ATR_VFLAG_SET;
  bld_env_variables(vtab,"BATCH_ALLOC_COOKIE",longbuf);

  bld_env_variables(vtab,"BATCH_JOBID",JobID);

  return(0);
  }  /* END CPACreatePartition() */




/* NOTE:  send SIGTERM, allow 30 seconds, SIGKILL */
/* NOTE:  allow configurable delay between term and kill */

int CPADestroyPartition(

  job *pjob)

  {
  char id[]="CPADestroyPartition";

  int rc;             
  int ErrorP;        /* O - non-zero if users of partition encountered error */
  unsigned long ParID;
  unsigned long long AdminCookie;

  resource              *presc;         /* Requested Resource List */
  resource_def          *prd;
  attribute             *pattr;

  pattr = &pjob->ji_wattr[(int)JOB_ATR_resource];

  prd = find_resc_def(svr_resc_def,"cpapartition",svr_resc_size);

  if (prd == NULL)
    {
    return(PBSE_SYSTEM);
    }

  if ((presc = find_resc_entry(pattr,prd)) == NULL)
    {
    return(PBSE_SYSTEM);  /* is this a real error? */
    }

  ParID = strTouL(presc->rs_value.at_val.at_str,NULL,10);

  prd = find_resc_def(svr_resc_def,"cpaadmincookie",svr_resc_size);

  if (prd == NULL)
    {
    return(PBSE_SYSTEM);
    }

  if ((presc = find_resc_entry(pattr,prd)) == NULL)
    {
    return(PBSE_SYSTEM);  /* is this a real error? */
    }

  AdminCookie = strTouL(presc->rs_value.at_val.at_str,NULL,10);

  if (LOGLEVEL >= 2)          
    {              
    printf("INFO:  destroying partition %lu with cookie %llu\n",
      ParID,                                                   
      AdminCookie);

    log_record(
      PBSEVENT_JOB,
      PBS_EVENTCLASS_JOB,
      pjob->ji_qs.ji_jobid,
      log_buffer);
    }

  /* will fail if yod is present */

  rc = cpa_destroy_partition(
    ParID,
    AdminCookie, 
    &ErrorP);      /* O - if set, destroy failed on one or more tasks */

  if (rc != 0)
    {
    sprintf(log_buffer,"cpa_destroy_partition: rc=%d (%s)\n",
      rc,
      cpa_rc2str(rc));

    log_err(-1,id,log_buffer);

    return(1);
    }
     
  return(0);
  }  /* END CPADestroyPartition() */




int CPASetJobEnv(

  unsigned long  ParID,        /* I */
  unsigned long  AllocCookie,  /* I */
  char          *JobID)        /* I */

  {
  char *ptr;

  char  tmpNum[64];

  const char *ParVar = "BATCH_PARTITION_ID";
  const char *AllVar = "BATCH_ALLOC_COOKIE";
  const char *JobVar = "BATCH_JOBID";

  /* NOTE:  add two bytes to malloc for '=' and '\0' */

  sprintf(tmpNum,"%ld",
    ParID);

  if (tmpNum[0] != '\0')
    {
    ptr = (char *)malloc(strlen(tmpNum) + strlen(ParVar) + 2);

    if (ptr == NULL)
      {
      return(1);                                               
      }

    sprintf(ptr,"%s=%s",
      ParVar,
      tmpNum);

    putenv(ptr);
    }

  sprintf(tmpNum,"%ld",
    AllocCookie);

  if (tmpNum[0] != '\0')
    {
    ptr = (char *)malloc(strlen(tmpNum) + strlen(AllVar) + 2);

    if (ptr == NULL)
      {
      return(1);
      }         
       
    sprintf(ptr,"%s=%s",
      AllVar,           
      tmpNum);   

    putenv(ptr);
    }

  if (JobID != NULL)
    {
    ptr = (char *)malloc(strlen(JobID) + strlen(JobVar) + 2);

    if (ptr == NULL)
      {             
      return(1);
      }

    sprintf(ptr,"%s=%s",
      JobVar,
      JobID);

    putenv(ptr);
    }

  return(0);
  }  /* END CPASetJobEnv() */

#endif /* ENABLE_CPA */
