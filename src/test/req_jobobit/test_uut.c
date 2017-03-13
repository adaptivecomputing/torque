#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <vector>

#include "pbs_job.h"
#include "req_jobobit.h"
#include "test_uut.h"
#include "pbs_error.h"
#include "batch_request.h"
#include "sched_cmds.h"
#include "server.h"
#include "work_task.h"
#include "completed_jobs_map.h"
#include "resource.h"


char *setup_from(svr_job *pjob, const char *suffix);
int   is_joined(svr_job *pjob, enum job_atr ati);
batch_request *return_stdfile(batch_request *preq, svr_job *pjob, enum job_atr ati);
void rel_resc(svr_job *pjob);
int handle_exiting_or_abort_substate(svr_job *pjob);
int setrerun(svr_job *pjob,const char *text);
batch_request *setup_cpyfiles(batch_request *preq, svr_job *pjob, char *from, char *to, int direction, int tflag);
int handle_returnstd(svr_job *pjob, batch_request *preq, int type);
int mom_comm(svr_job *pjob, void *(*func)(struct work_task *vp));
int handle_complete_first_time(svr_job *pjob);
int handle_complete_second_time(struct work_task *ptask);
int handle_complete_subjob(svr_job *pjob);
int handle_exited(svr_job *pjob);
int add_comment_to_parent(svr_job *parent_job, int cray_subjob_exited_nonzero, int exit_status);
int end_of_job_accounting(svr_job *pjob, std::string &acct_info, size_t accttail);
int handle_terminating_array_subjob(svr_job *pjob);
int handle_terminating_job(svr_job *pjob, int alreadymailed, const char *mailbuf);
int update_substate_from_exit_status(svr_job *pjob, int *alreadymailed, const char *text);
int handle_stageout(svr_job *pjob, int type, batch_request *preq);
int handle_stagedel(svr_job *pjob,int type,batch_request *preq);
int get_used(svr_job *pjob, std::string &data);
void set_job_comment(svr_job *pjob, const char *cmt);


extern pthread_mutex_t *svr_do_schedule_mutex;
extern pthread_mutex_t *listener_command_mutex;
extern int svr_do_schedule;
extern int listener_command;
extern completed_jobs_map_class completed_jobs_map;
int alloc_br_null;
extern struct server server;
extern int bad_connect;
extern int bad_job;
extern bool cray_enabled;
extern int double_bad;
extern int reported;
extern int bad_drequest;
extern int usage;
extern bool completed;
extern bool exited;
extern bool purged;
extern long disable_requeue;
extern int  attr_count;
extern int  next_count;
extern int  called_account_jobend;
extern std::string log_err_buf;


void init_server()
  {
  server.sv_attr_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(server.sv_attr_mutex, NULL);
  }


START_TEST(set_job_comment_test)
  {
  svr_job pjob;
  memset(&pjob, 0, sizeof(pjob));

  set_job_comment(&pjob, "bob");
  fail_unless(!strcmp(pjob.get_str_attr(JOB_ATR_Comment), "bob"));
  fail_unless(pjob.is_attr_set(JOB_ATR_Comment));
 
  // Overwrite, do not append
  set_job_comment(&pjob, "tim");
  fail_unless(!strcmp(pjob.get_str_attr(JOB_ATR_Comment), "tim"));
  fail_unless(pjob.is_attr_set(JOB_ATR_Comment));
  }
END_TEST


void add_resource(

  std::vector<resource> &resources,
  const char            *name,
  const char            *str_val,
  long                   lval1,
  long                   lval2)

  {
  resource_def *rd = (resource_def *)calloc(1, sizeof(resource_def));
  rd->rs_name = strdup(name);
  resource r;

  r.rs_defin = rd;
  
  if (str_val != NULL)
    {
    r.rs_value.at_val.at_str = strdup(str_val);
    }
  else if (lval2 < 0)
    r.rs_value.at_val.at_long = lval1;
  else
    {
    r.rs_value.at_val.at_size.atsv_num = lval1;
    r.rs_value.at_val.at_size.atsv_shift = lval2;
    }

  resources.push_back(r);
  } // add_resource()


