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
* without reference to its choice of law rules
*/

#include <stdio.h>
#include <ctype.h>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

#include "pbs_config.h"
#include "pbs_nodes.h"
#include "svrfunc.h"
#include "log.h"
#include "threadpool.h"
#include "u_tree.h"
#include "server.h"
#include "dis.h"
#include "utils.h"
#include "ji_mutex.h"
#include "../lib/Libutils/u_lock_ctl.h"
#include "mutex_mgr.hpp"
#include "id_map.hpp"
#include "plugin_internal.h"
#include "mom_update_constants.hpp"
#include "numa_constants.h"


extern attribute_def    node_attr_def[];   /* node attributes defs */
extern int              allow_any_mom;
extern char             server_name[];


int is_gpustat_get(struct pbsnode *np, unsigned int &i, std::vector<std::string> &status_info);
void clear_nvidia_gpus(struct pbsnode *np);
int  gpu_entry_by_id(struct pbsnode *pnode, const char *gpuid, int get_empty);
int gpu_has_job(struct pbsnode *pnode, int gpuid);


void move_past_mic_status(

  unsigned int             &status_index,
  std::vector<std::string> &status_info)

  {
  while (status_index < status_info.size())
    {
    if (!strcmp(status_info[status_index].c_str(), END_MIC_STATUS))
      break;

    status_index++;
    }
  } /* END move_past_mic_status() */



int save_single_mic_status(

  std::string    &single_mic_status,
  pbs_attribute  *temp)

  {
  int rc = PBSE_NONE;

  if (single_mic_status.length() > 0)
    {
    if ((rc = decode_arst(temp, NULL, NULL, single_mic_status.c_str(), 0)) != PBSE_NONE)
      {
      log_err(ENOMEM, __func__, "");
      free_arst(temp);
      }
    }

  return(rc);
  } /* END save_single_mic_status() */




int process_mic_status(
    
  struct pbsnode           *pnode, 
  unsigned int             &status_index,
  std::vector<std::string> &status_info)

  {
  int             rc = PBSE_NONE;
  pbs_attribute   temp;
  int             mic_count = 0;
  std::string     single_mic_status = "";
  char            mic_id_buf[MAXLINE];
  
  memset(&temp, 0, sizeof(temp));
  if ((rc = decode_arst(&temp, NULL, NULL, NULL, 0)) != PBSE_NONE)
    {
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, "cannot initialize attribute");
    move_past_mic_status(status_index, status_info);
    return(rc);
    }

  for (status_index++; status_index < status_info.size(); status_index++)
    {
    const char *str = status_info[status_index].c_str();

    if (!strcmp(str, END_MIC_STATUS))
      break;

    if (!strncmp(str, "mic_id=", strlen("mic_id=")))
      {
      if ((rc = save_single_mic_status(single_mic_status, &temp)) != PBSE_NONE)
        break;

      single_mic_status.clear();

      snprintf(mic_id_buf, sizeof(mic_id_buf), "mic[%d]=%s", mic_count, str);
      single_mic_status += mic_id_buf;

      mic_count++;
      }
    else
      {
      single_mic_status += ';';
      single_mic_status += str;
      }
    }

  rc = save_single_mic_status(single_mic_status, &temp);

  if (mic_count > pnode->nd_nmics)
    {
    pnode->nd_nmics_free += mic_count - pnode->nd_nmics;
    pnode->nd_nmics = mic_count;

    if (mic_count > pnode->nd_nmics_alloced)
      {
      for (int i = 0; i < mic_count - pnode->nd_nmics_alloced; i++)
        pnode->nd_micjobids.push_back(-1);

      pnode->nd_nmics_alloced = mic_count;
      }
    }

  move_past_mic_status(status_index, status_info);
  
  node_micstatus_list(&temp, pnode, ATR_ACTION_ALTER);

  return(rc);
  } /* END process_mic_status() */




/*
 * switches the current node to the desired
 * numa subnode requested in str and unlocks np
 */

struct pbsnode *get_numa_from_str(
    
  const char     *str, /* I */
  struct pbsnode *np)  /* I */

  {
  const char     *numa_id;
  struct pbsnode *numa;
  unsigned long   numa_index;
  char            log_buf[LOCAL_LOG_BUF_SIZE];
  
  if (np->node_boards == NULL)
    {
    /* ERROR */
    snprintf(log_buf,sizeof(log_buf),
      "Node %s isn't declared to be NUMA, but mom is reporting\n",
      np->get_name());
    log_err(-1, __func__, log_buf);
  
    np->unlock_node(__func__, "np numa update", LOGLEVEL);
    
    return(NULL);
    }
  
  numa_id = str + strlen(NUMA_KEYWORD);
  numa_index = atoi(numa_id);
  
  numa = AVL_find(numa_index, np->nd_mom_port, np->node_boards);
  
  if (numa == NULL)
    {
    /* ERROR */
    snprintf(log_buf,sizeof(log_buf),
      "Could not find NUMA index %lu for node %s\n",
      numa_index,
      np->get_name());
    log_err(-1, __func__, log_buf);
    
    np->unlock_node(__func__, "np numa update", LOGLEVEL);
    
    return(NULL);
    }
 
  /* SUCCESS */
  np->unlock_node(__func__, "np numa update", LOGLEVEL);
  numa->lock_node(__func__, "numa numa update", LOGLEVEL);
  
  numa->nd_lastupdate = time(NULL);
  
  return(numa);
  } /* END get_numa_from_str() */



/* 
 * unlocks np and returns a the node specified in string, locked
 */

