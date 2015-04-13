#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <errno.h>
#include <pthread.h>

#include "svr_movejob.h"
#include "test_svr_movejob.h"
#include "pbs_error.h"
#include "list_link.h"

int get_job_script_path(job *pjob, std::string &script_path);
int save_jobs_sid(char *jobid, long sid);
void log_commit_error(int con, int mom_err, char *job_id, bool &timeout);
int update_substate_if_needed(char *job_id, bool &change_substate_on_attempt_to_queue);
int queue_job_on_mom(int con, int *my_err, char *job_id, char *job_destin, tlist_head &attrl, bool &timeout, int type);
void encode_attributes(tlist_head &attrl, job *pjob, int resc_access_perm, int encode_type);
int encode_exec_host(pbs_attribute *, tlist_head *, const char *, const char *, int, int);
int send_job_script_if_needed(int con, bool need_to_send_job_script, const char *script_name, char *job_id);
int send_files_if_needed(int con, char *job_id, int type, bool job_has_run, unsigned long job_momaddr, char *stdout_path, char *stderr_path, char *chkpt_path);
int attempt_to_queue_job_on_mom(char *job_id, int con, char *job_destin, bool &change_substate_on_attempt_to_queue, tlist_head &attrl, bool &timeout, bool need_to_send_job_script, bool job_has_run, unsigned long job_momaddr, const char *script_name, char *stdout_path, char *stderr_path, char *chkpt_path, int type, int *my_err);
int commit_job_on_mom(int con, char *job_id, bool &timeout,int *mom_err);
int send_job_over_network(char *job_id, int con, char *job_destin, tlist_head &attrl, bool &attempt_to_queue_job, bool &change_substate_on_attempt_to_queue, bool &timeout, const char *script_name, bool need_to_send_job_script, bool job_has_run, unsigned long job_momaddr, char *stdout_path, char *stderr_path, char *chkpt_path, int type, int *my_err,int *mom_err);
int send_job_over_network_with_retries(char *job_id, char *job_destin, tlist_head &attrl, bool &attempt_to_queue_job, bool &change_subtate_on_attempt_to_queue, bool &timeout, const char *script_name, bool need_to_send_job_script, bool job_has_run, unsigned long momaddr, unsigned short momport, char *stdout_path, char *stderr_path, char *chkpt_path, int type, int *my_err, int *mom_err);

bool cpy_stdout_err_on_rerun = false;

extern bool get_jobs_array_recycled;
extern bool get_jobs_array_fail;
extern bool job_exist;
extern bool expired;
extern bool other_fail;
extern bool script_fail;
extern bool jobfile_fail;
extern bool commit_error;
extern bool rdycommit_fail;
extern attribute_def job_attr_def[];
extern int  retry;
extern bool connect_fail;

START_TEST(send_job_over_network_with_retries_test)
  {
  bool timeout = false;
  char *jobid = strdup("1.napali");
  char *destin = strdup("bob");
  bool attempt_to_queue = true;
  bool c = true;
  tlist_head h;
  int my_err;
  int mom_err;
  
  return;

  connect_fail = true;
  fail_unless(send_job_over_network_with_retries(jobid, destin, h, attempt_to_queue, c, timeout, "script", true, false, 10, 10, strdup("/out"), strdup("/err"), strdup("chkpt"), MOVE_TYPE_Exec, &my_err,&mom_err) == LOCUTION_FAIL);

  retry = 1;
  fail_unless(send_job_over_network_with_retries(jobid, destin, h, attempt_to_queue, c, timeout, "script", true, false, 10, 10, strdup("/out"), strdup("/err"), strdup("chkpt"), MOVE_TYPE_Exec, &my_err,&mom_err) == PBSE_NONE);

  retry = 10;
  fail_unless(send_job_over_network_with_retries(jobid, destin, h, attempt_to_queue, c, timeout, "script", true, false, 10, 10, strdup("/out"), strdup("/err"), strdup("chkpt"), MOVE_TYPE_Exec, &my_err,&mom_err) == LOCUTION_RETRY);

  }
END_TEST