START_TEST(get_used_test)
  {
  std::string data;
  svr_job     pjob;

  attr_count = 0;
  next_count = 0;

  std::vector<resource> *resources = new std::vector<resource>();
  add_resource(*resources, "cput", NULL, 100, -1);
  add_resource(*resources, "mem", NULL, 4096, 20);
  add_resource(*resources, "vmem", NULL, 8192, 20);
  pjob.set_resc_attr(JOB_ATR_resc_used, resources);

  fail_unless(get_used(&pjob, data) == PBSE_NONE);
  fail_unless(data == " resources_used.cput=100 resources_used.mem=4096mb resources_used.vmem=8192mb", "'%s'", data.c_str());
  }
END_TEST


START_TEST(handle_stagedel_test)
  {
  svr_job pjob;

  pjob.set_str_attr(JOB_ATR_exec_host, strdup("SherrieJWD"));

  // test to be sure that NULL exec hosts doesn't cause a segfault
  fail_unless(handle_stagedel(&pjob, WORK_Immed, NULL) == PBSE_NONE);
  }
END_TEST



START_TEST(handle_stageout_test)
  {
  svr_job pjob;

  // test to be sure that NULL exec hosts doesn't cause a segfault
  fail_unless(handle_stageout(&pjob, WORK_Immed, NULL) != PBSE_NONE);
  }
END_TEST


START_TEST(setup_from_test)
  {
  svr_job   pjob;
  char *str;

  pjob.set_fileprefix("bob");
  str = setup_from(&pjob, "by");
  fail_unless(!strcmp(str, "bobby"));
  }
END_TEST




START_TEST(is_joined_test)
  {
  svr_job   pjob;

  pjob.set_str_attr(JOB_ATR_join, strdup("oe"));
  fail_unless(is_joined(&pjob, JOB_ATR_exec_host) == 0);
  fail_unless(is_joined(&pjob, JOB_ATR_outpath) == 0);
  fail_unless(is_joined(&pjob, JOB_ATR_errpath) == 1);

  pjob.set_str_attr(JOB_ATR_join, strdup("e"));
  fail_unless(is_joined(&pjob, JOB_ATR_outpath) == 0);

  pjob.set_str_attr(JOB_ATR_join, strdup("o"));
  fail_unless(is_joined(&pjob, JOB_ATR_errpath) == 0);
  }
END_TEST




START_TEST(return_stdfile_test)
  {
  svr_job        pjob;
  batch_request  preq;
  batch_request *p1 = &preq;
  batch_request *p2;

  pjob.set_attr(JOB_ATR_checkpoint_name);

  fail_unless(return_stdfile(&preq, &pjob, JOB_ATR_outpath) != NULL);
  fail_unless(return_stdfile(&preq, &pjob, JOB_ATR_errpath) != NULL);

  pjob.set_str_attr(JOB_ATR_join, strdup("oe"));

  fail_unless((p2 = return_stdfile(&preq, &pjob, JOB_ATR_errpath)) == p1);
  fail_unless((p2 = return_stdfile(NULL, &pjob, JOB_ATR_outpath)) != NULL);
  fail_unless(p2->rq_type == PBS_BATCH_ReturnFiles);

  pjob.unset_attr(JOB_ATR_checkpoint_name);
  fail_unless(return_stdfile(&preq, &pjob, JOB_ATR_outpath) == NULL);

  pjob.set_long_attr(JOB_ATR_interactive, 1);
  fail_unless(return_stdfile(&preq, &pjob, JOB_ATR_outpath) == NULL);
  }
END_TEST




START_TEST(rel_resc_test)
  {
  svr_job pjob;

  rel_resc(&pjob);
  fail_unless(svr_do_schedule == SCH_SCHEDULE_TERM);
  fail_unless(listener_command == SCH_SCHEDULE_TERM);
  }
END_TEST




START_TEST(handle_exiting_or_abort_substate_test)
  {
  svr_job            pjob;

  fail_unless(handle_exiting_or_abort_substate(&pjob) == PBSE_NONE);
  fail_unless(pjob.get_state() == JOB_STATE_EXITING);
  fail_unless(pjob.get_substate() == JOB_SUBSTATE_RETURNSTD);

  pjob.set_attr(JOB_ATR_depend);
  fail_unless(handle_exiting_or_abort_substate(&pjob) == PBSE_NONE);
  }
END_TEST