struct pbsnode *get_node_from_str(

  const char     *str,     /* I */
  const char     *orig_id, /* I */
  struct pbsnode *np)      /* M */

  {
  /* this is a node reporting on another node as well */
  const char     *node_id = str + strlen("node=");
  struct pbsnode *next = NULL;
  char            log_buf[LOCAL_LOG_BUF_SIZE];
 
  /* don't do anything if the name is the same as this node's name */
  if (strcmp(node_id, np->get_name()))
    {
    np->unlock_node(__func__, "np not numa update", LOGLEVEL);
    
    next = find_nodebyname(node_id);
    
    if (next == NULL)
      {
      /* NYI: should we add logic here to attempt the canonical name if this 
       * is the short name, and attempt the short name if this is the 
       * canonical name? */
      
      /* ERROR */
      snprintf(log_buf,sizeof(log_buf),
        "Node %s is reporting on node %s, which pbs_server doesn't know about\n",
        orig_id,
        node_id);
      log_err(-1, __func__, log_buf);
      }
    else
      {
      if (LOGLEVEL >= 7)
        {
        snprintf(log_buf,sizeof(log_buf),
          "Node %s is reporting for node %s\n",
          orig_id,
          node_id);
        
        log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buf);
        }
      
      next->nd_lastupdate = time(NULL);
      }
    }
  else
    {
    next = np;
    next->nd_lastupdate = time(NULL);
    }

  /* next may be NULL */

  return(next);
  } /* END get_node_from_str() */



/*
 * handle_auto_np()
 *
 * @param np - the node in question
 * @param str - number of cpus written as a string
 */

int handle_auto_np(

  pbsnode    *np,  /* M */
  const char *str) /* I */

  {
  pbs_attribute nattr;
  
  /* first we decode str into nattr... + 6 is because str has format
   * ncpus=X, and 6 = strlen(ncpus=) */  
  if ((node_attr_def + ND_ATR_np)->at_decode(&nattr, ATTR_NODE_np, NULL, str, 0) == 0)
    {
    /* ... and if MOM's ncpus is different than our np... */
    if (nattr.at_val.at_long != np->nd_slots.get_total_execution_slots())
      {
      /* ... then we do the defined magic to create new subnodes */
      (node_attr_def + ND_ATR_np)->at_action(&nattr, (void *)np, ATR_ACTION_ALTER);
      
      update_nodes_file(np);
      }
    }

  return(PBSE_NONE);
  } /* END handle_auto_np() */




/*
 * update_job_data() - update job with values passed through "jobdata"
 *
 * This function is called every time we get a "jobdata" status from the pbs_mom.
 *
 * @see is_stat_get()
 */

void update_job_data(

  struct pbsnode *np,            /* I */
  const char     *jobstring_in)  /* I (changed attributes sent by mom) */

  {
  char  *jobdata;
  char  *jobdata_ptr;
  char  *jobidstr;
  char  *attr_name;
  char  *attr_value;
  char   log_buf[LOCAL_LOG_BUF_SIZE];

  svr_job   *pjob = NULL;
  int    on_node = FALSE;

  if ((jobstring_in == NULL) || (!isdigit(*jobstring_in)))
    {
    /* NO-OP */

    return;
    }

  /* FORMAT <JOBID>:<atrtributename=value>,<atrtributename=value>... */

  jobdata = strdup(jobstring_in);
  jobdata_ptr = jobdata;

  jobidstr = threadsafe_tokenizer(&jobdata_ptr, ":");

  if ((jobidstr != NULL) && isdigit(*jobidstr))
    {
    if (strstr(jobidstr, server_name) != NULL)
      {
      on_node = is_job_on_node(np, job_mapper.get_id(jobidstr));
      pjob = svr_find_job(jobidstr, TRUE);

      if (pjob != NULL)
        {
        int bad;
        svrattrl tA;
        mutex_mgr job_mutex(pjob->ji_mutex, true);
        
        /* job exists, so get the attributes and update them */
        attr_name = threadsafe_tokenizer(&jobdata_ptr, "=");
        
        while (attr_name != NULL)
          {
          attr_value = threadsafe_tokenizer(&jobdata_ptr, ",");
          
          if (LOGLEVEL >= 9)
            {
            sprintf(log_buf, "Mom sent changed attribute %s value %s for job %s",
              attr_name,
              attr_value,
              pjob->get_jobid());
              
            log_event(PBSEVENT_JOB,PBS_EVENTCLASS_JOB,pjob->get_jobid(),log_buf);  
            }

          memset(&tA, 0, sizeof(tA));

          tA.al_name  = attr_name;
          tA.al_resc  = (char *)"";
          tA.al_value = attr_value;
          tA.al_op    = SET;

          modify_job_attr(
            pjob,
            &tA,                              /* I: ATTR_sched_hint - svrattrl */
            ATR_DFLAG_MGWR | ATR_DFLAG_SvWR,
            &bad);

          attr_name = threadsafe_tokenizer(&jobdata_ptr, "=");
          }
        }
      
      if (on_node == FALSE)
        {
        /* job is reported by mom but server has no record of job */
        sprintf(log_buf, "stray job %s reported on %s", jobidstr, np->get_name());

        log_err(-1, __func__, log_buf);
        }
      }
    }

  free(jobdata);
  }  /* END update_job_data() */



/*
 * set_note_error()
 *
 * Adds the error reported to this node's note
 *
 * @param np - the node who is getting this error message.
 * @param str - the error message
 * @return PBSE_NONE on success
 */

int set_note_error(

  pbsnode           *np,
  const std::string &str)

  {
  std::string message;
  std::string errmsg(str);
  std::string oldnote = "";

  // If a note already exists, append the error; otherwise, create a new note
  if (np->nd_note.size() != 0)
    {
    oldnote = np->nd_note;

    // If this evaluates to true, we know that the error message has already been added to the note
    if (oldnote.find(errmsg) != std::string::npos)
      {
      return(PBSE_NONE);
      }

    // append message
    message = oldnote + " - " + errmsg;
    }
  else
    message = errmsg;

  np->nd_note = message.c_str();

  return(PBSE_NONE);
  }  /* END set_note() */



/*
 * restore_note()
 * Attempts to remove the error note from this node
 *
 * @param np - the node whose error message we're potentially removing 
 */

