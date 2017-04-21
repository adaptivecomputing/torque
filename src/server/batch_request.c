/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
*
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
*
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
*
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
*
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
*
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
*
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
*
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
*
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
*
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
*
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
*
* 7. DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/

#include <stdio.h>
#include <string.h>

#include "batch_request.h"
#include "utils.h"

/**
 * get_batch_request_id
 *
 * Add the batch request to the brh hash table and put the id
 * into the batch request. This allows the batch request to be
 * retrieved on a delayed invocation.
 */


int brh_index = 0;

int get_batch_request_id(

  batch_request *preq)

  {
  int  id;
  char buf[MAXLINE];

  brh.lock();
  id = brh_index++;
  brh.unlock();

  snprintf(buf, sizeof(buf), "%d", id);

  preq->rq_id = buf;

  insert_batch_request(preq);

  return(PBSE_NONE);
  } /* END get_batch_request_id() */


int insert_batch_request(

  batch_request *preq)

  {
  int rc;

  brh.lock();

  if (!brh.insert(preq, preq->rq_id))
    {
    rc = ENOMEM;
    log_err(rc, __func__, "No memory to resize the array...SYSTEM FAILURE\n");
    }
  else
    {
    rc = PBSE_NONE;
    }

  brh.unlock();

  return(rc);
  } /* END insert_batch_request() */


/*
 * get_batch_request
 *
 * Get the batch request from the hash table.
 */

batch_request *get_batch_request(

  const char *br_id)

  {
  batch_request *preq = NULL;

  brh.lock();
  preq = brh.find(br_id);
  brh.unlock();
  
  return(preq);
  } /* END get_batch_request() */


/*
 * get_remove_batch_request
 *
 * Get the batch request from the hash table and delete it from
 * the hash table.
 */


batch_request *get_remove_batch_request(

  const char *br_id)

  {
  batch_request *preq = NULL;

  brh.lock();
  preq = brh.find(br_id);
  if(preq != NULL)
    {
    brh.remove(br_id);
    }
  brh.unlock();
  
  return(preq);
  } /* END get_remove_batch_request() */




int remove_batch_request(

  const char *br_id)

  {
  brh.lock();
  bool found = brh.remove(br_id);
  brh.unlock();

  if (!found)
    return(THING_NOT_FOUND);
  else
    return(PBSE_NONE);
  } /* END remove_batch_request() */



// Constructor with a type
batch_request::batch_request(

  int type) : rq_type(type), rq_perm(0), rq_fromsvr(0), rq_conn(-1), rq_orgconn(-1), rq_extsz(0),
              rq_time(time(NULL)), rq_refcount(0), rq_extra(NULL), rq_noreply(false),
              rq_failcode(0), rq_extend(NULL), rq_id()

  {
  memset(this->rq_user, 0, sizeof(this->rq_user));
  memset(this->rq_host, 0, sizeof(this->rq_host));
  memset(&this->rq_ind, 0, sizeof(this->rq_ind));
  memset(&this->rq_reply, 0, sizeof(this->rq_reply));

  this->rq_reply.brp_choice = BATCH_REPLY_CHOICE_NULL;
  }



// Empty constructor
batch_request::batch_request() : rq_type(-1), rq_perm(0), rq_fromsvr(0), rq_conn(-1),
                                 rq_orgconn(-1), rq_extsz(0), rq_time(time(NULL)), rq_refcount(0),
                                 rq_extra(NULL), rq_noreply(false), rq_failcode(0),
                                 rq_extend(NULL), rq_id()

  {
  memset(this->rq_user, 0, sizeof(this->rq_user));
  memset(this->rq_host, 0, sizeof(this->rq_host));
  memset(&this->rq_ind, 0, sizeof(this->rq_ind));
  memset(&this->rq_reply, 0, sizeof(this->rq_reply));

  this->rq_reply.brp_choice = BATCH_REPLY_CHOICE_NULL;
  }



