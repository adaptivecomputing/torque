#include "license_pbs.h" /* See here for the software license */
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
/* declarations/includes for Global Vars */
#include "list_link.h" /* tlist_head, list_link */
#include "net_connect.h"
#include "log.h" /* LOG_BUF_SIZE */
#include "pbs_job.h" /* job */
#include "tm_.h"
#include "dis.h"
#include "libpbs.h"
#include "resource.h"
#include "mom_mach.h"
#include "test_catch_child.h"
#include "batch_request.h" /* batch_request */
#include "errno.h"
#include "mom_func.h"
#include "mom_job_cleanup.h"
#include "complete_req.hpp"

int server_down;
int called_open_socket = 0;
int called_fork_me = 0;
bool check_rur = true;

char         mom_alias[PBS_MAXHOSTNAME + 1];

std::list<job *>              alljobs_list;
std::vector<exiting_job_info> exiting_job_list;

const char *PMOMCommand[] =
  {
  "ALL_OKAY",
  "JOIN_JOB",
  "KILL_JOB",
  "SPAWN_TASK",
  "GET_TASKS",
  "SIGNAL_TASK",
  "OBIT_TASK",
  "POLL_JOB",
  "GET_INFO",
  "GET_RESC",
  "ABORT_JOB",
  "GET_TID",
  "RADIX_ALL_OK",
  "JOIN_JOB_RADIX",
  "KILL_JOB_RADIX",
  "ERROR",     /* 14+ */
  NULL
  };

int DIS_reply_read_count;
bool eintr_test;
int termin_child = 0; /* mom_main.c */
int LOGLEVEL = 10; /* force logging code to be exercised as tests run */ /* mom_main.c/pbsd_main.c */
tlist_head svr_alljobs; /* mom_main.c */
char log_buffer[LOG_BUF_SIZE]; /* pbs_log.c */
int    multi_mom = 1; /* mom_main.c */ /* 1 to trigger conditions */
unsigned int pbs_rm_port = 0; /* mom_main.c */
int    exiting_tasks = 0; /* mom_main.c */
char pbs_current_user[PBS_MAXUSER]; /* PBS_data.c */
int resc_access_perm = 0; /* attr_fn_resc */
int svr_resc_size = 0; /* resc_def_all.c */
resource_def *svr_resc_def; /* resc_def_all.c */
char        *path_epiloguser; /* mom_main.c */
char        *path_epilog; /* mom_main.c */
int PBSNodeCheckEpilog = 1; /* mom_main.c */ /* 1 to trigger conditions */
const char *msg_daemonname = "unset"; /* pbs_log.c */
char *path_jobs; /* mom_main.c */
tlist_head mom_polljobs; /* mom_main.c */
char        *path_epiloguserp; /* mom_main.c */
char        *path_epilogp; /* mom_main.c */
attribute_def job_attr_def[10]; /* src/server/job_attr_def.c */
bool exit_called = false;
int  job_exit_wait_time = 600;

int tc = 0; /* Used for test routining */
int func_num = 0;
int test_exit_called = 0;
int ran_one = 0;
int the_sock = 0;
job *lastpjob = NULL;
int is_login_node;


void exit_test(int num)
  {
  test_exit_called = num;
  }

void log_record(int eventtype, int objclass, const char *objname, const char *text)
  {
  }

void clear_down_mom_servers(void)
  {
  }