int restore_note(

  struct pbsnode *np)

  {
  std::string message;
  std::string oldnote;
  std::size_t found;
  std::size_t found_appendage;

  // If a note exists, strip it of any error message
  if (np->nd_note.size() != 0)
    {
    oldnote = np->nd_note;

    found = oldnote.find("ERROR");

    if (found == std::string::npos)
      {
      return(PBSE_NONE);
      }

     found_appendage = oldnote.find(" - ERROR");

     // adjust the found index to the first of "ERROR" or " - ERROR"
     if ((found_appendage != std::string::npos) && (found_appendage < found))
       {
       found = found_appendage;
       }

    // If the note would be empty after removing the error message,
    // simply remove the entire note
    if (found == 0)
      {
      np->nd_note.clear();
      }
    else
      {
      message = oldnote.substr(0, found);
      np->nd_note = message;
      }
    }

  return(PBSE_NONE);
  }  /* END restore_note() */



int process_uname_str(

  struct pbsnode *np,
  const char     *str)

  {
  /* for any mom mode if an address did not succeed at getnameinfo it was
   * given the hex value of its ip address */
  if (!strncmp(np->get_name(), "0x", 2))
    {
    const char *cp;
    char  node_name[PBS_MAXHOSTNAME + 1];
    int   count;
    
    if((cp = strchr(str, ' ')) == NULL)
      return (PBSE_RMBADPARAM);
    count = 0;
    
    do
      {
      cp++;
      node_name[count] = *cp;
      count++;
      } while (*cp != ' ' && count < PBS_MAXHOSTNAME);
    
    node_name[count-1] = 0;
    
    np->change_name(node_name);
    }

  return(PBSE_NONE);
  } /* END process_uname_str() */



/*
 * process_state_str()
 *
 * Processes the state string that came from the mom
 * @retun PBSE_NONE on SUCCESS, PBSE_* on an error
 */

int process_state_str(

  pbsnode    *np,
  const char *str)

  {
  char            log_buf[LOCAL_LOG_BUF_SIZE];
  int             rc = PBSE_NONE;

  if (np->nd_state & INUSE_NOHIERARCHY)
    {
    sprintf(log_buf, "node %s has not received its hierarchy yet.",
      np->get_name());

    log_err(-1, __func__, log_buf);
    return(PBSE_HIERARCHY_NOT_SENT);
    }
  if (!strncmp(str, "down", 10))
    {
    update_node_state(np, INUSE_DOWN);
    }
  else if (!strncmp(str, "busy", 10))
    {
    if (np->nd_state == INUSE_DOWN)
      {
      restore_note(np);
      }

    update_node_state(np, INUSE_BUSY);
    }
  else if (!strncmp(str, "free", 10))
    {
    if (np->nd_state == INUSE_DOWN)
      {
      restore_note(np);
      }

    update_node_state(np, INUSE_FREE);
    }
  else
    {
    sprintf(log_buf, "unknown %s from node %s",
      str,
      np->get_name());
    
    log_err(-1, __func__, log_buf);
    
    update_node_state(np, INUSE_UNKNOWN);
    }
  
  if (LOGLEVEL >= 9)
    {
    sprintf(log_buf, "node '%s' is at state '0x%x'\n",
      np->get_name(),
      np->nd_state);
    
    log_event(PBSEVENT_ADMIN, PBS_EVENTCLASS_SERVER, __func__, log_buf);
    }
  
  return(rc);
  } /* END process_state_str() */



int update_node_mac_addr(

  pbsnode    *np,
  const char *str)

  {
  unsigned char macaddr[6];
  for(int i = 0;i < 6;i++)
    {
    unsigned char upperNibble = 0;
    if((*str >= 'A')&&(*str <= 'F')) upperNibble = (unsigned char)((*str - 'A' + 0x0a));
    else if((*str >= 'a')&&(*str <= 'f')) upperNibble = (unsigned char)((*str - 'a' + 0x0a));
    else if((*str >= '0')&&(*str <= '9')) upperNibble = (unsigned char)((*str - '0'));
    else return -1;
    str++;
    unsigned char lowerNibble = 0;
    if((*str >= 'A')&&(*str <= 'F')) lowerNibble = (unsigned char)((*str - 'A' + 0x0a));
    else if((*str >= 'a')&&(*str <= 'f')) lowerNibble = (unsigned char)((*str - 'a' + 0x0a));
    else if((*str >= '0')&&(*str <= '9')) lowerNibble = (unsigned char)((*str - '0'));
    else return -1;
    macaddr[i] = ((upperNibble << 4) + lowerNibble);
    str++;
    if((*str != ':')&&(*str != '\0')) return -1;
    str++;
    }
  memcpy(np->nd_mac_addr,macaddr,6);
  return 0;
  }




int save_node_status(

  struct pbsnode *np,
  std::string    &new_status)

  {
  int  rc = PBSE_NONE;
  char date_attrib[MAXLINE];

  /* it's nice to know when the last update happened */
  snprintf(date_attrib, sizeof(date_attrib), "rectime=%ld", (long)time(NULL));

  new_status += ",";
  new_status += date_attrib;

  np->nd_status = new_status;

  return(rc);
  } /* END save_node_status() */



#ifdef PENABLE_LINUX_CGROUPS
/*
 * update_layout_if_needed()
 *
 * Updates the layout of pnode if 1) we don't have one or 2) the thread count has changed
 *
 * @param pnode - the node in question
 * @param layout - the string specifying the layout
 */