batch_request::batch_request(

  Json::Value &header) : rq_type(-1), rq_perm(0), rq_fromsvr(0), rq_conn(-1), rq_orgconn(-1),
                         rq_extsz(0), rq_time(time(NULL)), rq_refcount(0), rq_extra(NULL),
                         rq_noreply(false), rq_failcode(0), rq_extend(0), rq_id()

  {
  int version;
  // We must have a protocol version, a command, and a message or all is lost
  if ((header["command"].empty()) ||
      (header["protocol_version"].empty()) ||
      (header["message"].empty()))
    {
    throw(-1);
    }
  else
    {
    this->rq_type = header["command"].asInt();

    // We cannot deal with future versions
    version = header["protocol_version"].asInt();
    if (version > PBS_BATCH_PROT_VER)
      throw(-1);
    }

  Json::Value &message = header["message"];
  if (message["user"].empty())
    throw(-1);
  else
    snprintf(this->rq_user, sizeof(this->rq_user), "%s", message["user"].asString().c_str());

  switch (this->rq_type)
    {

    case PBS_BATCH_RunJob:

      // Must have a job id
      if (message["jobid"].empty())
        throw(-1);

      snprintf(this->rq_ind.rq_run.rq_jid, sizeof(this->rq_ind.rq_run.rq_jid),
        "%s", message["jobid"].asString().c_str());

      // May be asynchronous
      if (message["asynchronous"].empty() == false)
        this->rq_type = PBS_BATCH_AsyrunJob;

      // May specify a hostlist
      if (message["hostlist"].empty() == false)
        this->rq_ind.rq_run.rq_destin = strdup(message["hostlist"].asString().c_str());
      else
        this->rq_ind.rq_run.rq_destin = NULL;

      break;

    default:

      // NYI or incompatible
      throw(-1);

      break;
    }
  }



int batch_request::copy_attribute_list(

  const batch_request &other)

  {
  svrattrl             *pal = (svrattrl *)GET_NEXT(other.rq_ind.rq_manager.rq_attr);
  tlist_head           *phead = &this->rq_ind.rq_manager.rq_attr;
  svrattrl             *newpal = NULL;
  
  while (pal != NULL)
    {
    newpal = (svrattrl *)calloc(1, pal->al_tsize + 1);
    if (!newpal)
      {
      return(PBSE_SYSTEM);
      }

    CLEAR_LINK(newpal->al_link);
    
    newpal->al_atopl.next = 0;
    newpal->al_tsize = pal->al_tsize + 1;
    newpal->al_nameln = pal->al_nameln;
    newpal->al_flags  = pal->al_flags;
    newpal->al_atopl.name = (char *)newpal + sizeof(svrattrl);
    strcpy((char *)newpal->al_atopl.name, pal->al_atopl.name);
    newpal->al_nameln = pal->al_nameln;
    newpal->al_atopl.resource = newpal->al_atopl.name + newpal->al_nameln;

    if (pal->al_atopl.resource != NULL)
      strcpy((char *)newpal->al_atopl.resource, pal->al_atopl.resource);

    newpal->al_rescln = pal->al_rescln;
    newpal->al_atopl.value = newpal->al_atopl.name + newpal->al_nameln + newpal->al_rescln;
    strcpy((char *)newpal->al_atopl.value, pal->al_atopl.value);
    newpal->al_valln = pal->al_valln;
    newpal->al_atopl.op = pal->al_atopl.op;
    
    pal = (struct svrattrl *)GET_NEXT(pal->al_link);
    }
  
  if ((phead != NULL) &&
      (newpal != NULL))
    append_link(phead, &newpal->al_link, newpal);

  return(PBSE_NONE);
  } /* END copy_attribute_list() */



void batch_request::update_object_id(

  int job_index)

  {
  if (job_index >= 0)
    {
    /* If this is a job array it is possible we only have the array name
       and not the individual job. We need to find out what we have and
       modify the name if needed */
    char  newjobname[PBS_MAXSVRJOBID+1];
    char *ptr1 = strstr(this->rq_ind.rq_manager.rq_objname, "[]");

    if (ptr1)
      {
      char *ptr2;
      ptr1++;
      strcpy(newjobname, this->rq_ind.rq_manager.rq_objname);

      if ((ptr2 = strstr(newjobname, "[]")) != NULL)
        {
        ptr2++;
        *ptr2 = 0;
        }

      sprintf(this->rq_ind.rq_manager.rq_objname,"%s%d%s", 
        newjobname, job_index, ptr1);
      }
    }
  } // END update_object_id()



