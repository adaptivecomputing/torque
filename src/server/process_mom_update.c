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


extern attribute_def    node_attr_def[];   /* node attributes defs */
extern int              allow_any_mom;
extern char             server_name[];


int is_gpustat_get(struct pbsnode *np, unsigned int &i, std::vector<std::string> &status_info);
void clear_nvidia_gpus(struct pbsnode *np);
int  gpu_entry_by_id(struct pbsnode *pnode, const char *gpuid, int get_empty);
int gpu_has_job(struct pbsnode *pnode, int gpuid);


void move_past_mic_status(

  unsigned int             &i,
  std::vector<std::string> &status_info)

  {
  while (i < status_info.size())
    {
    if (!strcmp(status_info[i].c_str(), END_MIC_STATUS))
      break;

    i++;
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
  unsigned int             &i,
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
    move_past_mic_status(i, status_info);
    return(rc);
    }

  for (i++; i < status_info.size(); i++)
    {
    const char *str = status_info[i].c_str();

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
      struct jobinfo *tmp = (struct jobinfo *)calloc(mic_count, sizeof(struct jobinfo));
      
      if (tmp == NULL)
        return(ENOMEM);

      memcpy(tmp, pnode->nd_micjobs, sizeof(struct jobinfo) * pnode->nd_nmics_alloced);
      free(pnode->nd_micjobs);
      pnode->nd_micjobs = tmp;
          
      for (int j = pnode->nd_nmics_alloced; j < mic_count; j++)
        pnode->nd_micjobs[j].internal_job_id = -1;

      pnode->nd_nmics_alloced = mic_count;
      }
    }

  move_past_mic_status(i, status_info);
  
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
      np->nd_name);
    log_err(-1, __func__, log_buf);
  
    unlock_node(np, __func__, "np numa update", LOGLEVEL);
    
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
      np->nd_name);
    log_err(-1, __func__, log_buf);
    
    unlock_node(np, __func__, "np numa update", LOGLEVEL);
    
    return(NULL);
    }
 
  /* SUCCESS */
  unlock_node(np, __func__, "np numa update", LOGLEVEL);
  lock_node(numa, __func__, "numa numa update", LOGLEVEL);
  
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
  if (strcmp(node_id, np->nd_name))
    {
    unlock_node(np, __func__, "np not numa update", LOGLEVEL);
    
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




int handle_auto_np(

  struct pbsnode *np,  /* M */
  const char     *str) /* I */

  {
  pbs_attribute nattr;
  
  /* first we decode str into nattr... + 6 is because str has format
   * ncpus=X, and 6 = strlen(ncpus=) */  
  if ((node_attr_def + ND_ATR_np)->at_decode(&nattr, ATTR_NODE_np, NULL, str + 6, 0) == 0)
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

  job   *pjob = NULL;
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
              pjob->ji_qs.ji_jobid);
              
            log_event(PBSEVENT_JOB,PBS_EVENTCLASS_JOB,pjob->ji_qs.ji_jobid,log_buf);  
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
        sprintf(log_buf, "stray job %s reported on %s", jobidstr, np->nd_name);

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

  struct pbsnode *np,
  const char     *str)

  {
  std::string message;
  std::string errmsg = std::string(str).substr(8, std::string::npos);
  std::string oldnote = "";

  // remove newlines from string
  errmsg.erase(std::remove(errmsg.begin(), errmsg.end(), '\n'), errmsg.end());

  // If a note already exists, append the error; otherwise, create a new note
  if (np->nd_note != NULL)
    {
    oldnote = np->nd_note;

    // If this evaluates to true, we know that the error message has already been added to the note
    if (oldnote.find(errmsg) != std::string::npos)
      {
      return(PBSE_NONE);
      }

    // append message
    message = oldnote + " - " + errmsg;
    free(np->nd_note);
    }
  else
    message = errmsg;

  np->nd_note = strdup(message.c_str());

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
  if (np->nd_note != NULL)
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
      free(np->nd_note);
      np->nd_note = NULL;
      }
    else
      {
      message = oldnote.substr(0, found);
      free(np->nd_note);
      np->nd_note = strdup(message.c_str());
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
  if (!strncmp(np->nd_name, "0x", 2))
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
    cp = strdup(node_name);
    free(np->nd_name);
    np->nd_name = (char *)cp;
    np->nd_first = init_prop(np->nd_name);
    np->nd_last = np->nd_first;
    np->nd_f_st = init_prop(np->nd_name);
    np->nd_l_st = np->nd_f_st;
    }

  return(PBSE_NONE);
  } /* END process_uname_str() */




