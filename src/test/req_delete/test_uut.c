#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "pbs_error.h"
#include "pbs_job.h"
#include "req_delete.h"
#include "test_req_delete.h"
#include "server.h"
#include "work_task.h"

#define PURGE_SUCCESS 1

int delete_inactive_job(job **, const char *);
void force_purge_work(job *pjob);
int delete_all_work(batch_request *);
void ensure_deleted(struct work_task *ptask);
int  apply_job_delete_nanny(job *pjob, int delay);
void job_delete_nanny(struct work_task *ptask);
void post_job_delete_nanny(batch_request *preq_sig);
int forced_jobpurge(job *pjob, batch_request *preq);
void post_delete_mom2(struct work_task *pwt);
int handle_delete_all(batch_request *preq, char *Msg);
int handle_single_delete(batch_request *preq, char *Msg);
int perform_job_delete_array_bookkeeping(job *pjob, int cancel_exit_code);
bool exit_called;
extern int  depend_term_called;
extern long keep_seconds;
extern int bad_queue;
extern int bad_relay;
extern int signal_issued;
extern int nanny;
extern bool br_freed;
extern int alloc_work;
extern int  updated_array_values;
extern bool find_job_fail;
struct server server;
extern const char *delpurgestr;

char server_host[PBS_MAXHOSTNAME + 1];
time_t pbs_tcp_timeout = 300;
time_t apply_job_delete_nanny_time = 0;

extern all_jobs  alljobs;

int set_pbs_server_name()
  {
  struct addrinfo hints, *info;
  int gai_result;

  char hostname[1024];
  hostname[1023] = '\0';
  gethostname(hostname, 1023);

  memset(&hints, 0, sizeof hints);
  hints.ai_flags = AI_CANONNAME;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
    perror("couldn't getaddrinfo");
    perror(hostname);
    fprintf(stderr, "%s\n", gai_strerror(gai_result));
    return -1;
  }

  snprintf(server_host, sizeof(server_host), "%s", info->ai_canonname);

  freeaddrinfo(info);
  return 0;
  }

START_TEST(test_perform_job_delete_array_bookkeeping)
  {
  job           *pjob = (job *)calloc(1, sizeof(job));
  sprintf(pjob->ji_arraystructid, "1[].roshar");
  sprintf(pjob->ji_qs.ji_jobid, "1[0].roshar");

  find_job_fail = false;
  updated_array_values = 0;
  fail_unless(perform_job_delete_array_bookkeeping(pjob, 1) == PBSE_NONE);
  fail_unless(updated_array_values == 1);
  
  find_job_fail = true;
  fail_unless(perform_job_delete_array_bookkeeping(pjob, 1) != PBSE_NONE);
  fail_unless(updated_array_values == 2);
  }
END_TEST

START_TEST(test_handle_single_delete)
  {
  batch_request *preq = new batch_request();
  strcpy(preq->rq_ind.rq_delete.rq_objname, "2.napali");
  fail_unless(handle_single_delete(preq, NULL) == PBSE_NONE);
  fail_unless(preq->rq_noreply == FALSE);

  // Now an asynchronous delete
  preq->rq_extend = strdup(DELASYNC);
  strcpy(preq->rq_ind.rq_delete.rq_objname, "1.napali");
  fail_unless(handle_single_delete(preq, NULL) == PBSE_NONE);
  fail_unless(preq->rq_noreply == true);
  }
END_TEST

START_TEST(test_handle_delete_all)
  {
  batch_request preq;
  preq.rq_extend = strdup(DELASYNC);

  fail_unless(handle_delete_all(&preq, NULL) == PBSE_NONE);
  fail_unless(preq.rq_noreply == true);
  }
END_TEST

START_TEST(test_post_delete_mom2)
  {
  struct work_task *ptask;

  ptask = (struct work_task *)calloc(1, sizeof(struct work_task));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  signal_issued = FALSE;
  post_delete_mom2(ptask);
  fail_unless(signal_issued == FALSE);

  signal_issued = FALSE;
  ptask = (struct work_task *)calloc(1, sizeof(struct work_task));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");
  post_delete_mom2(ptask);
  fail_unless(signal_issued == TRUE);
  }
END_TEST

START_TEST(test_forced_jobpurge)
  {
  job           *pjob;
  batch_request *preq;

  pjob = new job();
  preq = new batch_request();
  strcpy(pjob->ji_qs.ji_jobid, "1.napali");
  memset(pjob->ji_arraystructid, 0, sizeof(pjob->ji_arraystructid));

  preq->rq_extend = strdup(delpurgestr);
  nanny = 0;
  fail_unless(forced_jobpurge(pjob, preq) == -1);

  preq->rq_perm = ATR_DFLAG_MGRD;
  fail_unless(forced_jobpurge(pjob, preq) == PURGE_SUCCESS);
 
  preq->rq_extend = NULL;
  fail_unless(forced_jobpurge(pjob, preq) == PBSE_NONE);

  nanny = 1;
  }