void update_layout_if_needed(

  pbsnode           *pnode,
  const std::string &layout,
  Json::Value       *jv,
  bool               force)

  {
  char                     log_buf[LOCAL_LOG_BUF_SIZE];
  std::vector<std::string> valid_ids;

  if ((pnode->nd_layout.is_initialized() == false) ||
      (force == true))
    {
    for (size_t i = 0; i < pnode->nd_job_usages.size(); i++)
      {
      const char *id = job_mapper.get_name(pnode->nd_job_usages[i].internal_job_id);
      
      if (id != NULL)
        valid_ids.push_back(id);
      }

    if (jv == NULL)
      {
      Machine m(layout, valid_ids);

      if (pnode->nd_layout.is_initialized() == true)
        m.save_allocations(pnode->nd_layout);

      pnode->nd_layout = m;
      }
    else
      {
      Machine m(*jv, valid_ids);
      
      if (pnode->nd_layout.is_initialized() == true)
        m.save_allocations(pnode->nd_layout);

      pnode->nd_layout = m;
      }
    }
  else if (((pnode->nd_layout.getTotalThreads() != pnode->nd_slots.get_total_execution_slots()) ||
            (pnode->nd_ngpus != pnode->nd_layout.get_total_gpus())) &&
           (pnode->nd_job_usages.size() == 0))
    {
    int old_count = pnode->nd_layout.getTotalThreads();
    int new_count = pnode->nd_slots.get_total_execution_slots();

    // If the number of np for the node has changed, then we should get a new layout as long
    // as we don't have active jobs
    for (size_t i = 0; i < pnode->nd_job_usages.size(); i++)
      {
      const char *id = job_mapper.get_name(pnode->nd_job_usages[i].internal_job_id);

      if (id != NULL)
        valid_ids.push_back(id);
      }
    
    if (jv == NULL)
      {
      Machine m(layout, valid_ids);
      pnode->nd_layout = m;
      }
    else
      {
      Machine m(*jv, valid_ids);
      pnode->nd_layout = m;
      }

    if (LOGLEVEL >= 3)
      {
      snprintf(log_buf, sizeof(log_buf),
        "Node %s appears to have had it's core/thread count updated. The layout has now also been updated from %d to %d.",
        pnode->get_name(), old_count, new_count);

      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, log_buf);
      }
    }
  } // END update_layout_if_needed()
#endif



/*
 * process_gpu_mode_string()
 *
 * @param current - the node where the gpu resides
 * @param mode_str - a string specifying the mode
 * @param gpuidx - the id of the gpu
 */

void process_gpu_mode_string(
    
  pbsnode           *current,
  const std::string &mode_str,
  int                gpuidx)

  {
  char log_buf[LOCAL_LOG_BUF_SIZE];

  if ((mode_str == "Normal") ||
      (mode_str == "Default"))
    {
    current->nd_gpusn[gpuidx].mode = gpu_normal;
    if (gpu_has_job(current, gpuidx))
      {
      current->nd_gpusn[gpuidx].state = gpu_shared;
      }
    else
      {
      current->nd_gpusn[gpuidx].inuse = 0;
      current->nd_gpusn[gpuidx].state = gpu_unallocated;
      }
    }
  else if ((mode_str == "Exclusive") ||
           (mode_str == "Exclusive_Thread"))
    {
    current->nd_gpusn[gpuidx].mode = gpu_exclusive_thread;
    if (gpu_has_job(current, gpuidx))
      {
      current->nd_gpusn[gpuidx].state = gpu_exclusive;
      }
    else
      {
      current->nd_gpusn[gpuidx].inuse = 0;
      current->nd_gpusn[gpuidx].state = gpu_unallocated;
      }
    }
  else if (mode_str == "Exclusive_Process")
    {
    current->nd_gpusn[gpuidx].mode = gpu_exclusive_process;
    if (gpu_has_job(current, gpuidx))
      {
      current->nd_gpusn[gpuidx].state = gpu_exclusive;
      }
    else
      {
      current->nd_gpusn[gpuidx].inuse = 0;
      current->nd_gpusn[gpuidx].state = gpu_unallocated;
      }
    }
  else if (mode_str == "Prohibited")
    {
    current->nd_gpusn[gpuidx].mode = gpu_prohibited;
    current->nd_gpusn[gpuidx].state = gpu_unavailable;
    }
  else
    {
    /* unknown mode, default to prohibited */
    current->nd_gpusn[gpuidx].mode = gpu_prohibited;
    current->nd_gpusn[gpuidx].state = gpu_unavailable;
    if (LOGLEVEL >= 3)
      {
      sprintf(log_buf,
        "GPU %d has unknown mode on node %s",
        gpuidx,
        current->get_name());

      log_ext(-1, __func__, log_buf, LOG_DEBUG);
      }
    }
  } // END process_gpu_mode_string()



/*
 * process_gpu_information()
 *
 */

int process_gpu_information(
    
  pbsnode           *current,
  Json::Value       &gpu_info,
  std::string       &status_line,
  const std::string &driver_version)

  {
  std::stringstream gpu_status;
  int               gpuidx = -1;
  char              log_buf[LOCAL_LOG_BUF_SIZE];

  if (gpu_info[GPU_ID].empty() == false)
    {
    /*
     * Get this gpus index, if it does not yet exist then find an empty entry.
     * We need to allow for the gpu status results being returned in
     * different orders since the nvidia order may change upon mom's reboot
     */
    const char *gpu_id_str = gpu_info[GPU_ID].asString().c_str();
    gpuidx = gpu_entry_by_id(current, gpu_id_str, TRUE);
      
    if (gpuidx == -1)
      {
      // Error
      if (LOGLEVEL >= 3)
        {
        sprintf(log_buf, "Failed to get/create entry for gpu %s on node %s\n",
          gpu_id_str, current->get_name());
        log_ext(-1, __func__, log_buf, LOG_DEBUG);
        }

      return(-1);
      }

    gpu_status << "gpu[" << gpuidx << "]=gpu_id=" << gpu_id_str << ";";
    current->nd_gpusn[gpuidx].driver_ver = atoi(driver_version.c_str());
    current->nd_gpus_real = TRUE;
    if (current->nd_gpusn[gpuidx].gpuid.size() == 0)
      current->nd_gpusn[gpuidx].gpuid = gpu_id_str;
    }
  else
    {
    return(-1);
    }

  if (gpu_info[GPU_MODE].empty() == false)
    {
    process_gpu_mode_string(current, gpu_info[GPU_MODE].asString(), gpuidx);

    switch (current->nd_gpusn[gpuidx].state)
      {
      case gpu_unallocated:

        gpu_status << ";gpu_state=Unallocated";
        break;

      case gpu_shared:

        gpu_status << ";gpu_state=Shared";
        break;

      case gpu_exclusive:

        gpu_status << "gpu_state=Exclusive";
        break;

      case gpu_unavailable:

        gpu_status << "gpu_state=Unavailable";
        break;
      }
    }

  std::vector<std::string> keys = gpu_info.getMemberNames();

  for (size_t i = 0; i < keys.size(); i++)
    {
    if (keys[i] == GPU_TEMPERATURE)
      {
      gpu_status << ";" << keys[i] << "=" << gpu_info[keys[i]].asInt();
      }
    else if ((keys[i] != GPU_MODE) &&
             (keys[i] != GPU_ID))
      gpu_status << ";" << keys[i] << "="  << gpu_info[keys[i]].asString();
    }

  if (status_line.size() != 0)
    status_line += ";";
  status_line += gpu_status.str();

  current->nd_gpustatus = status_line;
  
  return(PBSE_NONE);
  } // END process_gpu_information()