int process_state_str(

  struct pbsnode *np,
  const char    *str)

  {
  char            log_buf[LOCAL_LOG_BUF_SIZE];
  int             rc = PBSE_NONE;

  if (np->nd_state & INUSE_NOHIERARCHY)
    {
    sprintf(log_buf, "node %s has not received its hierarchy yet.",
      (np->nd_name != NULL) ? np->nd_name : "NULL");

    log_err(-1, __func__, log_buf);
    return PBSE_HIERARCHY_NOT_SENT;
    }
  if (!strncmp(str, "state=down", 10))
    {
    update_node_state(np, INUSE_DOWN);
    }
  else if (!strncmp(str, "state=busy", 10))
    {
    if (np->nd_state == INUSE_DOWN)
      {
      restore_note(np);
      }

    update_node_state(np, INUSE_BUSY);
    }
  else if (!strncmp(str, "state=free", 10))
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
      (np->nd_name != NULL) ? np->nd_name : "NULL");
    
    log_err(-1, __func__, log_buf);
    
    update_node_state(np, INUSE_UNKNOWN);
    }
  
  if (LOGLEVEL >= 9)
    {
    sprintf(log_buf, "node '%s' is at state '0x%x'\n",
      np->nd_name,
      np->nd_state);
    
    log_event(PBSEVENT_ADMIN, PBS_EVENTCLASS_SERVER, __func__, log_buf);
    }
  
  return(rc);
  } /* END process_state_str() */

int update_node_mac_addr(
    struct pbsnode *np,
    const char    *str)
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
  pbs_attribute  *temp)

  {
  int  rc = PBSE_NONE;
  char date_attrib[MAXLINE];

  /* it's nice to know when the last update happened */
  snprintf(date_attrib, sizeof(date_attrib), "rectime=%ld", (long)time(NULL));
  
  if (decode_arst(temp, NULL, NULL, date_attrib, 0))
    {
    DBPRT(("is_stat_get:  cannot add date_attrib\n"));
    }
  
  /* insert the information from "temp" into np */
  if ((rc = node_status_list(temp, np, ATR_ACTION_ALTER)) != PBSE_NONE)
    {
    DBPRT(("is_stat_get: cannot set node status list\n"));
    }

  free_arst(temp);

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
  bool               force)

  {
  char                     log_buf[LOCAL_LOG_BUF_SIZE];
  std::vector<std::string> valid_ids;

  if ((pnode->nd_layout == NULL) ||
      (force == true))
    {
    for (size_t i = 0; i < pnode->nd_job_usages.size(); i++)
      {
      const char *id = job_mapper.get_name(pnode->nd_job_usages[i].internal_job_id);
      
      if (id != NULL)
        valid_ids.push_back(id);
      }

    Machine *m = new Machine(layout, valid_ids);

    if (pnode->nd_layout != NULL)
      {
      m->save_allocations(*pnode->nd_layout);
      delete pnode->nd_layout;
      }

    pnode->nd_layout = m;
    }
  else if (((pnode->nd_layout->getTotalThreads() != pnode->nd_slots.get_total_execution_slots()) ||
            (pnode->nd_ngpus != pnode->nd_layout->get_total_gpus())) &&
           (pnode->nd_job_usages.size() == 0))
    {
    int old_count = pnode->nd_layout->getTotalThreads();
    int new_count = pnode->nd_slots.get_total_execution_slots();

    // If the number of np for the node has changed, then we should get a new layout as long
    // as we don't have active jobs
    delete pnode->nd_layout;
    
    for (size_t i = 0; i < pnode->nd_job_usages.size(); i++)
      {
      const char *id = job_mapper.get_name(pnode->nd_job_usages[i].internal_job_id);

      if (id != NULL)
        valid_ids.push_back(id);
      }
    
    pnode->nd_layout = new Machine(layout, valid_ids);

    if (LOGLEVEL >= 3)
      {
      snprintf(log_buf, sizeof(log_buf),
        "Node %s appears to have had it's core/thread count updated. The layout has now also been updated from %d to %d.",
        pnode->nd_name, old_count, new_count);

      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, log_buf);
      }
    }
  } // END update_layout_if_needed()