// Copy constructor
batch_request::batch_request(

  const batch_request &other) : rq_type(other.rq_type), rq_perm(other.rq_perm),
                                rq_fromsvr(other.rq_fromsvr), rq_conn(other.rq_conn),
                                rq_orgconn(other.rq_orgconn), rq_extsz(other.rq_extsz),
                                rq_time(other.rq_time), rq_refcount(other.rq_refcount),
                                rq_noreply(other.rq_noreply), rq_failcode(other.rq_failcode),
                                rq_id(other.rq_id)

  {
  if (other.rq_extra == NULL)
    this->rq_extra = NULL;
  else
    this->rq_extra = strdup((char *)other.rq_extra);

  if (other.rq_extend == NULL)
    this->rq_extend = NULL;
  else
    this->rq_extend = strdup(other.rq_extend);

  strcpy(this->rq_user, other.rq_user);
  strcpy(this->rq_host, other.rq_host);
  memcpy(&this->rq_reply, &other.rq_reply, sizeof(this->rq_reply));

  switch (this->rq_type)
    {
    /* This function was created for a modify array request (PBS_BATCH_ModifyJob)
       the other.rq_ind structure was allocated in dis_request_read. If other
       BATCH types are needed refer to that function to see how the rq_ind structure
       was allocated and then copy it here. */
    case PBS_BATCH_DeleteJob:
    case PBS_BATCH_HoldJob:
    case PBS_BATCH_CheckpointJob:
    case PBS_BATCH_ModifyJob:
    case PBS_BATCH_AsyModifyJob:

      memcpy(this->rq_ind.rq_manager.rq_objname,
        other.rq_ind.rq_manager.rq_objname, PBS_MAXSVRJOBID + 1);
      
      /* based on how decode_DIS_Manage allocates data */
      CLEAR_HEAD(this->rq_ind.rq_manager.rq_attr);
      
      this->rq_ind.rq_manager.rq_cmd = other.rq_ind.rq_manager.rq_cmd;
      this->rq_ind.rq_manager.rq_objtype = other.rq_ind.rq_manager.rq_objtype;
      
      strcpy(this->rq_ind.rq_manager.rq_objname, other.rq_ind.rq_manager.rq_objname);
      this->copy_attribute_list(other);
      
      break;

    case PBS_BATCH_SignalJob:

      strcpy(this->rq_ind.rq_signal.rq_jid, other.rq_ind.rq_signal.rq_jid);
      strcpy(this->rq_ind.rq_signal.rq_signame, other.rq_ind.rq_signal.rq_signame);

      break;

    case PBS_BATCH_MessJob:

      strcpy(this->rq_ind.rq_message.rq_jid, other.rq_ind.rq_message.rq_jid);
      this->rq_ind.rq_message.rq_file = other.rq_ind.rq_message.rq_file;

      if (other.rq_ind.rq_message.rq_text != NULL)
        this->rq_ind.rq_message.rq_text = strdup(other.rq_ind.rq_message.rq_text);

      break;

    case PBS_BATCH_RunJob:
    case PBS_BATCH_AsyrunJob:

      strcpy(this->rq_ind.rq_run.rq_jid, other.rq_ind.rq_run.rq_jid);
  
      if (other.rq_ind.rq_run.rq_destin)
        this->rq_ind.rq_run.rq_destin = strdup(other.rq_ind.rq_run.rq_destin);

      break;

    case PBS_BATCH_Rerun:

      strcpy(this->rq_ind.rq_rerun, other.rq_ind.rq_rerun);
      break;
      
    default:

      break;
    }
  }
  