START_TEST(setrerun_test)
  {
  svr_job pjob;

  fail_unless(setrerun(&pjob,NULL) != PBSE_NONE);
  pjob.set_bool_attr(JOB_ATR_rerunable, true);
  fail_unless(setrerun(&pjob,"rerunner") == PBSE_NONE);
  fail_unless(pjob.get_substate() == JOB_SUBSTATE_RERUN);
  }
END_TEST




START_TEST(setup_cpyfiles_test)
  {
  svr_job           *pjob = new svr_job();
  batch_request *preq;
  alloc_br_null = 0;

  pjob->set_jobid("1.napali");
  preq = setup_cpyfiles(NULL, pjob, strdup("from"), strdup("to"), 1, 1);
  fail_unless(preq != NULL);

  pjob->set_str_attr(JOB_ATR_egroup, strdup("dbeer"));
  preq = setup_cpyfiles(NULL, pjob, strdup("from"), strdup("to"), 1, 1);

  preq = setup_cpyfiles(preq, pjob, strdup("from"), strdup("to"), 1, 1);
  fail_unless(preq != NULL);
  }
END_TEST




START_TEST(handle_returnstd_test)
  {
  batch_request *preq;
  svr_job       *pjob;

  pjob = new svr_job();
  preq = new batch_request();

  pjob->set_jobid("1.napali");
  pjob->set_fileprefix("1.napali");
  
  fail_unless(handle_returnstd(pjob, preq, WORK_Immed) == PBSE_JOB_FILE_CORRUPT);
  
  pjob->set_str_attr(JOB_ATR_exec_host, strdup("napali/0"));
  fail_unless(handle_returnstd(pjob, preq, WORK_Deferred_Reply) == PBSE_NONE);
  
  // preq may have been deleted on the last run
  preq = new batch_request();
  fail_unless(handle_returnstd(pjob, preq, WORK_Immed) == PBSE_NONE);
  }
END_TEST




START_TEST(mom_comm_test)
  {
  svr_job pjob;

  pjob.set_jobid("1.napali");
  pjob.set_str_attr(JOB_ATR_exec_host, strdup("napali/0+napali/1"));

  fail_unless(mom_comm(&pjob, NULL) >= 0);
  
  /* set some variables for error cases */
  bad_connect = 1;
  cray_enabled = true;
  fail_unless(mom_comm(&pjob, NULL) == -1);
  bad_job = 1;
  fail_unless(mom_comm(&pjob, NULL) == -1 * PBSE_JOB_RECYCLED);

  bad_connect = 0;
  cray_enabled = false;
  bad_job = 0;
  }
END_TEST




START_TEST(handle_complete_first_time_test)
  {
  svr_job pjob;

  pjob.set_jobid("1.napali");

  fail_unless(handle_complete_first_time(&pjob) == 0);
  cray_enabled = true;
  double_bad = 1;

  fail_unless(handle_complete_first_time(&pjob) == PBSE_JOBNOTFOUND);

  double_bad = 0;
  fail_unless(handle_complete_first_time(&pjob) == 0);

  pjob.set_substate(JOB_SUBSTATE_COMPLETE);
  pjob.set_attr(JOB_ATR_comp_time);
  fail_unless(handle_complete_first_time(&pjob) == 0);
  }
END_TEST




START_TEST(handle_complete_second_time_test)
  {
  struct work_task *ptask;
  ptask = (work_task *)calloc(1, sizeof(*ptask));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  handle_complete_second_time(ptask);
  
  ptask = (work_task *)calloc(1, sizeof(*ptask));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");
  handle_complete_second_time(ptask);

  bad_job = 1;
  ptask = (work_task *)calloc(1, sizeof(*ptask));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");
  handle_complete_second_time(ptask);

  bad_job = 0;
  reported = 1;
  ptask = (work_task *)calloc(1, sizeof(*ptask));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");
  handle_complete_second_time(ptask);

  completed = false;
  purged = false;
  ptask = (work_task *)calloc(1, sizeof(*ptask));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");
  handle_complete_second_time(ptask);
  fail_unless(purged == false);
  
  completed = true;
  ptask = (work_task *)calloc(1, sizeof(*ptask));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");
  handle_complete_second_time(ptask);
  fail_unless(purged == true);
  completed = false;

  completed = false;
  exited = true;
  ptask = (work_task *)calloc(1, sizeof(*ptask));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");
  handle_complete_second_time(ptask);
  fail_unless(purged == true);
  }