#endif



int process_status_info(

  const char               *nd_name,
  std::vector<std::string> &status_info)

  {
  const char     *name = nd_name;
  struct pbsnode *current;
  long            mom_job_sync = FALSE;
  long            auto_np = FALSE;
  long            down_on_error = FALSE;
  long            note_append_on_error = FALSE;
  int             dont_change_state = FALSE;
  pbs_attribute   temp;
  int             rc = PBSE_NONE;
  bool            send_hello = false;
#ifdef PENABLE_LINUX_CGROUPS
  bool            force_layout_update = false;
#endif

  get_svr_attr_l(SRV_ATR_MomJobSync, &mom_job_sync);
  get_svr_attr_l(SRV_ATR_AutoNodeNP, &auto_np);
  get_svr_attr_l(SRV_ATR_NoteAppendOnError, &note_append_on_error);
  get_svr_attr_l(SRV_ATR_DownOnError, &down_on_error);

  /* Before filling the "temp" pbs_attribute, initialize it.
   * The second and third parameter to decode_arst are never
   * used, so just leave them empty. (GBS) */
  memset(&temp, 0, sizeof(temp));

  if ((rc = decode_arst(&temp, NULL, NULL, NULL, 0)) != PBSE_NONE)
    {
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, "cannot initialize attribute");
    return(rc);
    }

  /* if original node cannot be found do not process the update */
  if ((current = find_nodebyname(nd_name)) == NULL)
    return(PBSE_NONE);

  //A node we put to sleep is up and running.
  if (current->nd_power_state != POWER_STATE_RUNNING)
    {
    //Make sure we wait for a stray update that came after we changed the state to pass
    //by.
    if((current->nd_power_state_change_time + NODE_POWER_CHANGE_TIMEOUT) < time(NULL))
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
      /* if we've already processed some, save this before moving on */
      if (i != 0)
        save_node_status(current, &temp);
      
      dont_change_state = FALSE;

      if ((current = get_numa_from_str(str, current)) == NULL)
        break;
      else
        continue;
      }
    else if (!strncmp(str, "node=", strlen("node=")))
      {
      /* if we've already processed some, save this before moving on */
      if (i != 0)
        save_node_status(current, &temp);

      dont_change_state = FALSE;

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
            dont_change_state = TRUE;
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
      str = status_info[i].c_str();
      }
    else if (!strcmp(str, START_MIC_STATUS))
      {
      process_mic_status(current, i, status_info);
      str = status_info[i].c_str();
      }
#ifdef PENABLE_LINUX_CGROUPS
    else if (!strcmp(str, "force_layout_update"))
      force_layout_update = true;
    else if (!strncmp(str, "layout", 6))
      {
      update_layout_if_needed(current, status_info[i], force_layout_update);

      // reset this to false in case we have a mom hierarchy in place
      force_layout_update = false;

      continue;
      }