int process_mic_information(

  pbsnode     *current,
  Json::Value &mic_status,
  int          mic_index,
  std::string &mic_status_line)

  {
  int         rc = PBSE_NONE;
  std::string single_mic_status;
  char        mic_id_buf[MAXLINE];

  std::vector<std::string> keys = mic_status.getMemberNames();
  
  for (size_t i = 0; i < keys.size(); i++)
    {
    if (keys[i] == MIC_ID)
      {
      snprintf(mic_id_buf, sizeof(mic_id_buf),
        "mic[%d]=%s=%s", mic_index, keys[i].c_str(), mic_status[keys[i]].asString().c_str());
      single_mic_status += mic_id_buf;
      }
    else if ((keys[i] == NUM_CORES) ||
             (keys[i] == NUM_THREADS))
      {
      snprintf(mic_id_buf, sizeof(mic_id_buf),
        "%s=%d", keys[i].c_str(), mic_status[keys[i]].asInt());
      single_mic_status += ";";
      single_mic_status += mic_id_buf;
      }
    else
      {
      single_mic_status += ';';
      single_mic_status += keys[i];
      single_mic_status += '=';
      single_mic_status += mic_status[keys[i]].asString();
      }
    }

  mic_status_line += single_mic_status;

  return(rc);
  } // END process_mic_information()



void remove_newlines(

  const std::string &line,
  std::string       &no_newlines,
  bool              &changed)

  {
  no_newlines = line;

  size_t pos = no_newlines.find('\n');
  changed = false;

  while (pos != std::string::npos)
    {
    no_newlines.replace(pos, 1, 1, ' ');
    pos = no_newlines.find('\n');
    changed = true;
    }
  } // END remove_newlines()



/*
 * process_node_json_status()
 *
 */

int process_node_json_status(

  Json::Value &node_status,
  bool         mom_job_sync,
  bool         auto_np,
  bool         down_on_error,
  bool         note_append_on_error)

  {
  std::vector<std::string> node_attrs = node_status.getMemberNames();
  std::string              status_line;
  bool                     force_layout_update = !node_status["force_layout_update"].empty();
  bool                     dont_change_state = false;
  bool                     send_hello = false;
  pbsnode                 *current = NULL;
  char                     log_buf[LOCAL_LOG_BUF_SIZE];
  const char              *node_name = NULL;
  int                      rc = PBSE_NONE;

  if (node_status[NAME].empty() == false)
    {
    node_name = node_status[NAME].asString().c_str();
    current = find_nodebyname(node_name);
    }

  if (current == NULL)
    {
    if (node_name == NULL)
      snprintf(log_buf, sizeof(log_buf), "Status update came with no node name");
    else
      snprintf(log_buf, sizeof(log_buf), "Couldn't find a node named %s", node_name);

    log_err(PBSE_UNKNODE, __func__, log_buf);
    return(PBSE_UNKNODE);
    }
      
  if (node_status[MESSAGE].empty() == false)
    {
    bool        changed = false;
    std::string no_newlines;

    remove_newlines(node_status[MESSAGE].asString(), no_newlines, changed);

    if (changed)
      node_status[MESSAGE] = no_newlines;

    if (down_on_error == true)
      {
      update_node_state(current, INUSE_DOWN);
      dont_change_state = true;

      if (note_append_on_error == true)
        {
        set_note_error(current, no_newlines);
        }
      }
    }

  // Build the status string
  for (size_t i = 0; i < node_attrs.size(); i++)
    {
    std::string &attr_name = node_attrs[i];
    if ((attr_name != "force_layout_update") &&
        (attr_name != GPUS) &&
        (attr_name != MICS) &&
        (attr_name != LAYOUT) &&
        (attr_name != NAME) &&
        (attr_name != PLUGIN_RESOURCES) &&
        (attr_name != NUMA_INDEX) &&
        (attr_name != "first_update"))
      {
      if (status_line.size() > 0)
        status_line += ",";

      status_line += attr_name + "=" + node_status[attr_name].asString();
      }
    }

  if (node_status[LAYOUT].empty() == false)
    {
    update_layout_if_needed(current, "", &node_status[LAYOUT], force_layout_update);
    }
  
  if (node_status["first_update"].empty() == false)
    {
    send_hello = true;
    
    /* reset gpu data in case mom reconnects with changed gpus */
    clear_nvidia_gpus(current);
    }

  if ((node_status[STATE].empty() == false) &&
      (dont_change_state == false))
    process_state_str(current, node_status[STATE].asString().c_str());

  if ((allow_any_mom == TRUE) &&
      (node_status[UNAME].empty() == false) &&
      (node_status[UNAME].asString().find("uname") == 0)) 
    process_uname_str(current, node_status[UNAME].asString().c_str());

  if (node_status[MACADDR].empty() == false)
    update_node_mac_addr(current, node_status[MACADDR].asString().c_str());

  if ((mom_job_sync == true) &&
      (node_status[JOBDATA].empty() == false))
    update_job_data(current, node_status[JOBDATA].asString().c_str());

  if ((auto_np) &&
      (node_status[NCPUS].empty() == false))
    handle_auto_np(current, node_status[NCPUS].asString().c_str());

  if (node_status[VERSION].empty() == false)
    current->set_version(node_status[VERSION].asString().c_str());

  if ((node_status[GPUS].empty() == false) &&
      (node_status[GPUS][GPU_STR].isArray()))
    {
    int          size = node_status[GPUS][GPU_STR].size();
    int          reported_gpu_count = 0;
    int          original_gpu_count = current->nd_ngpus;
    std::string  gpu_status_line;
    std::string  driver_version;
    
    if (node_status[GPUS][DRIVER_VERSION].empty() == false)
      driver_version = node_status[GPUS][DRIVER_VERSION].asString();

    for (int i = 0; i < size; i++)
      {
      if (process_gpu_information(current, node_status[GPUS][GPU_STR][i], gpu_status_line,
            driver_version) == PBSE_NONE)
        reported_gpu_count++;

      if (reported_gpu_count != original_gpu_count)
        {
        current->nd_ngpus = reported_gpu_count;

        /* update the nodes file */
        update_nodes_file(current);
        }
      }
    
    if (node_status[GPUS][TIMESTAMP].empty() == false)
      {
      gpu_status_line += TIMESTAMP;
      gpu_status_line += "=";
      gpu_status_line += node_status[GPUS][TIMESTAMP].asString();
      }

    gpu_status_line += DRIVER_VERSION;
    gpu_status_line += "=";
    gpu_status_line += driver_version;
    } 

  if ((node_status[MICS].empty() == false) &&
      (node_status[MIC_STR].isArray()))
    {
    int         size = node_status[MICS][MIC_STR].size();
    std::string mic_status_line;
      
    if (size > current->nd_nmics)
      {
      current->nd_nmics_free += size - current->nd_nmics;
      current->nd_nmics = size;

      if (size > current->nd_nmics_alloced)
        {
        for (int i = 0; i < size - current->nd_nmics_alloced; i++)
          current->nd_micjobids.push_back(-1);

        current->nd_nmics_alloced = size;
        }
      }

    for (int i = 0; i < size; i++)
      process_mic_information(current, node_status[MICS][MIC_STR][i], i, mic_status_line);
    }

  if (node_status[PLUGIN_RESOURCES].empty() == false)
    current->capture_plugin_resources(node_status[PLUGIN_RESOURCES]);

  if (node_status[JOBS].empty() == false)
    {
    sync_job_info *sji = new sync_job_info();
    sji->node_name = current->get_name();
    sji->sync_jobs = mom_job_sync;
      
    // sji is freed in sync_node_jobs()
    enqueue_threadpool_request(sync_node_jobs, sji, task_pool);
    }
  /* sji->job_info = str + 5; */
        
  save_node_status(current, status_line);

  current->unlock_node(__func__, NULL, LOGLEVEL);

  if (send_hello == true)
    rc = SEND_HELLO;
  
  return(rc);
  } // END process_node_json_status()