END_TEST




START_TEST(handle_complete_subjob_test)
  {
  svr_job *parent   = new svr_job();
  svr_job *cray     = new svr_job();
  svr_job *external = new svr_job();

  parent->set_jobid("1.napali");
  svr_do_schedule_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  listener_command_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(svr_do_schedule_mutex, NULL);
  pthread_mutex_init(listener_command_mutex, NULL);

  cray->ji_parent_job = parent;
  external->ji_parent_job = parent;
  parent->ji_cray_clone = cray;
  parent->ji_external_clone = external;

  fail_unless(handle_complete_subjob(cray) == PBSE_NONE);
  fail_unless(parent->get_state() != JOB_STATE_COMPLETE);

  printf("final\n");

  double_bad = 1;
  cray->set_state(JOB_STATE_COMPLETE);
  fail_unless(handle_complete_subjob(external) == PBSE_NONE);
  fail_unless(parent->get_state() == JOB_STATE_COMPLETE);
  fail_unless(handle_complete_subjob(external) == PBSE_NONE);
  }
END_TEST




START_TEST(handle_exited_test)
  {
  svr_job *pjob = new svr_job();

  pjob->set_jobid("1.napali");
  pjob->set_str_attr(JOB_ATR_exec_host, strdup("napali/0+napali/1"));
  svr_do_schedule_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  listener_command_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(svr_do_schedule_mutex, NULL);
  pthread_mutex_init(listener_command_mutex, NULL);

  double_bad = 0;
  fail_unless(handle_exited(pjob) == PBSE_NONE);

  double_bad = 1;
  fail_unless(handle_exited(pjob) == PBSE_JOBNOTFOUND);
  double_bad = 0;

  bad_job = TRUE;
  fail_unless(handle_exited(pjob) == PBSE_CONNECT);
  bad_drequest = TRUE;
  fail_unless(handle_exited(pjob) == PBSE_CONNECT);
  alloc_br_null = TRUE;
  }
END_TEST




START_TEST(add_comment_to_parent_test)
  {
  svr_job *pjob = new svr_job();

  pjob->set_jobid("1.napali");
  pjob->set_str_attr(JOB_ATR_Comment, strdup("napali/0+napali/1"));
  fail_unless(add_comment_to_parent(pjob, TRUE, 12) == PBSE_NONE);
  fail_unless(add_comment_to_parent(pjob, FALSE, 12) == PBSE_NONE);
  }
END_TEST




START_TEST(end_of_job_accounting_test)
  {
  char *str = strdup("bob tom");
  std::string acct_data(str);
  svr_job  *pjob = new svr_job();
  pjob->set_jobid("1.napali");
  size_t accttail = acct_data.length();
  fail_unless(end_of_job_accounting(pjob, acct_data, accttail) == PBSE_NONE);
  usage = 1;
  fail_unless(end_of_job_accounting(pjob, acct_data, accttail) == PBSE_NONE);
  fail_unless(called_account_jobend == false);

  // We need to also set a start time to call this.
  pjob->set_start_time(1);
  fail_unless(end_of_job_accounting(pjob, acct_data, accttail) == PBSE_NONE);
  fail_unless(called_account_jobend == true);
  // Make sure this is set - should cause the next call to not execute
  fail_unless((pjob->get_svrflags() & JOB_ACCOUNTED_FOR) != 0);
  
  // Make sure that we'll do end of job accounting for jobs that are deleted while running
  // First call doesn't do account_jobend due to svrflags
  pjob->ji_being_deleted = true;
  called_account_jobend = false;
  fail_unless(end_of_job_accounting(pjob, acct_data, accttail) == PBSE_NONE);
  fail_unless(called_account_jobend == false);

  pjob->set_svrflags(0);
  fail_unless(end_of_job_accounting(pjob, acct_data, accttail) == PBSE_NONE);
  fail_unless(called_account_jobend == true);
  
  usage = 0;
  }
END_TEST