START_TEST(send_job_over_network_test)
  {
  bool timeout = false;
  char *jobid = strdup("1.napali");
  char *destin = strdup("bob");
  bool attempt_to_queue = true;
  bool c = true;
  tlist_head h;
  int my_err;
  int mom_err;

  CLEAR_HEAD(h);

  fail_unless(send_job_over_network(strdup("2.napali"), 5, destin, h, attempt_to_queue, c, timeout, "script", true, false, 10, strdup("/out"), strdup("/err"), strdup("/chkpt"), MOVE_TYPE_Exec, &my_err,&mom_err) == LOCUTION_FAIL);

  CLEAR_HEAD(h);

  fail_unless(send_job_over_network(jobid, 5, destin, h, attempt_to_queue, c, timeout, "script", true, false, 10, strdup("/out"), strdup("/err"), strdup("/chkpt"), MOVE_TYPE_Exec, &my_err,&mom_err) == PBSE_NONE);
  fail_unless(attempt_to_queue == false);

  fprintf(stderr,"%p %s\n",(void *)destin,destin);
  CLEAR_HEAD(h);

  rdycommit_fail = true;
  fail_unless(send_job_over_network(jobid, 5, destin, h, attempt_to_queue, c, timeout, "script", true, false, 10, strdup("/out"), strdup("/err"), strdup("/chkpt"), MOVE_TYPE_Exec, &my_err,&mom_err) == LOCUTION_RETRY);
  rdycommit_fail = false;

  }
END_TEST

START_TEST(commit_job_on_mom_test);
  {
  bool timeout = false;
  char *jobid = strdup("1.napali");
  int  mom_err;

  fail_unless(commit_job_on_mom(5, jobid, timeout,&mom_err) == PBSE_NONE);
  commit_error = true;
  fail_unless(commit_job_on_mom(5, jobid, timeout,&mom_err) == LOCUTION_FAIL);
  commit_error = false;
  }
END_TEST

START_TEST(attempt_to_queue_job_on_mom_test)
  {
  bool b = true;
  bool timeout = true;
  tlist_head h;
  int  my_err;

  fail_unless(attempt_to_queue_job_on_mom(strdup("1.napali"), 5, strdup("napali"), b, h, timeout, true, true, 5, "script", strdup("/out"), strdup("/err"), strdup("/chkpt"), MOVE_TYPE_Exec, &my_err) == LOCUTION_SUCCESS);
  fail_unless(attempt_to_queue_job_on_mom(strdup("2.napali"), 5, strdup("napali"), b, h, timeout, true, true, 5, "script", strdup("/out"), strdup("/err"), strdup("/chkpt"), MOVE_TYPE_Exec, &my_err) == LOCUTION_FAIL);
  }
END_TEST

START_TEST(send_files_if_needed_test)
  {
  fail_unless(send_files_if_needed(5, strdup("1.napali"), MOVE_TYPE_Exec, false, 1, strdup("/out"), strdup("/err"), strdup("/chkpt")) == PBSE_NONE);
  fail_unless(send_files_if_needed(5, strdup("1.napali"), MOVE_TYPE_Exec, true, 1, strdup("/out"), strdup("/err"), strdup("/chkpt")) == PBSE_NONE);
  jobfile_fail = true;
  fail_unless(send_files_if_needed(5, strdup("1.napali"), MOVE_TYPE_Exec, true, 1, strdup("/out"), strdup("/err"), strdup("/chkpt")) != PBSE_NONE);
  jobfile_fail = false;
  }
END_TEST

START_TEST(send_job_script_if_needed_test)
  {
  script_fail = false;

  fail_unless(send_job_script_if_needed(4, false, strdup("bobo"), strdup("1.napali")) == PBSE_NONE);
  fail_unless(send_job_script_if_needed(4, true, strdup("bobo"), strdup("1.napali")) == PBSE_NONE);

  script_fail = true;
  fail_unless(send_job_script_if_needed(4, true, strdup("bobo"), strdup("1.napali")) != PBSE_NONE);
  script_fail = false;
  }
END_TEST

void init_job_attr_def()

  {
  for (int i = 0; i < JOB_ATR_LAST; i++)
    {
    job_attr_def[i].at_name = strdup("exec_host");
    job_attr_def[i].at_encode = encode_str;
    }

  job_attr_def[JOB_ATR_exec_host].at_flags |= ATR_DFLAG_MOM;
  job_attr_def[JOB_ATR_exec_host].at_encode = encode_exec_host;
  }

START_TEST(encode_attributes_test)
  {
  tlist_head attrl;
  job        pjob;

  memset(&pjob, 0, sizeof(pjob));

  CLEAR_HEAD(attrl);
  init_job_attr_def();

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("1.napali");
  pjob.ji_wattr[JOB_ATR_exec_host].at_flags |= ATR_VFLAG_SET;

  encode_attributes(attrl, &pjob, ATR_DFLAG_MOM, ATR_ENCODE_MOM);

  fail_unless(GET_NEXT(attrl) != NULL);
  }