void *get_next(list_link  pl, char     *file, int      line)
  {
  job *pjob = NULL;
  struct brp_status *stats = NULL;
  svrattrl *sattr = NULL;
  task *ptask = NULL;
  obitent *pobit = NULL;
  switch (func_num)
    {
  case OBIT_REPLY:
      {
      switch (ran_one)
        {
      case 0:
        pjob = (job *)calloc(1, sizeof(job));
        break;
      case 1:
        pjob = (job *)calloc(1, sizeof(job));
        pjob->ji_qs.ji_substate = JOB_SUBSTATE_OBIT;
        pjob->ji_momhandle = the_sock;
        if (tc == 4)
          {
          pjob->ji_wattr[JOB_ATR_interactive].at_val.at_long = 0;
          }
        else if (tc == 9)
          {
          pjob->ji_wattr[JOB_ATR_interactive].at_flags |= ATR_VFLAG_SET;
          pjob->ji_wattr[JOB_ATR_interactive].at_val.at_long = 0;
          }
        else if (tc == 10)
          {
          pjob->ji_momhandle = -1;
          }
        break;
      case 2:
        break;
        }
      }
    break;
  case PREOBIT_REPLY:
      {
      switch (ran_one)
        {
      case 1:
        if (((tc >= 4) && (tc <= 11)) || (tc == 12) || (tc == 13) || (tc == 14))
          pjob = (job *)calloc(1, sizeof(job));
        break;
      case 2:
        if (tc != 2)
          {
          pjob = (job *)calloc(1, sizeof(job));
          }
        break;
      case 3:
        pjob = (job *)calloc(1, sizeof(job));
        if (tc != 1)
          {
          pjob->ji_qs.ji_substate = JOB_SUBSTATE_PREOBIT;
          if ((tc == 10) || (tc == 11) || (tc == 12) || (tc == 13) || (tc == 14))
            {
            pjob->ji_hosts = (hnodent *)calloc(2, sizeof(hnodent));
            pjob->ji_hosts[0].hn_host = (char *)calloc(9, 1);
            strcpy(pjob->ji_hosts[0].hn_host, "myserver");
            if (tc == 11)
              {
              pjob->ji_wattr[JOB_ATR_interactive].at_flags |= ATR_VFLAG_SET;
              pjob->ji_wattr[JOB_ATR_interactive].at_val.at_long = 1;
              }
            else if (tc == 13)
              {
              pjob->ji_wattr[JOB_ATR_interactive].at_flags |= ATR_VFLAG_SET;
              pjob->ji_wattr[JOB_ATR_interactive].at_val.at_long = 1;
              }
            }
          }
        else
          {
          pjob->ji_qs.ji_substate = JOB_SUBSTATE_EXITING;
          }
        pjob->ji_momhandle = the_sock;
        break;
      case 4:
        if (tc == 8)
          {
          ran_one++;
          return stats;
          }
        else if (tc == 9)
          {
          stats = (struct brp_status *)calloc(1, sizeof(struct brp_status));
          strcpy(stats->brp_objname, "tc8");
          ran_one++;
          return stats;
          }
        else
          {
          pjob = (job *)calloc(1, sizeof(job));
          pjob->ji_qs.ji_substate = JOB_SUBSTATE_PREOBIT;
          pjob->ji_momhandle = the_sock;
          }
        break;
      case 5:
        if (tc == 9)
          {
          sattr = (svrattrl *)calloc(1, sizeof(svrattrl));
          ran_one++;
          return sattr;
          }
        else if ((tc == 10) || (tc == 12) || (tc == 13) || (tc == 14))
          {
          sattr = (svrattrl *)calloc(1, sizeof(svrattrl));
          sattr->al_name = (char *)calloc(10, 1);
          strcpy(sattr->al_name, ATTR_exechost);
          sattr->al_value = (char *)calloc(9, 1);
          strcpy(sattr->al_value, "myserver");
          ran_one++;
          return sattr;
          }
        else if (tc == 11)
          {
          sattr = (svrattrl *)calloc(1, sizeof(svrattrl));
          sattr->al_name = (char *)calloc(7, 1);
          strcpy(sattr->al_name, "not it");
          ran_one++;
          return sattr;
          }
        break;
      case 6:
        if (tc == 11)
          {
          sattr = (svrattrl *)calloc(1, sizeof(svrattrl));
          sattr->al_name = (char *)calloc(10, 1);
          strcpy(sattr->al_name, ATTR_exechost);
          sattr->al_value = (char *)calloc(12, 1);
          strcpy(sattr->al_value, "notmyserver");
          ran_one++;
          return sattr;
          }
        }
      }
    break;
  case SCAN_FOR_EXITING:
      {
      switch (ran_one)
        {
      case 0:
        if (tc == 1)
          {
          /*
           * Case 0 - 370 to 392
           */
          pjob = (job *)calloc(1, sizeof(job));
          }
        else if (tc == 2)
          {
          /* setenv
           * expected exit 826
           */
          pjob = (job *)calloc(1, sizeof(job));
          pjob->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
          pjob->ji_qs.ji_un.ji_momt.ji_exitstat = TM_NULL_TASK;
          pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HERE;
          pjob->ji_qs.ji_substate = JOB_SUBSTATE_NOTERM_REQUE;
          pjob->ji_wattr[1].at_val.at_long = 3;
          pjob->ji_sampletim = 1;
          break;
          }
        else if (pjob == NULL)
          {
          /* pjob == NULL */
          }
        break;
      case 1:
        if (tc == 1)
          {
          /* Line 372
           * expected return at 403
           */
          pjob = (job *)calloc(1, sizeof(job));
          pjob->ji_flags |= MOM_CHECKPOINT_ACTIVE;
          }
        else if (tc == 2)
          {
          ptask = (task *)calloc(1, sizeof(task));
          ptask->ti_qs.ti_status = TI_STATE_EXITED;
          ptask->ti_qs.ti_parenttask = TM_NULL_TASK;
          ran_one++;
          return ptask;
          }
        break;
      case 2:
        if (tc == 1)
          {
          /* Line 372
           * expected return at 415
           */
          pjob = (job *)calloc(1, sizeof(job));
          pjob->ji_flags |= MOM_CHECKPOINT_POST;
          }
        else if (tc == 2)
          {
          /* pobit == NULL */
          }
        break;
      case 3:
        /* Line 372
         * expected return at 421
         */
        if (tc == 1)
          {
          pjob = (job *)calloc(1, sizeof(job));
          }
        else if (tc == 2)
          {
          /* ptask == NULL */
          }
        break;
      case 4:
        if (tc == 1)
          {
          /* Line 372
           * return at 687->705 (After completing coverage both loops)
           */
          pjob = (job *)calloc(1, sizeof(job));
          pjob->ji_nodeid = 123;
          pjob->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
          pjob->ji_wattr[0].at_val.at_long = 0;
          pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HERE;
          pjob->ji_qs.ji_svrflags |= JOB_SVFLG_INTERMEDIATE_MOM;
          pjob->ji_qs.ji_substate = JOB_SUBSTATE_SUSPEND;
          pjob->ji_vnods = (vnodent *)calloc(1, sizeof(vnodent));
          pjob->ji_numvnod = 2;
          pjob->ji_vnods = (vnodent *)calloc(2, sizeof(vnodent));
          pjob->ji_vnods[0].vn_node = 123;
          pjob->ji_vnods[0].vn_host = (hnodent *)calloc(1, sizeof(hnodent));
          pjob->ji_vnods[0].vn_host->hn_node = 123;
          pjob->ji_vnods[1].vn_node = 321;
          pjob->ji_vnods[1].vn_host = (hnodent *)calloc(1, sizeof(hnodent));
          pjob->ji_vnods[1].vn_host->hn_node = 321;
          }
        else if (tc == 2)
          {
          /* ptask == NULL */
          }
        break;
      case 5:
        if (tc == 1)
          {
          /* Line 441
           * expected return 446
           */
          ptask = (task *)calloc(1, sizeof(task));
          ptask->ti_qs.ti_status = TI_STATE_EMBRYO;
          ran_one++;
          return ptask;
          }
        else if (tc == 2)
          {
          pjob = (job *)calloc(1, sizeof(job));
          pjob->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
          pjob->ji_qs.ji_un.ji_momt.ji_exitstat = TM_NULL_TASK;
          pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HERE;
          pjob->ji_qs.ji_substate = JOB_SUBSTATE_NOTERM_REQUE;
          }
        break;
      case 6:
        if (tc == 1)
          {
          /* Line 443
           * path conditions
           * mom_radix < 2
           * pjob->ji_sampletim = 0
           * pjob->ji_qs.ji_svrflags & JOB_SVFLG_INTERMEDIATE_MOM ==0
           * num_sisters = 0
           * multi_mom = 1
           */
          ptask = (task *)calloc(1, sizeof(task));
          ptask->ti_qs.ti_status = TI_STATE_EXITED;
          ptask->ti_qs.ti_parenttask = TM_NULL_TASK;
          ran_one++;
          return ptask;
          }
        else if (tc == 2)
          {
          /* ptask -- NULL */
          }
        break;
      case 7:
        if (tc == 1)
          {
          /* Line 545
           * path conditions
           * pjob->ji_nodeid == pnode->hn_node
           * tp->ti_fd != -1
           */
          pobit = (obitent *)calloc(1, sizeof(obitent));
          pobit->oe_info.fe_node = 123;
          ran_one++;
          return pobit;
          }
        else if (tc == 2)
          {
          /* ptask == NULL */
          }
        break;
      case 8:
        if (tc == 1)
          {
          /* Line 545
           * path conditions
           * pjob->ji_nodeid != pnode->hn_node
           */
          pobit = (obitent *)calloc(1, sizeof(obitent));
          pobit->oe_info.fe_node = 321;
          ran_one++;
          return pobit;
          }
        else if (tc == 2)
          {
          /* exit at 826 */
          pjob = (job *)calloc(1, sizeof(job));
          pjob->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
          pjob->ji_qs.ji_un.ji_momt.ji_exitstat = TM_NULL_TASK;
          pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HERE;
          pjob->ji_qs.ji_substate = JOB_SUBSTATE_NOTERM_REQUE;
          }
        break;
      case 9:
        if (tc == 1)
          {
          /* Line 545
           * exit condition
           * pobit == NULL
           */
          }
        else if (tc == 2)
          {
          /* ptask == NULL */
          }
        break;
      case 10:
        if (tc == 1)
          {
          /* Line 443
           * exit condition
           * ptask == NULL
           */
          }
        else if (tc == 2)
          {
          /* ptask == NULL */
          }
        break;
      case 11:
        /* Line 372
         * return at 660
         */
        pjob = (job *)calloc(1, sizeof(job));
        pjob->ji_nodeid = 123;
        pjob->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
        pjob->ji_wattr[1].at_val.at_long = 3;
        pjob->ji_qs.ji_un.ji_momt.ji_exitstat = TM_NULL_TASK;
        pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HERE;
        pjob->ji_qs.ji_substate = JOB_SUBSTATE_STAGEOUT;
        break;
      case 12:
        /* Line 443
         * path conditions
         * mom_radix > 2
         * time_now - pjob->ji_sampletim > 5
         * pjob->ji_qs.ji_svrflags & JOB_SVFLG_INTERMEDIATE_MOM ==0
         * num_sisters = 1
         * multi_mom = 1
         */
        ptask = (task *)calloc(1, sizeof(task));
        if (tc == 1)
          {
          ptask->ti_qs.ti_status = TI_STATE_EXITED;
          ptask->ti_qs.ti_parenttask = TM_NULL_TASK;
          ran_one++;
          return ptask;
          }
        break;
      case 13:
        /* Line 545
         * exit condition
         * pobit == NULL
         */
        break;
      case 14:
        /* Line 443
         * exit condition
         * ptask == NULL
         */
        break;
      case 15:
        /* Line 372
         * return at 637
         */
        pjob = (job *)calloc(1, sizeof(job));
        pjob->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
        pjob->ji_qs.ji_un.ji_momt.ji_exitstat = TM_NULL_TASK;
/*        pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HERE; */
        pjob->ji_qs.ji_svrflags |= JOB_SVFLG_INTERMEDIATE_MOM;
        pjob->ji_qs.ji_substate = JOB_SUBSTATE_STAGEOUT;
        break;
      case 16: /* ptask == NULL */
        break;
      case 17:
        /* Line 372
         * return at 676
         */
        pjob = (job *)calloc(1, sizeof(job));
        pjob->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
        pjob->ji_qs.ji_un.ji_momt.ji_exitstat = TM_NULL_TASK;
        pjob->ji_qs.ji_substate = JOB_SUBSTATE_EXITING;
        break;
      case 18: /* ptask == NULL */
        break;
      case 19:
        /* Line 372
         * return at 702->705
         */
        pjob = (job *)calloc(1, sizeof(job));
        pjob->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
        pjob->ji_qs.ji_un.ji_momt.ji_exitstat = TM_NULL_TASK;
        pjob->ji_qs.ji_svrflags |= JOB_SVFLG_INTERMEDIATE_MOM;
        pjob->ji_qs.ji_substate = JOB_SUBSTATE_NOTERM_REQUE;
        break;
      case 20:
        break;
      case 21: /* ptask == NULL */
        /* Line 372
         * return at 718->787
         * path condition
         * pjob->ji_qs.ji_svrflags & JOB_SVFLG_INTERMEDIATE_MOM
         * pjob->ji_qs.ji_substate != JOB_SUBSTATE_EXITING
         */
        pjob = (job *)calloc(1, sizeof(job));
        pjob->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
        pjob->ji_qs.ji_un.ji_momt.ji_exitstat = TM_NULL_TASK;
        pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HERE;
        pjob->ji_qs.ji_substate = JOB_SUBSTATE_EXITING;
        break;
      case 22: /* ptask == NULL */
        break;
      case 23:
        /* Line 372
         * return at 721->817
         * path condition
         * pjob->ji_qs.ji_svrflags & JOB_SVFLG_INTERMEDIATE_MOM
         * pjob->ji_qs.ji_substate == JOB_SUBSTATE_EXITING
         */
        pjob = (job *)calloc(1, sizeof(job));
        pjob->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
        pjob->ji_qs.ji_un.ji_momt.ji_exitstat = TM_NULL_TASK;
        pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HERE;
        pjob->ji_qs.ji_substate = JOB_SUBSTATE_NOTERM_REQUE;
        break;
      case 24: /* ptask == NULL */
        break;
      case 25:
        ptask = (task *)calloc(1, sizeof(task));
        ptask->ti_qs.ti_status = TI_STATE_RUNNING;
        ran_one++;
        return ptask;
        break;
      case 26: /* ptask == NULL */
        break;
        /* ENDS HERE!!! */
      default:
/*        lastpjob = pjob; */
        break;
        }
      }
    break;
  case EXIT_MOM_JOB:
      {
      switch (ran_one)
        {
      case 0:
        if (tc == 3)
          {
          ptask = (task *)calloc(1, sizeof(task));
          ptask->ti_qs.ti_status = TI_STATE_EXITED;
          ran_one++;
          return ptask;
          }
        else if  (tc == 4)
          {
          /* ptask == NULL */
          }
        break;
      case 1:
        if (tc == 3)
          {
          /* returns at 2128 */
          ptask = (task *)calloc(1, sizeof(task));
          ptask->ti_qs.ti_status = TI_STATE_RUNNING;
          ran_one++;
          return ptask;
          }
        break;
      case 2:
        if (tc == 3)
          {
          /* ptask == NULL */
          }
        break;
      default:
        break;
        }
      }
  default:
    break;
    }
  ran_one++;
  return pjob;
  }