/*
 * process_status_info()
 *
 * @param nd_name - the name of the node we're 
 */

int process_status_info(

  const char  *nd_name,
  Json::Value &status_update)

  {
  bool                     mom_job_sync = true;
  bool                     auto_np = false;
  bool                     down_on_error = false;
  bool                     note_append_on_error = false;
  int                      rc = PBSE_NONE;

  get_svr_attr_b(SRV_ATR_MomJobSync, &mom_job_sync);
  get_svr_attr_b(SRV_ATR_AutoNodeNP, &auto_np);
  get_svr_attr_b(SRV_ATR_NoteAppendOnError, &note_append_on_error);
  get_svr_attr_b(SRV_ATR_DownOnError, &down_on_error);

  if ((status_update[NODE].empty() == false) &&
      (status_update[NODE].isArray()))
    {
    int size = status_update[NODE].size();

    for (int i = 0; i < size; i++)
      rc = process_node_json_status(status_update[NODE][i], mom_job_sync, auto_np, down_on_error,
                                    note_append_on_error);
    }

  return(rc);
  } // END process_status_info()



/*
 * process_status_info()
 *
 * @param nd_name - the name of the node who sent this update
 * @param status_info - a list of each status string sent in
 * @return PBSE_NONE on SUCCESS, PBSE_* on error
 */