START_TEST(handle_terminating_array_subjob_test)
  {
  svr_job  *pjob = new svr_job();
  pjob->set_jobid("1[1].napali");
  strcpy(pjob->ji_arraystructid, "1[].napali");

  double_bad = 1;
  fail_unless(handle_terminating_array_subjob(pjob) == PBSE_UNKJOBID);

  double_bad = 0;
  bad_job = 0;
  fail_unless(handle_terminating_array_subjob(pjob) == PBSE_NONE);

  bad_job = 1;
  fail_unless(handle_terminating_array_subjob(pjob) == PBSE_UNKJOBID);
  bad_job = 0;
  }
END_TEST


START_TEST(handle_rerunning_array_subjob_test)
  {
  svr_job  *pjob = new svr_job();
  pjob->set_jobid("1[1].napali");
  strcpy(pjob->ji_arraystructid, "1[].napali");
  
  double_bad = 1;
  fail_unless(handle_terminating_array_subjob(pjob) == PBSE_UNKJOBID);

  double_bad = 0;
  bad_job = 0;
  fail_unless(handle_terminating_array_subjob(pjob) == PBSE_NONE);

  bad_job = 1;
  fail_unless(handle_terminating_array_subjob(pjob) == PBSE_UNKJOBID);
  }
END_TEST


START_TEST(handle_terminating_job_test)
  {
  svr_job  *pjob = new svr_job();

  bad_job = 0;
  pjob->set_jobid("1.napali");
  pjob->set_attr(JOB_ATR_restart_name);
  fail_unless(handle_terminating_job(pjob, 0, "bob") == PBSE_NONE);
  }
END_TEST




START_TEST(update_substate_from_exit_status_test)
  {
  svr_job  *pjob = new svr_job();
  int   alreadymailed = 0;

  pjob->set_bool_attr(JOB_ATR_rerunable, true);
  pjob->set_jobid("1.napali");

  pjob->set_exec_exitstat(-1000);
  cray_enabled = false;
  usage = 0;
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,"Some random message") == PBSE_NONE);
  fail_unless(pjob->get_substate() == JOB_SUBSTATE_RERUN1);
  fail_unless(alreadymailed == 0);

  pjob->set_substate(JOB_SUBSTATE_RUNNING);
  pjob->set_exec_exitstat(JOB_EXEC_OVERLIMIT_MEM);
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,"Some random message") == PBSE_NONE);
  fail_unless(pjob->get_substate() == JOB_SUBSTATE_RUNNING);
  fail_unless(alreadymailed == 1);

  alreadymailed = 0;
  pjob->set_substate(JOB_SUBSTATE_RUNNING);
  pjob->set_exec_exitstat(JOB_EXEC_FAIL1);
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,NULL) == PBSE_NONE);
  fail_unless(pjob->get_substate() == JOB_SUBSTATE_RUNNING);
  fail_unless(alreadymailed == 1);

  alreadymailed = 0;
  pjob->set_substate(JOB_SUBSTATE_RUNNING);
  pjob->set_exec_exitstat(JOB_EXEC_INITABT);
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,"Some random message") == PBSE_NONE);
  fail_unless(pjob->get_substate() == JOB_SUBSTATE_RERUN);
  fail_unless((pjob->get_svrflags() & JOB_SVFLG_HASRUN) != 0);

  alreadymailed = 0;
  pjob->set_substate(JOB_SUBSTATE_RUNNING);
  pjob->set_exec_exitstat(JOB_EXEC_RETRY);
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,"\n\n\n\n\n\n\n\n\n") == PBSE_NONE);
  fail_unless(pjob->get_substate() == JOB_SUBSTATE_RERUN);
  pjob->set_svrflags(0);
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,"Some random message") == PBSE_NONE);
  fail_unless(pjob->get_substate() == JOB_SUBSTATE_RERUN1);
  
  // make sure disabling auto requeuing works
  pjob->set_substate(JOB_SUBSTATE_RUNNING);
  pjob->set_exec_exitstat(JOB_EXEC_RETRY);
  disable_requeue = 1;
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,NULL) == PBSE_NONE);
  fail_unless(pjob->get_substate() == JOB_SUBSTATE_RUNNING);
  disable_requeue = 0;


  alreadymailed = 0;
  pjob->set_substate(JOB_SUBSTATE_RUNNING);
  pjob->set_exec_exitstat(JOB_EXEC_BADRESRT);
  pjob->set_svrflags(JOB_SVFLG_CHECKPOINT_FILE);
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,"Some random message") == PBSE_NONE);
  fail_unless(pjob->get_substate() == JOB_SUBSTATE_RERUN);
  fail_unless((pjob->get_svrflags() & JOB_SVFLG_CHECKPOINT_FILE) == 0);

  alreadymailed = 0;
  pjob->set_substate(JOB_SUBSTATE_RUNNING);
  pjob->set_exec_exitstat(JOB_EXEC_INITRST);
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,"Some random message") == PBSE_SYSTEM);
  fail_unless(pjob->ji_momhandle == -1);
  fail_unless((pjob->get_svrflags() & JOB_SVFLG_HASRUN) != 0);
  fail_unless((pjob->get_svrflags() & JOB_SVFLG_CHECKPOINT_FILE) != 0);

  alreadymailed = 0;
  pjob->set_substate(JOB_SUBSTATE_RUNNING);
  pjob->set_exec_exitstat(JOB_EXEC_INITRMG);
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,"Some random message") == PBSE_NONE);
  fail_unless((pjob->get_svrflags() & JOB_SVFLG_HASRUN) != 0);
  fail_unless((pjob->get_svrflags() & JOB_SVFLG_CHECKPOINT_MIGRATEABLE) != 0);
  
  alreadymailed = 0;
  // The substate should stay the same when the job is being deleted
  pjob->ji_being_deleted = true;
  pjob->set_substate(JOB_SUBSTATE_RUNNING);
  pjob->set_exec_exitstat(JOB_EXEC_RETRY);
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,"Some random message") == PBSE_NONE);
  fail_unless(pjob->get_substate() == JOB_SUBSTATE_RUNNING,
              "Shouldn't update substate when job is being deleted");
  
  pjob->ji_being_deleted = false;
  pjob->set_substate(JOB_SUBSTATE_RUNNING);
  pjob->set_exec_exitstat(JOB_EXEC_RETRY_PROLOGUE);
  fail_unless(update_substate_from_exit_status(pjob, &alreadymailed,"Some random message") == PBSE_NONE);
  fail_unless(pjob->get_substate() == JOB_SUBSTATE_RERUN);
  fail_unless(log_err_buf.find("prologue") != std::string::npos);
  }