int is_mom_server_down(pbs_net_t server_address)
  {
  int rc = 0;
  if ((tc == 1) && (ran_one == 1))
    {
    rc = 1;
    }
  else if (server_down == TRUE)
    rc = TRUE;

  return rc;
  }

void checkpoint_partial(job *pjob)
  {
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  {
  }

int send_sisters(job *pjob, int com, int using_radix, std::set<int> *sisters_contacted)
  {
  int rc = 1;
  if (func_num == SCAN_FOR_EXITING)
    {
    if (ran_one == 7)
      {
      if (tc == 1)
        {
        rc = 0;
        }
      else if (tc == 2)
        {
        rc = 1;
        }
      }
    }
  return rc;
  }

int send_sisters_radix(job *pjob, int com)
  {
  return 1;
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  return 1;
  }

task *task_find(job *pjob, tm_task_id taskid)
  {
  task *tp = (task *)calloc(1, sizeof(task));
  if (ran_one == 9)
    {
    tp->ti_chan = NULL;
    }
  else if (ran_one == 10)
    {
    tp->ti_chan = NULL;
    }
  return tp;
  }

int tm_reply(tcp_chan *, int com, int i)
  {
  return 0;
  }

int diswsl(struct tcp_chan *chan, long value)
  {
  return 0;
  }

#undef diswsi
int diswsi(struct tcp_chan *chan, int value)
  {
  return diswsl(chan, (long) value);
  }

int DIS_tcp_wflush(struct tcp_chan *chan)
  {
  return 0;
  }

int im_compose(struct tcp_chan *chan, char *jobid, const char *cookie, int command, tm_event_t event, tm_task_id taskid)
  {
  return 0;
  }

int rpp_flush(int index)
  {
  return 0;
  }

void delete_link(struct list_link *old)
  {
  }

int task_save(task *ptask)
  {
  return 0;
  }

const char *PJobSubState[] =
  {
  "TRANSIN",                /* Transit in, wait for commit */
  "TRANSICM",               /* Transit in, wait for commit */
  "TRNOUT",                 /* transiting job outbound */
  "TRNOUTCM",               /* transiting outbound, rdy to commit */
  "SUBSTATE04",
  "SUBSTATE05",
  "SUBSTATE06",
  "SUBSTATE07",
  "SUBSTATE08",
  "SUBSTATE09",
  "QUEUED",                 /* job queued and ready for selection */
  "PRESTAGEIN",             /* job queued, has files to stage in */
  "SUBSTATE12",
  "SYNCRES",                /* job waiting on sync start ready */
  "STAGEIN",                /* job staging in files then wait */
  "STAGEGO",                /* job staging in files and then run */
  "STAGECMP",               /* job stage in complete */
  "SUBSTATE17",
  "SUBSTATE18",
  "SUBSTATE19",
  "HELD",      /* job held - user or operator */
  "SYNCHOLD",  /* job held - waiting on sync regist */
  "DEPNHOLD",  /* job held - waiting on dependency */
  "SUBSTATE23",
  "SUBSTATE24",
  "SUBSTATE25",
  "SUBSTATE26",
  "SUBSTATE27",
  "SUBSTATE28",
  "SUBSTATE29",
  "WAITING",   /* job waiting on execution time */
  "SUBSTATE31",
  "SUBSTATE32",
  "SUBSTATE33",
  "SUBSTATE34",
  "SUBSTATE35",
  "SUBSTATE36",
  "STAGEFAIL", /* job held - file stage in failed */
  "SUBSTATE38",
  "SUBSTATE39",
  "PRERUN",    /* job sent to MOM to run */
  "STARTING",  /* final job start initiated */
  "RUNNING",   /* job running */
  "SUSPEND",   /* job suspended, CRAY only */
  "SUBSTATE44",
  "SUBSTATE45",
  "SUBSTATE46",
  "SUBSTATE47",
  "SUBSTATE48",
  "SUBSTATE49",
  "EXITING",   /* Start of job exiting processing */
  "STAGEOUT",  /* job staging out (other) files   */
  "STAGEDEL",  /* job deleteing staged out files  */
  "EXITED",    /* job exit processing completed   */
  "ABORT",     /* job is being aborted by server  */
  "SUBSTATE55",
  "SUBSTATE56",
  "PREOBIT",   /* preobit job status */
  "OBIT",      /* (MOM) job obit notice sent */
  "COMPLETED",
  "RERUN",     /* job is rerun, recover output stage */
  "RERUN1",    /* job is rerun, stageout phase */
  "RERUN2",    /* job is rerun, delete files stage */
  "RERUN3",    /* job is rerun, mom delete job */
  "RETSTD",    /* job has checkpoint file, return stdout / stderr files to server
                * spool dir so that job can be restarted
                */
  NULL
  };

int kill_job(job *pjob, int sig, const char *killer_id_name, const char *why_killed_reason)
  {
  if (func_num == INIT_ABORT_JOBS)
    {
    if ((tc == 12) || (tc == 13))
      {
      pjob->ji_qs.ji_svrflags = 0;
      pjob->ji_qs.ji_svrflags |= JOB_SVFLG_CHECKPOINT_MIGRATEABLE | JOB_SVFLG_HERE;
      }
    }
  return 0;
  }

int mom_open_socket_to_jobs_server(job *pjob, const char *caller_id, void *(*message_handler)(void *))
  {
  called_open_socket++;
  int sock = 1;
  if (func_num == POST_EPILOGUE)
    {
    if (tc == 1)
      {
      if (ran_one == 0)
        {
        ran_one++;
        sock = -1;
        }
      }
    else if (tc == 2)
      {
      sock = -1;
      }
    }
  else if (func_num == SCAN_FOR_EXITING)
    {
    if (ran_one == 23)
      {
      sock = -1;
      errno = EINPROGRESS;
      }
    }
  return sock;
  }

struct tcp_chan *DIS_tcp_setup(int sock)
  {
  static tcp_chan bob;
  return(&bob);
  }

void DIS_tcp_close(struct tcp_chan *chan) {}

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

void set_mom_server_down(pbs_net_t server_address)
  {
  }

int encode_DIS_ReqHdr(struct tcp_chan *chan, int reqt, char *user)
  {
  int rc = 1;
  if (func_num == POST_EPILOGUE)
    {
    switch(tc)
      {
    case 5:
      rc = 0;
      break;
    case 6:
      rc = 0;
      break;
    case 7:
      rc = 0;
      break;
      }
    }
  else if (func_num == SCAN_FOR_EXITING)
    {
    if ((tc == 1) && (ran_one == 27))
      {
      rc = 0;
      }
    else if (tc == 2)
      {
      rc = 0;
      }
    }
  return rc;
  }

int encode_DIS_Status(struct tcp_chan *chan, char *objid, struct attrl *pattrl)
  {
  int rc= 1;
  if (func_num == SCAN_FOR_EXITING)
    {
    if ((tc == 1) && (ran_one == 27))
      {
      rc = 0;
      }
    else if (tc == 2)
      {
      rc = 0;
      }
    }
  return rc;
  }

int encode_DIS_ReqExtend(struct tcp_chan *chan, char *extend)
  {
  int rc = 1;
  if (func_num == POST_EPILOGUE)
    {
    switch(tc)
      {
    case 4:
      rc = 0;
      break;
    case 6:
      rc = 0;
      break;
    case 7:
      rc = 0;
      break;
      }
    }
  else if (func_num == SCAN_FOR_EXITING)
    {
    if (tc == 2)
      {
      rc = 0;
      }
    }
  return rc;
  }

int no_mom_servers_down(void)
  {
  int rc = 1;
  return rc;
  }

int mark_for_resend(job *pjob)
  {
  return 1;
  }

struct batch_request *alloc_br(int type)
  {
  struct batch_request *br = NULL;
  if (func_num == POST_EPILOGUE)
    {
    if (tc > 3)
      {
      br = (struct batch_request *)calloc(1, sizeof(struct batch_request));
      }
    }
  else
    {
    br = (struct batch_request *)calloc(1, sizeof(struct batch_request));
    }
  return br;
  }

void encode_used(job *pjob, int i, std::stringstream *output, list_link *l)
  {
  }

void encode_flagged_attrs(job *pjob, int i, std::stringstream *output, list_link *l)
  {
  }

int encode_DIS_JobObit(struct tcp_chan *chan, struct batch_request *preq)
  {
  int rc = 1;
  if (func_num == POST_EPILOGUE)
    {
    switch(tc)
      {
    case 4:
      rc = 0;
      break;
    case 5:
      rc = 0;
      break;
    case 7:
      rc = 0;
      break;
      }
    }
  return rc;
  }

void free_br(struct batch_request *preq)
  {
  }

int DIS_reply_read(struct tcp_chan *chan, struct batch_reply *preply)
  {
  int rc = 0;

  if (eintr_test == true)
    {
    DIS_reply_read_count++;
    errno = EINTR;
    return(-1);
    }

  switch (func_num)
    {
  case OBIT_REPLY:
      {
      switch (tc)
        {
      case 1:
          rc = 1;
      case 2:
          preply->brp_code = PBSE_NONE;
          LOGLEVEL = 4;
        break;
      case 3:
          preply->brp_code = PBSE_ALRDYEXIT;
          LOGLEVEL = 7;
        break;
      case 4:
          preply->brp_code = PBSE_CLEANEDOUT;
        break;
      case 5:
          preply->brp_code = -1;
        break;
      case 6:
          preply->brp_code = PBSE_BADSTATE;
        break;
      case 7:
          preply->brp_code = PBSE_SYSTEM;
        break;
      case 8:
          preply->brp_code = PBSE_MOMREJECT;
        break;
      case 9:
          preply->brp_code = PBSE_CLEANEDOUT;
        break;
      case 10:
          preply->brp_code = PBSE_CLEANEDOUT;
        break;
      default:
        break;
        }
      break;
      }
    break;
    case PREOBIT_REPLY:
      {
      switch (tc)
        {
      case 1:
        rc = 1;
        break;
      case 2:
        rc = 0;
        break;
      case 4:
        preply->brp_code = PBSE_CLEANEDOUT;
        break;
      case 5:
        preply->brp_code = PBSE_NONE;
        break;
      case 6:
        preply->brp_code = PBSE_MOMREJECT;
        break;
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
        preply->brp_code = PBSE_NONE;
        preply->brp_choice = BATCH_REPLY_CHOICE_Status;
        break;
        rc = 1;
      default:
        break;
        }
      }
    break;
    default:
    break;
    }
  if (ran_one == 0)
    {
    errno = EINTR;
    }
  else
    {
    errno = 0;
    }
  ran_one++;
  return rc;
  }

void log_err(int errnum, const char *routine, const char *text)
  {
  }

void close_conn(int sd, int has_mutex)
  {
  }

void clear_conn(int sd, int has_mutex)
  {
  }

char *std_file_name(job *pjob, enum job_file which, int *keeping)
  {
  char *stuff = NULL;
  return stuff;
  }

int job_unlink_file(job *pjob, const char *name)
  {
  return 0;
  }

pid_t fork_me(int conn)
  {
  called_fork_me++;

  int rc = 0;
  if (func_num == PREOBIT_REPLY)
    {
    if (tc == 10)
      {
      rc = 1;
      }
    else if (tc == 12)
      {
      rc = -1;
      }
    } 
  return rc;
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  resource *presc = (resource *)calloc(1, sizeof(resource));
  presc->rs_value.at_flags |= ATR_VFLAG_SET;
  presc->rs_value.at_val.at_str = (char *)calloc(5, 1);
  strcpy(presc->rs_value.at_val.at_str, "hmm");
  return presc;
  }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  return NULL;
  }