int process_status_info(

  const char               *nd_name,
  std::vector<std::string> &status_info)

  {
  const char     *name = nd_name;
  pbsnode        *current;
  bool            mom_job_sync = true;
  bool            auto_np = false;
  bool            down_on_error = false;
  bool            note_append_on_error = false;
  bool            dont_change_state = false;
  int             rc = PBSE_NONE;
  bool            send_hello = false;
  std::string     temp;
#ifdef PENABLE_LINUX_CGROUPS
  bool            force_layout_update = false;
#endif

  get_svr_attr_b(SRV_ATR_MomJobSync, &mom_job_sync);
  get_svr_attr_b(SRV_ATR_AutoNodeNP, &auto_np);
  get_svr_attr_b(SRV_ATR_NoteAppendOnError, &note_append_on_error);
  get_svr_attr_b(SRV_ATR_DownOnError, &down_on_error);

  /* if original node cannot be found do not process the update */
  if ((current = find_nodebyname(nd_name)) == NULL)
    return(PBSE_NONE);

  //A node we put to sleep is up and running.
  if (current->nd_power_state != POWER_STATE_RUNNING)
    {
    //Make sure we wait for a stray update that came after we changed the state to pass
    //by.
    if ((current->nd_power_state_change_time + NODE_POWER_CHANGE_TIMEOUT) < time(NULL))
      {
      current->nd_power_state = POWER_STATE_RUNNING;
      write_node_power_state();
      }
    }

  /* loop over each string */
  for (unsigned int i = 0; i != status_info.size(); i++)
    {
    const char *str = status_info[i].c_str();

    /* these two options are for switching nodes */
    if (!strncmp(str, NUMA_KEYWORD, strlen(NUMA_KEYWORD)))
      {
      // If we've already processed some, save this before moving on
      if (i != 0)
        {
        save_node_status(current, temp);
        temp.clear();
        }

      dont_change_state = false;

      if ((current = get_numa_from_str(str, current)) == NULL)
        break;
      else
        continue;
      }
    else if (!strncmp(str, "node=", strlen("node=")))
      {
      /* if we've already processed some, save this before moving on */
      if (i != 0)
        {
        save_node_status(current, temp);
        temp.clear();
        }

      dont_change_state = false;

      if ((current = get_node_from_str(str, name, current)) == NULL)
        break;
      else
        {
        if (current->nd_mom_reported_down == TRUE)
          {
          /* There is a race condition if using a mom hierarchy and manually
           * shutting down a non-level 1 mom: if its message that the mom is
           * shutting down gets there before its last status update, the node
           * can incorrectly be set as free again. For that reason, only set
           * a mom back up if its reporting for itself. */
          if (strcmp(name, str + strlen("node=")) != 0)
            dont_change_state = true;
          else
            current->nd_mom_reported_down = FALSE;
          }

        continue;
        }
      }

    /* add the info to the "temp" pbs_attribute */
    else if (!strcmp(str, START_GPU_STATUS))
      {
      is_gpustat_get(current, i, status_info);
      continue;
      }
    else if (!strcmp(str, START_MIC_STATUS))
      {
      process_mic_status(current, i, status_info);
      continue;
      }
#ifdef PENABLE_LINUX_CGROUPS
    else if (!strcmp(str, "force_layout_update"))
      {
      force_layout_update = true;
      continue;
      }
    else if (!strncmp(str, "layout", 6))
      {
      // Add 7 to skip "layout="
      update_layout_if_needed(current, str + 7, NULL, force_layout_update);

      // reset this to false in case we have a mom hierarchy in place
      force_layout_update = false;

      continue;
      }
#endif
    else if (!strncmp(str, PLUGIN_EQUALS, PLUGIN_EQ_LEN))
      {
      current->capture_plugin_resources(str + PLUGIN_EQ_LEN);
      continue;
      }
    else if (!strncmp(str, "jobs=", 5))
      {
      /* walk job list reported by mom */
      sync_job_info *sji = new sync_job_info();
      sji->node_name = current->get_name();
      sji->job_info = str + 5;
      sji->sync_jobs = mom_job_sync;

      // sji is freed in sync_node_jobs()
      enqueue_threadpool_request(sync_node_jobs, sji, task_pool);

      continue;
      }
    else if (!strcmp(str, "first_update=true"))
      {
      /* mom is requesting that we send the mom hierarchy file to her */
      //remove_hello(&hellos, current->nd_id);
      send_hello = true;
      
      /* reset gpu data in case mom reconnects with changed gpus */
      clear_nvidia_gpus(current);

      continue;
      }
    else 
      {
      // Save this string to our status line.
      if (temp.size() > 0)
        temp += ",";

      if (!strncmp(str, "message=", 8))
        {
        std::string no_newlines(str);
        size_t      pos = no_newlines.find('\n');
        
        while (pos != std::string::npos)
          {
          no_newlines.replace(pos, 1, 1, ' ');
          pos = no_newlines.find('\n');
          }

        temp += no_newlines;
        }
      else
        temp += str;
    
      if (!strncmp(str, "state", 5))
        {
        if (dont_change_state == false)
          process_state_str(current, str + 6); // +6 moves past 'state='
        }
      else if ((allow_any_mom == TRUE) &&
               (!strncmp(str, "uname", 5))) 
        {
        process_uname_str(current, str);
        }
      else if (!strncmp(str, "me", 2))  /* shorter str compare than "message" */
        {
        if ((!strncmp(str, "message=ERROR", 13)) &&
            (down_on_error == TRUE))
          {
          std::string no_newlines;
          bool        changed;

          update_node_state(current, INUSE_DOWN);
          dont_change_state = true;

          remove_newlines(str + 8, no_newlines, changed);

          if (note_append_on_error == true)
            {
            set_note_error(current, no_newlines);
            }
          }
        }
      else if (!strncmp(str,"macaddr=",8))
        {
        update_node_mac_addr(current,str + 8);
        }
      else if ((mom_job_sync == true) &&
               (!strncmp(str, "jobdata=", 8)))
        {
        /* update job attributes based on what the MOM gives us */      
        update_job_data(current, str + strlen("jobdata="));
        }
      else if ((auto_np) &&
               (!(strncmp(str, "ncpus=", 6))))

        {
        handle_auto_np(current, str);
        }
      else if (!strncmp(str, "version=", 8))
        {
        current->set_version(str + 8);
        }
      }

    } /* END processing strings */

  if (current != NULL)
    {
    save_node_status(current, temp);
    current->unlock_node(__func__, NULL, LOGLEVEL);
    }
  
  if ((rc == PBSE_NONE) &&
      (send_hello == true))
    rc = SEND_HELLO;
    
  return(rc);
  } /* END process_status_info() */



/*
 * move_past_gpu_status()
 *
 * @param i - the current index of which string we're processing in the list
 * @param status_info - the list of status strings
 */

void move_past_gpu_status(

  unsigned int             &i,
  std::vector<std::string> &status_info)

  {
  while (i < status_info.size())
    {
    if (!strcmp(status_info[i].c_str(), END_GPU_STATUS))
      break;

    i++;
    }
  } /* END move_past_gpu_status() */




/*
 * Function to process gpu status messages received from the mom
 */