batch_request &batch_request::operator =(
    
  const batch_request &other)

  {
  this->rq_type = other.rq_type;
  this->rq_perm = other.rq_perm;
  this->rq_fromsvr = other.rq_fromsvr;
  this->rq_conn = other.rq_conn;
  this->rq_orgconn = other.rq_orgconn;
  this->rq_extsz = other.rq_extsz;
  this->rq_time = other.rq_time;
  this->rq_refcount = other.rq_refcount;
  this->rq_noreply = other.rq_noreply;
  this->rq_failcode = other.rq_failcode;
  
  if (other.rq_extra == NULL)
    this->rq_extra = NULL;
  else
    this->rq_extra = strdup((char *)other.rq_extra);

  if (other.rq_extend == NULL)
    this->rq_extend = NULL;
  else
    this->rq_extend = strdup(other.rq_extend);

  this->rq_id = other.rq_id;
  
  strcpy(this->rq_user, other.rq_user);
  strcpy(this->rq_host, other.rq_host);
  memcpy(&this->rq_reply, &other.rq_reply, sizeof(this->rq_reply));

  switch (this->rq_type)
    {
    /* This function was created for a modify array request (PBS_BATCH_ModifyJob)
       the other.rq_ind structure was allocated in dis_request_read. If other
       BATCH types are needed refer to that function to see how the rq_ind structure
       was allocated and then copy it here. */
    case PBS_BATCH_DeleteJob:
    case PBS_BATCH_HoldJob:
    case PBS_BATCH_CheckpointJob:
    case PBS_BATCH_ModifyJob:
    case PBS_BATCH_AsyModifyJob:

      memcpy(this->rq_ind.rq_manager.rq_objname,
        other.rq_ind.rq_manager.rq_objname, PBS_MAXSVRJOBID + 1);
      
      /* based on how decode_DIS_Manage allocates data */
      CLEAR_HEAD(this->rq_ind.rq_manager.rq_attr);
      
      this->rq_ind.rq_manager.rq_cmd = other.rq_ind.rq_manager.rq_cmd;
      this->rq_ind.rq_manager.rq_objtype = other.rq_ind.rq_manager.rq_objtype;
      
      strcpy(this->rq_ind.rq_manager.rq_objname, other.rq_ind.rq_manager.rq_objname);
      this->copy_attribute_list(other);
      
      break;

    case PBS_BATCH_SignalJob:

      strcpy(this->rq_ind.rq_signal.rq_jid, other.rq_ind.rq_signal.rq_jid);
      strcpy(this->rq_ind.rq_signal.rq_signame, other.rq_ind.rq_signal.rq_signame);

      break;

    case PBS_BATCH_MessJob:

      strcpy(this->rq_ind.rq_message.rq_jid, other.rq_ind.rq_message.rq_jid);
      this->rq_ind.rq_message.rq_file = other.rq_ind.rq_message.rq_file;

      if (other.rq_ind.rq_message.rq_text != NULL)
        this->rq_ind.rq_message.rq_text = strdup(other.rq_ind.rq_message.rq_text);

      break;

    case PBS_BATCH_RunJob:
    case PBS_BATCH_AsyrunJob:
 
      strcpy(this->rq_ind.rq_run.rq_jid, other.rq_ind.rq_run.rq_jid);
      if (other.rq_ind.rq_run.rq_destin)
        this->rq_ind.rq_run.rq_destin = strdup(other.rq_ind.rq_run.rq_destin);

      break;

    case PBS_BATCH_Rerun:

      strcpy(this->rq_ind.rq_rerun, other.rq_ind.rq_rerun);
      break;
      
    default:

      break;
    }

  return(*this);
  } // END = operator



void freebr_manage(

  struct rq_manage *pmgr)

  {
  free_attrlist(&pmgr->rq_attr);

  return;
  }  /* END freebr_manage() */



void freebr_cpyfile(

  struct rq_cpyfile *pcf)

  {
  struct rqfpair *ppair;

  while ((ppair = (struct rqfpair *)GET_NEXT(pcf->rq_pair)) != NULL)
    {
    delete_link(&ppair->fp_link);

    if (ppair->fp_local != NULL)
      free(ppair->fp_local);

    if (ppair->fp_rmt != NULL)
      free(ppair->fp_rmt);

    free(ppair);
    }

  return;
  }  /* END freebr_cpyfile() */