char *get_local_script_path(job *pjob, char *base)
  {
  char *val = (char *)calloc(2, 1);
  val[0] = 'v';
  return val;
  }

int run_pelog(int which, char *specpelog, job *pjog, int pe_io_type, int deletejob)
  {
  return 1;
  }

void log_ext(int errnum, const char *routine, const char *text, int severity)
  {
  }

void check_state(int Force)
  {
  }

void mom_server_all_update_stat(void)
  {
  }

job *job_recov(const char *filename)
  {
  job *pjob = NULL;
  if (!((func_num == INIT_ABORT_JOBS) && (tc == 1)))
    {
    pjob = (job *)calloc(1,sizeof(job));
    }
  return pjob;
  }

void set_globid(job *pjob, struct startjob_rtn *sjr)
  {
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  }

void job_nodes(job &pjob)
  {
  }

int task_recov(job *pjob)
  {
  int rc = 0; /* SUCCESS */
  return rc;
  }

void mom_checkpoint_recover(job *pjob)
  {
#undef DBPRT
#define DBPRT(x)
  /* Building fake array */
  pjob->ji_numnodes = 3;
  pjob->ji_hosts = (hnodent *)calloc(3, sizeof(hnodent));
  pjob->ji_hosts[0].hn_host = (char *)calloc(1, 20);
  strcpy(pjob->ji_hosts[0].hn_host, "tom");
  pjob->ji_hosts[1].hn_host = (char *)calloc(1, 20);
  strcpy(pjob->ji_hosts[1].hn_host, "dick");
  pjob->ji_hosts[2].hn_host = (char *)calloc(1, 20);
  strcpy(pjob->ji_hosts[2].hn_host, "harry");
  pjob->ji_qs.ji_substate = JOB_SUBSTATE_RUNNING;
  pjob->ji_qs.ji_svrflags = JOB_SVFLG_HERE;
  pjob->ji_wattr[0].at_val.at_long = 0;
  pjob->ji_wattr[1].at_val.at_long = 1;
/*  pjob->ji_wattr */
  if (func_num == INIT_ABORT_JOBS)
    {
    if (tc == 3)
      {
      pjob->ji_grpcache = NULL;
      }
    else if (tc == 4)
      {
      pjob->ji_qs.ji_substate = JOB_SUBSTATE_PRERUN;
      }
    else if (tc == 5)
      {
      pjob->ji_qs.ji_substate = JOB_SUBSTATE_SUSPEND;
      }
    else if (tc == 6)
      {
      pjob->ji_qs.ji_substate = JOB_SUBSTATE_EXITED;
      }
    else if (tc == 7)
      {
      pjob->ji_qs.ji_substate = JOB_SUBSTATE_NOTERM_REQUE;
      }
    else if (tc == 8)
      {
      pjob->ji_qs.ji_substate = JOB_SUBSTATE_EXITING;
      }
    else if (tc == 9)
      {
      pjob->ji_qs.ji_svrflags = JOB_SVFLG_HERE;
      }
    else if (tc == 11)
      {
      pjob->ji_qs.ji_svrflags = JOB_SVFLG_HERE+1;
      pjob->ji_numnodes = 1;
      }
    else if (tc == 12)
      {
      pjob->ji_qs.ji_svrflags = JOB_SVFLG_HERE;
      pjob->ji_numnodes = 1;
      }
    }
  }