#endif
    else if (!strcmp(str, "first_update=true"))
      {
      /* mom is requesting that we send the mom hierarchy file to her */
      //remove_hello(&hellos, current->nd_id);
      send_hello = true;
      
      /* reset gpu data in case mom reconnects with changed gpus */
      clear_nvidia_gpus(current);
      }
    else if ((rc = decode_arst(&temp, NULL, NULL, str, 0)) != PBSE_NONE)
      {
      DBPRT(("is_stat_get: cannot add attributes\n"));

      free_arst(&temp);

      break;
      }

    if (!strncmp(str, "state", 5))
      {
      if (dont_change_state == FALSE)
        process_state_str(current, str);
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
        update_node_state(current, INUSE_DOWN);
        dont_change_state = TRUE;
	if (note_append_on_error == TRUE)
	  {
            set_note_error(current, str);
	  }
        }
      }
    else if (!strncmp(str,"macaddr=",8))
      {
      update_node_mac_addr(current,str + 8);
      }
    else if ((mom_job_sync == TRUE) &&
             (!strncmp(str, "jobdata=", 8)))
      {
      /* update job attributes based on what the MOM gives us */      
      update_job_data(current, str + strlen("jobdata="));
      }
    else if ((mom_job_sync == TRUE) &&
             (!strncmp(str, "jobs=", 5)))
      {
      /* walk job list reported by mom */
      size_t         len = strlen(str) + strlen(current->nd_name) + 2;
      char          *jobstr = (char *)calloc(1, len);
      sync_job_info *sji = (sync_job_info *)calloc(1, sizeof(sync_job_info));

      if ((jobstr != NULL) &&
          (sji != NULL))
        {
        sprintf(jobstr, "%s:%s", current->nd_name, str+5);
        sji->input = jobstr;
        sji->timestamp = time(NULL);

        /* sji must be freed in sync_node_jobs */
        enqueue_threadpool_request(sync_node_jobs, sji, task_pool);
        }
      else
        {
        if (jobstr != NULL)
          {
          free(jobstr);
          }
        if (sji != NULL)
          {
          free(sji);
          }
        }
      }
    else if (auto_np)
      {
      if (!(strncmp(str, "ncpus=", 6)))
        {
        handle_auto_np(current, str);
        }
      }
    } /* END processing strings */

  if (current != NULL)
    {
    save_node_status(current, &temp);
    unlock_node(current, __func__, NULL, LOGLEVEL);
    }
  
  if ((rc == PBSE_NONE) &&
      (send_hello == true))
    rc = SEND_HELLO;
    
  return(rc);
  } /* END process_status_info() */



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
    sprintf(log_buf, "received gpu status from node %s", np->nd_name);

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
            np->nd_name);

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
      if ((gpuidx >= 0) && (np->nd_gpusn[gpuidx].gpuid == NULL))
        {
        np->nd_gpusn[gpuidx].gpuid = strdup(gpuid);
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
      if ((!memcmp(str + 9, "Normal", 6)) || (!memcmp(str + 9, "Default", 7)))
        {
        np->nd_gpusn[gpuidx].mode = gpu_normal;
        if (gpu_has_job(np, gpuidx))
          {
          np->nd_gpusn[gpuidx].state = gpu_shared;
          }
        else
          {
          np->nd_gpusn[gpuidx].state = gpu_unallocated;
          }
        }
      else if ((!memcmp(str + 9, "Exclusive", 9)) ||
              (!memcmp(str + 9, "Exclusive_Thread", 16)))
        {
        np->nd_gpusn[gpuidx].mode = gpu_exclusive_thread;
        if (gpu_has_job(np, gpuidx))
          {
          np->nd_gpusn[gpuidx].state = gpu_exclusive;
          }
        else
          {
          np->nd_gpusn[gpuidx].state = gpu_unallocated;
          }
        }
      else if (!memcmp(str + 9, "Exclusive_Process", 17))
        {
        np->nd_gpusn[gpuidx].mode = gpu_exclusive_process;
        if (gpu_has_job(np, gpuidx))
          {
          np->nd_gpusn[gpuidx].state = gpu_exclusive;
          }
        else
          {
          np->nd_gpusn[gpuidx].state = gpu_unallocated;
          }
        }
      else if (!memcmp(str + 9, "Prohibited", 10))
        {
        np->nd_gpusn[gpuidx].mode = gpu_prohibited;
        np->nd_gpusn[gpuidx].state = gpu_unavailable;
        }
      else
        {
        /* unknown mode, default to prohibited */
        np->nd_gpusn[gpuidx].mode = gpu_prohibited;
        np->nd_gpusn[gpuidx].state = gpu_unavailable;
        if (LOGLEVEL >= 3)
          {
          sprintf(log_buf,
            "GPU %s has unknown mode on node %s",
            gpuid,
            np->nd_name);

          log_ext(-1, __func__, log_buf, LOG_DEBUG);
          }
        }
 
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