END_TEST

START_TEST(queue_job_on_mom_test)
  {
  int my_err = 0;
  tlist_head attrl;
  bool       timeout = false;
  int        type = MOVE_TYPE_Exec;

  fail_unless(queue_job_on_mom(1, &my_err, strdup("1.napali"), strdup("batch"), attrl, timeout, type) == PBSE_NONE);
  fail_unless(timeout == false);
  
  expired = true;
  fail_unless(queue_job_on_mom(1, &my_err, strdup("1.napali"), strdup("batch"), attrl, timeout, type) == LOCUTION_RETRY);
  fail_unless(timeout == true);

  timeout = false;
  expired = false;
  job_exist = true;
  fail_unless(queue_job_on_mom(1, &my_err, strdup("1.napali"), strdup("batch"), attrl, timeout, type) == LOCUTION_DONE);

  job_exist = false;
  other_fail = true;
  fail_unless(queue_job_on_mom(1, &my_err, strdup("1.napali"), strdup("batch"), attrl, timeout, type) == LOCUTION_RETRY);
  other_fail = false;
  }
END_TEST

START_TEST(update_substate_if_needed_test)
  {
  bool change = false;
  fail_unless(update_substate_if_needed(strdup("2.napali"), change) == PBSE_NONE);
  change = true;
  fail_unless(update_substate_if_needed(strdup("1.napali"), change) == PBSE_NONE);
  fail_unless(update_substate_if_needed(strdup("2.napali"), change) != PBSE_NONE);
  }
END_TEST

START_TEST(log_commit_error_test)
  {
  bool timeout = false;

  connection[5].ch_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));

  errno = EINTR;
  log_commit_error(5, PBSE_TIMEOUT, strdup("2.napali"), timeout);
  fail_unless(timeout == false);

  errno = EINPROGRESS;
  log_commit_error(5, EINPROGRESS, strdup("3.napali"), timeout);
  fail_unless(timeout == true);
  }
END_TEST

START_TEST(get_job_script_path_test)
  {
  job         *pjob = (job *)calloc(1, sizeof(job));
  std::string  script;

  strcpy(pjob->ji_qs.ji_fileprefix, "1.napali");

  fail_unless(get_job_script_path(pjob, script) == PBSE_NONE);
  fail_unless(script == "/var/spool/torque/server_priv/jobs/1.napali.SC");

  pjob->ji_arraystructid[0] = 'a';
  fail_unless(get_job_script_path(pjob, script) == PBSE_NONE);
  fail_unless(script == "/var/spool/torque/server_priv/jobs/2.napali.SC");

  get_jobs_array_fail = true;
  fail_unless(get_job_script_path(pjob, script) == -1);
  get_jobs_array_fail = false;

  get_jobs_array_recycled = true;
  fail_unless(get_job_script_path(pjob, script) == PBSE_JOB_RECYCLED);
  get_jobs_array_recycled = false;
  }
END_TEST

START_TEST(save_jobs_sid_test)
  {
  fail_unless(save_jobs_sid(strdup("2.napali"), 10) != PBSE_NONE);
  fail_unless(save_jobs_sid(strdup("1.napali"), 10) == PBSE_NONE);
  }
END_TEST

Suite *svr_movejob_suite(void)
  {
  Suite *s = suite_create("svr_movejob_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, get_job_script_path_test);
  tcase_add_test(tc_core, log_commit_error_test);
  tcase_add_test(tc_core, update_substate_if_needed_test);
  tcase_add_test(tc_core, queue_job_on_mom_test);
  tcase_add_test(tc_core, encode_attributes_test);
  tcase_add_test(tc_core, send_job_script_if_needed_test);
  tcase_add_test(tc_core, send_files_if_needed_test);
  tcase_add_test(tc_core, attempt_to_queue_job_on_mom_test);
  tcase_add_test(tc_core, commit_job_on_mom_test);
  tcase_add_test(tc_core, send_job_over_network_test);
  tcase_add_test(tc_core, send_job_over_network_with_retries_test);
  tcase_set_timeout(tc_core, 12);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("save_jobs_sid_test");
  tcase_add_test(tc_core, save_jobs_sid_test);
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
  sr = srunner_create(svr_movejob_suite());
  srunner_set_log(sr, "svr_movejob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