END_TEST

START_TEST(test_delete_all_work)
  {
  //struct all_jobs  alljobs;
  job           *pjob = new job();
  batch_request *preq = new batch_request();
  preq->rq_extend = strdup(delpurgestr);
  nanny = 0;

  insert_job(&alljobs, pjob);

  // no lock should remain on job after delete_all_work() 
  //  so test by making sure we can set lock 
  fail_unless(delete_all_work(preq) == PBSE_NONE);
  fail_unless(pthread_mutex_trylock(pjob->ji_mutex) == 0);

  nanny = 1;
  }
END_TEST

START_TEST(test_post_job_delete_nanny)
  {
  batch_request *preq_sig;
  
  preq_sig = new batch_request();
  nanny = 0;
  post_job_delete_nanny(preq_sig);

  nanny = 1;
  strcpy(preq_sig->rq_ind.rq_signal.rq_jid, "2.napali");
  post_job_delete_nanny(preq_sig);

  nanny = 1;
  strcpy(preq_sig->rq_ind.rq_signal.rq_jid, "1.napali");
  post_job_delete_nanny(preq_sig);

  nanny = 1;
  strcpy(preq_sig->rq_ind.rq_signal.rq_jid, "1.napali");
  preq_sig->rq_reply.brp_code = PBSE_UNKJOBID;
  post_job_delete_nanny(preq_sig);
  }
END_TEST

START_TEST(test_job_delete_nanny)
  {
  struct work_task *ptask;
  ptask = (struct work_task *)calloc(1, sizeof(struct work_task));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");

  nanny = 0;
  job_delete_nanny(ptask);
  fail_unless(signal_issued == FALSE);

  ptask = (struct work_task *)calloc(1, sizeof(struct work_task));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");
  nanny = 1;
  job_delete_nanny(ptask);
  fail_unless(signal_issued == TRUE);
  }
END_TEST

START_TEST(test_apply_job_delete_nanny)
  {
  job *pjob = new job();

  fail_unless(apply_job_delete_nanny(pjob, -1) == -1);
  fail_unless(pjob->ji_has_delete_nanny == FALSE);
  fail_unless(apply_job_delete_nanny(pjob, 0) == PBSE_NONE);
  fail_unless(pjob->ji_has_delete_nanny == TRUE);
  pjob->ji_has_delete_nanny = FALSE;
  fail_unless(apply_job_delete_nanny(pjob, 10) == PBSE_NONE);
  fail_unless(pjob->ji_has_delete_nanny == TRUE);

  fail_unless(apply_job_delete_nanny(pjob, 1) == PBSE_NONE);
  }
END_TEST 

START_TEST(test_ensure_deleted)
  {
  struct work_task *ptask = (struct work_task *)calloc(1, sizeof(struct work_task));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");

  ensure_deleted(ptask);
  }
END_TEST 

START_TEST(test_delete_inactive_job)
  {
  job *pjob = new job();

  fail_unless(delete_inactive_job((job **)NULL, NULL) == PBSE_BAD_PARAMETER);
  
  keep_seconds = 10;
  fail_unless(delete_inactive_job(&pjob, NULL) == PBSE_NONE);
  fail_unless(pjob->ji_qs.ji_state == JOB_STATE_COMPLETE);

  pjob = new job();
  pjob->ji_qs.ji_state = JOB_STATE_QUEUED;
  bad_queue = 1;
  fail_unless(delete_inactive_job(&pjob, NULL) == PBSE_NONE);
  fail_unless(pjob->ji_qs.ji_state == JOB_STATE_COMPLETE);
  bad_queue = 0;

  pjob = new job();
  pjob->ji_qs.ji_svrflags |= JOB_SVFLG_CHECKPOINT_FILE;
  fail_unless(delete_inactive_job(&pjob, NULL) == PBSE_NONE);
  fail_unless(pjob->ji_qs.ji_state == JOB_STATE_EXITING);
  fail_unless(pjob->ji_momhandle = -1);

  pjob = new job();
  pjob->ji_qs.ji_svrflags = JOB_SVFLG_StagedIn;
  fail_unless(delete_inactive_job(&pjob, NULL) == PBSE_NONE);
  fail_unless(pjob == NULL);

  pjob = new job();
  bad_relay = 1;
  pjob->ji_qs.ji_svrflags = JOB_SVFLG_StagedIn;
  fail_unless(delete_inactive_job(&pjob, NULL) == PBSE_NONE);
  fail_unless(pjob == NULL);
  bad_relay = 0;
  }