u_long addclient(const char *name)
  {
  return 0;
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  int rc = 0;
  if (func_num == INIT_ABORT_JOBS)
    {
    if (tc == 10)
      {
      rc = 1;
      }
    }
  else if (func_num == SCAN_FOR_EXITING)
    {
    if ((tc == 1) && (ran_one == 13))
      {
      rc = 1;
      }
    else if ((tc == 2) && (ran_one == 2))
      {
      rc = 1;
      }
    }
  return rc;
  }

int mom_do_poll(job *pjob)
  {
  int retval = 0;
  if ((func_num == INIT_ABORT_JOBS) && (tc == 2))
    {
    retval = 1;
    }
  return retval;
  }

void mom_job_purge(job *pjob)
  {
  return;
  }

int check_pwd(job *pjob)
  {
  return(-1);
  }

unsigned long gettime(resource *pres)
  {
  fprintf(stderr, "The call to gettime needs to be mocked!!\n");
  exit(1);
  }

int diswul(struct tcp_chan *chan, unsigned long value)
  {
  return 0;
  }

u_long resc_used(job *pjob, const char *name, u_long(*f) (resource *))
  {
  return 1;
  }

unsigned long getsize(resource *pres)
  {
  fprintf(stderr, "The call to getsize needs to be mocked!!\n");
  exit(1);
  }

int tcp_connect_sockaddr(struct sockaddr *sa, size_t sa_size, bool use_log)
  {
  return 1;
  }

int pbs_disconnect_socket(int sock)
  {                           
  return 1;
  }       

int add_to_resend_things(resend_momcomm *mc)
  {
  return(0);
  }

im_compose_info *create_compose_reply_info(const char *jobid, const char *cookie, hnodent *np, int command, tm_event_t event, tm_task_id taskid, const char *data)
  {
  return(NULL);
  }

int release_job_reservation(job *pjob)
  {
  fprintf(stderr, "The call to getsize needs to be mocked!!\n");
  exit(1);
  }

bool am_i_mother_superior(const job &pjob)
  {
  return(false);
  }

void get_energy_used(job *pjob)
  {}

int init_nvidia_nvml(unsigned int device_count) 
  {
  return(0);
  }

int check_nvidia_setup()
  {
  return(0);
  }

int shut_nvidia_nvml() 
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

int complete_req::get_task_stats(unsigned int &req_index, std::vector<int> &task_index,
                                 std::vector<unsigned long> &cput_used,
                                 std::vector<unsigned long long> &mem_used, const char *hostname)
  {
  return(0);
  }

task::~task() {}
