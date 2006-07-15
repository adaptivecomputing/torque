#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_CPALIB_H
#include <cpalib.h>
#endif
#include "server_limits.h"
#include "list_link.h"
#include "attribute.h"
#include "job.h"
#include "log.h"


#ifdef ENABLE_CPA

extern int LOGLEVEL;


int CPACreatePartition(job *pjob, struct var_table *vtab)


  int   UID,         /* I */
  char *AcctID,      /* I */
  char *JobID,       /* I */
  char *MasterNode,  /* I - hostname of master yod node */
  unsigned long      *ParID,       /* O - partition id */
  unsigned long long *AdminCookie, /* O - admin cookie */
  unsigned long long *AllocCookie) /* O - alloc cookie */

  {
  cpa_node_req_t *NodeReq;

  int rc;

  char *Value;

  char *Spec;

  int   PPN;
  int   Flags;
  int   Size=0;
  int   UID;

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

  UID = pjob->ji_qs.ji_euid;

  if ((Size <= 0) || (UID < 0))
    {
    /* FAILURE */

    printf("ERROR:  invalid parameters:  S: %d  U: %d  M: %s\n",
      Size,
      UID,
      (MasterNode != NULL) ? MasterNode : "NULL");

    return(1);
    }

  PPN = 1;       /* NOTE:  not really supported w/in CPA, always use 1
*/
  Flags = 0;     /* NOTE:  only allocate compute hosts, always use 0 */
  Spec = NULL;   /* NOTE: required node specification, not used */
  Wanted = NULL; /* NOTE: list of nodes from which to select resources,
not used */

  NodeReq = cpa_new_node_req(
    Size, /* number of procs/nodes required by job */
    PPN,
    Flags,
    Spec,
    Wanted);

  if (NodeReq == NULL)
    {
    /* FAILURE:  cannot alloc memory for node req */

    printf("cpa_new_node_req: NULL\n");

    return(1);
    }

  rc = cpa_create_partition(
    NodeReq,
    CPA_BATCH,
    CPA_NOT_SPECIFIED,
    UID,
    (AcctID != NULL) ? AcctID : "DEFAULT",
    (cpa_partition_id_t *)ParID,   /* O */
    (cpa_cookie_t *)AdminCookie,   /* O */
    (cpa_cookie_t *)AllocCookie);  /* O */

  if (rc != 0)
    {
    /* FAILURE */

    printf("cpa_create_partition: rc=%d (%s)\n",
      rc,
      cpa_rc2str(rc));

    return(1);
    }

  rc = cpa_assign_partition(
    (cpa_partition_id_t)*ParID,
    (cpa_cookie_t)*AdminCookie,
    pjob->ji_qs.ji_jobid,
    1);     /* NOT CURRENTLY USED - should be set to NID of 'master
host' */

  if (rc != 0)
    {
    /* FAILURE */

    printf("cpa_assign_partition: rc=%d (%s)\n",
      rc,
      cpa_rc2str(rc));

    return(1);
    }         

  return(0);
  }  /* END CPACreatePartition() */




/* NOTE:  send SIGTERM, allow 30 seconds, SIGKILL */
/* NOTE:  allow configurable delay between term and kill */

int CPADestroyPartition(

  unsigned long      ParID,         /* I */
  unsigned long long AdminCookie,   /* I */
  int *ErrorP)        /* O - non-zero if users of partition encountered
error */

  {
  int rc;             

  /* will fail if yod is present */

  /* call at epilog time */
                        
  if (LOGLEVEL >= 2)          
    {              
    printf("INFO:  destroying partition %ld with cookie %ld\n",
      ParID,                                                   
      AdminCookie);
    }

  rc = cpa_destroy_partition(
    ParID,
    AdminCookie, 
    ErrorP);      /* O - if set, destroy failed on one or more tasks */

  if (rc != 0)
    {
    printf("cpa_destroy_partition: rc=%d (%s)\n",
      rc,
      cpa_rc2str(rc));

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