END_TEST




Suite *req_jobobit_suite(void)
  {
  Suite *s = suite_create("req_jobobit_suite methods");
  TCase *tc_core = tcase_create("req_jobobit_tests");
  tcase_add_test(tc_core, setup_from_test);
  tcase_add_test(tc_core, is_joined_test);
  tcase_add_test(tc_core, return_stdfile_test);
  tcase_add_test(tc_core, rel_resc_test);
  tcase_add_test(tc_core, handle_exiting_or_abort_substate_test);
  tcase_add_test(tc_core, setrerun_test);
  tcase_add_test(tc_core, setup_cpyfiles_test);
  tcase_add_test(tc_core, handle_returnstd_test);
  tcase_add_test(tc_core, mom_comm_test);
  tcase_add_test(tc_core, handle_complete_first_time_test);
  tcase_add_test(tc_core, handle_complete_second_time_test);
  tcase_add_test(tc_core, handle_complete_subjob_test);
  tcase_add_test(tc_core, handle_exited_test);
  tcase_add_test(tc_core, add_comment_to_parent_test);
  tcase_add_test(tc_core, end_of_job_accounting_test);
  tcase_add_test(tc_core, handle_terminating_array_subjob_test);
  tcase_add_test(tc_core, handle_rerunning_array_subjob_test);
  tcase_add_test(tc_core, handle_terminating_job_test);
  tcase_add_test(tc_core, handle_stageout_test);
  tcase_add_test(tc_core, update_substate_from_exit_status_test);
  tcase_add_test(tc_core, handle_stagedel_test);
  tcase_add_test(tc_core, get_used_test);
  tcase_add_test(tc_core, set_job_comment_test);
  suite_add_tcase(s, tc_core);

  return(s);
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  svr_do_schedule_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  listener_command_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(svr_do_schedule_mutex, NULL);
  pthread_mutex_init(listener_command_mutex, NULL);
  SRunner *sr = NULL;
  init_server();
  rundebug();
  sr = srunner_create(req_jobobit_suite());
  srunner_set_log(sr, "req_jobobit_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