void free_rescrq(

  struct rq_rescq *pq)

  {
  int i;

  i = pq->rq_num;

  while (i--)
    {
    if (*(pq->rq_list + i) != NULL)
      free(*(pq->rq_list + i));
    }

  if (pq->rq_list != NULL)
    free(pq->rq_list);

  return;
  }  /* END free_rescrq() */



batch_request::~batch_request()

  {
  if (this->rq_id.size() != 0)
    {
    remove_batch_request(this->rq_id.c_str());
    }

  reply_free(&this->rq_reply);

  if (this->rq_extend) 
    {
    free(this->rq_extend);
    this->rq_extend = NULL;
    }

  if (this->rq_extra)
    {
    free(this->rq_extra);
    this->rq_extra = NULL;
    }

  switch (this->rq_type)
    {
    case PBS_BATCH_QueueJob:
    case PBS_BATCH_QueueJob2:

      free_attrlist(&this->rq_ind.rq_queuejob.rq_attr);

      break;

    case PBS_BATCH_JobCred:

      if (this->rq_ind.rq_jobcred.rq_data)
        {
        free(this->rq_ind.rq_jobcred.rq_data);
        this->rq_ind.rq_jobcred.rq_data = NULL;
        }

      break;

    case PBS_BATCH_MvJobFile:
    case PBS_BATCH_jobscript:
    case PBS_BATCH_jobscript2:

      if (this->rq_ind.rq_jobfile.rq_data)
        {
        free(this->rq_ind.rq_jobfile.rq_data);
        this->rq_ind.rq_jobfile.rq_data = NULL;
        }
      break;

    case PBS_BATCH_HoldJob:

      freebr_manage(&this->rq_ind.rq_hold.rq_orig);

      break;

    case PBS_BATCH_CheckpointJob:

      freebr_manage(&this->rq_ind.rq_manager);

      break;

    case PBS_BATCH_MessJob:

      if (this->rq_ind.rq_message.rq_text)
        {
        free(this->rq_ind.rq_message.rq_text);
        this->rq_ind.rq_message.rq_text = NULL;
        }

      break;

    case PBS_BATCH_ModifyJob:

    case PBS_BATCH_AsyModifyJob:

      freebr_manage(&this->rq_ind.rq_modify);

      break;

    case PBS_BATCH_StatusJob:

    case PBS_BATCH_StatusQue:

    case PBS_BATCH_StatusNode:

    case PBS_BATCH_StatusSvr:
      /* DIAGTODO: handle PBS_BATCH_StatusDiag */

      free_attrlist(&this->rq_ind.rq_status.rq_attr);

      break;

    case PBS_BATCH_JobObit:

      free_attrlist(&this->rq_ind.rq_jobobit.rq_attr);

      break;

    case PBS_BATCH_CopyFiles:

    case PBS_BATCH_DelFiles:

      freebr_cpyfile(&this->rq_ind.rq_cpyfile);

      break;

    case PBS_BATCH_ModifyNode:

    case PBS_BATCH_Manager:

      freebr_manage(&this->rq_ind.rq_manager);

      break;

    case PBS_BATCH_ReleaseJob:

      freebr_manage(&this->rq_ind.rq_release);

      break;

    case PBS_BATCH_Rescq:

      free_rescrq(&this->rq_ind.rq_rescq);

      break;

    case PBS_BATCH_SelectJobs:

    case PBS_BATCH_SelStat:

      free_attrlist(&this->rq_ind.rq_select);

      break;

    case PBS_BATCH_SelStatAttr:

      free_attrlist(&this->rq_ind.rq_select);
      free_attrlist(&this->rq_ind.rq_status.rq_attr);

      break;

    case PBS_BATCH_RunJob:
    case PBS_BATCH_AsyrunJob:

      if (this->rq_ind.rq_run.rq_destin)
        {
        free(this->rq_ind.rq_run.rq_destin);
        this->rq_ind.rq_run.rq_destin = NULL;
        }
      break;

    default:

      /* NO-OP */

      break;
    }  /* END switch (this->rq_type) */
  }