int is_gpustat_get(

  struct pbsnode           *np,      /* I (modified) */
  unsigned int             &i,
  std::vector<std::string> &status_info)

  {
  pbs_attribute      temp;
  const char        *gpuid = NULL;
  char               log_buf[LOCAL_LOG_BUF_SIZE];
  int                gpuidx = -1;
  std::stringstream  gpuinfo;
  int                need_delimiter = FALSE;
  int                reportedgpucnt = 0;
  int                startgpucnt = 0;
  int                drv_ver = 0;

  if (np == NULL)
    {
    sprintf(log_buf, "Invalid parameter for np  passed to is_gpustat_get");
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buf);
    return(PBSE_BAD_PARAMETER);
    }

  if (LOGLEVEL >= 7)
    {
    sprintf(log_buf, "received gpu status from node %s", np->get_name());

    log_record(PBSEVENT_SCHED, PBS_EVENTCLASS_REQUEST, __func__, log_buf);
    }

  /* save current gpu count for node */
  startgpucnt = np->nd_ngpus;

  /*
   *  Before filling the "temp" pbs_attribute, initialize it.
   *  The second and third parameter to decode_arst are never
   *  used, so just leave them empty. (GBS)
   */

  memset(&temp, 0, sizeof(temp));

  if (decode_arst(&temp, NULL, NULL, NULL, 0))
    {
    DBPRT(("is_gpustat_get:  cannot initialize attribute\n"));

    return(DIS_NOCOMMIT);
    }

  i++;

  for (; i < status_info.size(); i++)
    {
    /* add the info to the "temp" attribute */
    const char *str = status_info[i].c_str();

    /* get timestamp */
    if (!strncmp(str, "timestamp=", 10))
      {
      if (decode_arst(&temp, NULL, NULL, str, 0))
        {
        DBPRT(("is_gpustat_get: cannot add attributes\n"));

        free_arst(&temp);
        move_past_gpu_status(i, status_info);

        return(DIS_NOCOMMIT);
        }
      continue;
      }

    /* get driver version, if there is one */
    if (!strncmp(str, "driver_ver=", 11))
      {
      if (decode_arst(&temp, NULL, NULL, str, 0))
        {
        DBPRT(("is_gpustat_get: cannot add attributes\n"));

        free_arst(&temp);
        move_past_gpu_status(i, status_info);

        return(DIS_NOCOMMIT);
        }
      drv_ver = atoi(str + 11);
      continue;
      }
    else if (!strcmp(str, END_GPU_STATUS))
      {
      break;
      }

    /* gpuid must come before the rest or we will be in trouble */

    if (!strncmp(str, "gpuid=", 6))
      {
      if (gpuinfo.str().size() > 0)
        {
        if (decode_arst(&temp, NULL, NULL, gpuinfo.str().c_str(), 0))
          {
          DBPRT(("is_gpustat_get: cannot add attributes\n"));

          free_arst(&temp);
          move_past_gpu_status(i, status_info);

          return(DIS_NOCOMMIT);
          }

        gpuinfo.str("");
        }

      gpuid = &str[6];

      /*
       * Get this gpus index, if it does not yet exist then find an empty entry.
       * We need to allow for the gpu status results being returned in
       * different orders since the nvidia order may change upon mom's reboot
       */

      gpuidx = gpu_entry_by_id(np, gpuid, TRUE);
      if (gpuidx == -1)
        {
        /*
         * Failure - we could not get / create a nd_gpusn entry for this gpu,
         * log an error message.
         */

        if (LOGLEVEL >= 3)
          {
          sprintf(log_buf,
            "Failed to get/create entry for gpu %s on node %s\n",
            gpuid,
            np->get_name());

          log_ext(-1, __func__, log_buf, LOG_DEBUG);
          }

        free_arst(&temp);
        move_past_gpu_status(i, status_info);

        return(DIS_SUCCESS);
        }

      gpuinfo << "gpu[" << gpuidx << "]=gpu_id=" << gpuid << ";";
      need_delimiter = FALSE;
      reportedgpucnt++;
      np->nd_gpusn[gpuidx].driver_ver = drv_ver;

      /* mark that this gpu node is not virtual */
      np->nd_gpus_real = TRUE;
      
      /*
       * if we have not filled in the gpu_id returned by the mom node
       * then fill it in
       */
      if ((gpuidx >= 0) &&
          (np->nd_gpusn[gpuidx].gpuid.size() == 0))
        {
        np->nd_gpusn[gpuidx].gpuid = gpuid;
        }      

      }
    else
      {
      if (need_delimiter)
        {
        gpuinfo << ";";
        }
     
      gpuinfo << str;
      
      need_delimiter = TRUE;
      }

    /* check current gpu mode and determine gpu state */
    
    if (!memcmp(str, "gpu_mode=", 9))
      {
      process_gpu_mode_string(np, str + 9, gpuidx);
 
      /* add gpu_mode so it gets added to the pbs_attribute */

      if (need_delimiter)
        {
        gpuinfo << ";";
        }

      switch (np->nd_gpusn[gpuidx].state)
        {
        case gpu_unallocated:

          gpuinfo << "gpu_state=Unallocated";
          break;

        case gpu_shared:

          gpuinfo << "gpu_state=Shared";
          break;

        case gpu_exclusive:

          gpuinfo << "gpu_state=Exclusive";
          break;

        case gpu_unavailable:

          gpuinfo << "gpu_state=Unavailable";
          break;
        }
      }

    } /* end of while disrst */

  if (gpuinfo.str().size() > 0)
    {
    if (decode_arst(&temp, NULL, NULL, gpuinfo.str().c_str(), 0))
      {
      DBPRT(("is_gpustat_get: cannot add attributes\n"));
      
      free_arst(&temp);
      move_past_gpu_status(i, status_info);

      return(DIS_NOCOMMIT);
      }
    }

  /* maintain the gpu count, if it has changed we need to update the nodes file */

  if (reportedgpucnt != startgpucnt)
    {
    np->nd_ngpus = reportedgpucnt;

    /* update the nodes file */
    update_nodes_file(np);
    }

  node_gpustatus_list(&temp, np, ATR_ACTION_ALTER);
  move_past_gpu_status(i, status_info);

  return(DIS_SUCCESS);
  }  /* END is_gpustat_get() */



/*
** Start a standard inter-server message.
*/

int is_compose(

  struct tcp_chan *chan,  /* I */
  int command) /* I */

  {
  int ret;

  ret = diswsi(chan, IS_PROTOCOL);

  if (ret != DIS_SUCCESS)
    goto done;

  ret = diswsi(chan, IS_PROTOCOL_VER);

  if (ret != DIS_SUCCESS)
    goto done;

  ret = diswsi(chan, command);

  if (ret != DIS_SUCCESS)
    goto done;

  return(DIS_SUCCESS);

done:

  DBPRT(("is_compose: send error %s\n",
         dis_emsg[ret]))

  return(ret);
  }  /* END is_compose() */