END_TEST 

START_TEST(test_force_purge_work)
  {
  job *pjob = new job();

  sprintf(pjob->ji_qs.ji_jobid, "17.roshar");
  pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("bob");
  depend_term_called = 0;
  force_purge_work(pjob);
  // normally pjob wouldn't be valid at this point, but I've made the functions 
  // that free set these values in the scaffolding so we can test what happened 
  fail_unless(pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str == NULL);
  fail_unless(pjob->ji_qs.ji_state == JOB_STATE_COMPLETE);
  fail_unless(depend_term_called > 0);
  }
END_TEST 

START_TEST(test_is_ms_on_server)
  {
  int rc = set_pbs_server_name();
  if (rc != 0) {
    fprintf(stderr, "test_is_ms_on_server() can't run because test harness can't be constructed reliably. Skipping.\n");
    return;
  }

  job myjob;

  myjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(server_host);
  rc = is_ms_on_server(&myjob);
  fail_unless(rc != 0, "failed to detect mother superior is the same as the pbs_server");
  free(myjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str);

  myjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(server_host);
  char *p = strchr(myjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, '.');
  if (p)
	 *p = '\0';
  rc = is_ms_on_server(&myjob);
  fail_unless(rc != 0, "failed to detect mother superior is the same as the pbs_server");
  free(myjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str);

  myjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("bob");
  rc = is_ms_on_server(&myjob);
  fail_unless(rc == 0, "failed to detect mother superior is not the same as the pbs_server");
  free(myjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str);
  }
END_TEST 

START_TEST(test_setup_apply_job_delete_nanny)
  {
  time_t time_now = time(NULL);
  time_t call_time = time_now + 200;

  nanny = 0; /* nanny is being used by the other unit tests above */
  server.sv_attr[SRV_ATR_KeepCompleted].at_flags = ATR_VFLAG_SET;
  server.sv_attr[SRV_ATR_KeepCompleted].at_val.at_long = 200;

  server.sv_attr[SRV_ATR_JobNanny].at_flags = ATR_VFLAG_SET;
  server.sv_attr[SRV_ATR_JobNanny].at_val.at_long = 1;

  job myjob;

  nanny = 0; // don't use this hook as the other unit tests do
  setup_apply_job_delete_nanny(&myjob, time_now);
  fail_unless(apply_job_delete_nanny_time == call_time, 
              "apply_job_delete_nanny did not go off at the right time");

  //Now, let's test there was no keep_completed server option
  call_time = time_now + KEEP_COMPLETED_DEFAULT;
  server.sv_attr[SRV_ATR_KeepCompleted].at_flags = 0;
  server.sv_attr[SRV_ATR_KeepCompleted].at_val.at_long = 0;

  setup_apply_job_delete_nanny(&myjob, time_now);
  fail_unless(apply_job_delete_nanny_time != call_time,
    "A new job delete time shouldn't have been set since this job already has a nanny");

  // Pretending we don't have a nanny already should set a new time
  myjob.ji_has_delete_nanny = false;
  setup_apply_job_delete_nanny(&myjob, time_now);
  fail_unless(apply_job_delete_nanny_time == call_time, 
              "apply_job_delete_nanny did not go off at the right time when there was no keep_completed");
  }
END_TEST 

Suite *req_delete_suite(void)
  {
  Suite *s = suite_create("req_delete_suite methods");
  TCase *tc_core = tcase_create("tests");
  tcase_add_test(tc_core, test_delete_inactive_job);
  tcase_add_test(tc_core, test_force_purge_work);
  tcase_add_test(tc_core, test_delete_all_work);
  tcase_add_test(tc_core, test_ensure_deleted);
  tcase_add_test(tc_core, test_apply_job_delete_nanny);
  tcase_add_test(tc_core, test_post_job_delete_nanny);
  tcase_add_test(tc_core, test_job_delete_nanny);
  tcase_add_test(tc_core, test_forced_jobpurge);
  tcase_add_test(tc_core, test_post_delete_mom2);
  tcase_add_test(tc_core, test_is_ms_on_server);
  tcase_add_test(tc_core, test_setup_apply_job_delete_nanny);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("more");
  tcase_add_test(tc_core, test_handle_delete_all);
  tcase_add_test(tc_core, test_handle_single_delete);
  tcase_add_test(tc_core, test_perform_job_delete_array_bookkeeping);
  suite_add_tcase(s, tc_core);

  return s;
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  server.sv_attr_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  sr = srunner_create(req_delete_suite());
  srunner_set_log(sr, "req_delete_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
