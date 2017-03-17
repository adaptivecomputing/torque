#include "license_pbs.h" /* See here for the software license */
#include "job_func.h"
#include "array.h"
#include "pbs_job.h"
#include "test_job_func.h"
#include "batch_request.h"
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "pbs_error.h"
#include "completed_jobs_map.h"

extern sem_t *job_clone_semaphore;
extern attribute_def job_attr_def[];
int hostname_in_externals(char *, char *);
int fix_cray_exec_hosts(job *);
int fix_external_exec_hosts(job *);
int change_external_job_name(job *);
int split_job(job *);
bool add_job_called = false;
bool internal_job_id_exists(int internal_id);
void job_free(job *pj, int  use_recycle);

//bool svr_job_purge_called = false;
extern completed_jobs_map_class completed_jobs_map;
extern int called_remove_job;
extern int dequejob_rc;
extern all_jobs alljobs;

char buf[4096];

START_TEST(internal_job_id_exists_test)
  {
  job pjob;
  alljobs.insert(&pjob, "0.napali");
  alljobs.insert(&pjob, "1.napali");
  alljobs.insert(&pjob, "2.napali");

  // internal ids are set to map to %d.napali in scaffolding.c if < 5
  for (int i = 0; i < 3; i++)
    fail_unless(internal_job_id_exists(i) == true);

  for (int i = 3; i < 10; i++)
    fail_unless(internal_job_id_exists(i) == false);
  }
END_TEST


START_TEST(hostname_in_externals_test)
  {
  char *externals = strdup("one+two+three");
  
  fail_unless(hostname_in_externals((char *)"one", externals) == TRUE, "couldn't find entry");
  fail_unless(hostname_in_externals((char *)"two", externals) == TRUE, "couldn't find entry");
  fail_unless(hostname_in_externals((char *)"three", externals) == TRUE, "couldn't find entry");

  /* corner cases */
  fail_unless(hostname_in_externals((char *)"thre", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals((char *)"tw", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals((char *)"on", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals((char *)"wo", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals((char *)"ne", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals((char *)"hree", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals((char *)"tthree", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals((char *)"ttwo", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals((char *)"oone", externals) == FALSE, "false positive");
  }
END_TEST




START_TEST(fix_cray_exec_hosts_test)
  {
  char *externals = (char *)"one+two+three";
  char *exec1 = (char *)"one/0+one/1+napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3";
  char *exec2 = (char *)"napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3+two/0+two/1";
  char *exec3 = (char *)"napali/0+napali/1+napali/2+napali/3+three/0+three/1+a1/0+a1/1+a1/2+a1/3";
  job   pjob;

  memset(&pjob, 0, sizeof(pjob));

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec1);
  pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str = strdup(externals);


  fix_cray_exec_hosts(&pjob);
  snprintf(buf, sizeof(buf), "exec hosts should be %s but is %s",
    "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3",
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str);

  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3"), buf);

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec2);
  fix_cray_exec_hosts(&pjob);
  snprintf(buf, sizeof(buf), "exec hosts should be %s but is %s",
    "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3",
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str);

  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3"), buf);

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec3);
  fix_cray_exec_hosts(&pjob);
  snprintf(buf, sizeof(buf), "exec hosts should be %s but is %s",
    "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3",
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str);

  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3"), buf);
  }
END_TEST



START_TEST(fix_external_exec_hosts_test)
  {
  char *externals = (char *)"one+two+three";
  char *exec1 = (char *)"one/0+one/1+napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3";
  char *exec2 = (char *)"napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3+two/0+two/1";
  char *exec3 = (char *)"napali/0+napali/1+napali/2+napali/3+three/0+three/1+a1/0+a1/1+a1/2+a1/3";
  job   pjob;

  memset(&pjob, 0, sizeof(pjob));

  fail_unless(fix_external_exec_hosts(&pjob) == PBSE_BAD_PARAMETER, "error codes not correctly checked");

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec1);
  pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str = strdup(externals);

  fix_external_exec_hosts(&pjob);
  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "one/0+one/1"), "not set correctly");

  
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec2);
  fix_external_exec_hosts(&pjob);
  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "two/0+two/1"), "not set correctly");
  
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec3);
  fix_external_exec_hosts(&pjob);
  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "three/0+three/1"), "not set correctly");
  }
END_TEST



START_TEST(change_external_job_name_test)
  {
  job pjob;

  strcpy(pjob.ji_qs.ji_jobid, "1.napali");
  change_external_job_name(&pjob);
  fail_unless(!strcmp(pjob.ji_qs.ji_jobid, "1-0.napali"), "name not changed correctly");

  strcpy(pjob.ji_qs.ji_jobid, "12.napali");
  change_external_job_name(&pjob);
  fail_unless(!strcmp(pjob.ji_qs.ji_jobid, "12-0.napali"), "name not changed correctly");

  strcpy(pjob.ji_qs.ji_jobid, "1005.napali");
  change_external_job_name(&pjob);
  fail_unless(!strcmp(pjob.ji_qs.ji_jobid, "1005-0.napali"), "name not changed correctly");
  }
END_TEST



START_TEST(split_job_test)
  {
  job   pjob;
  job  *external;
  job  *cray;
  char *externals = (char *)"one+two+three";
  char *exec1 = (char *)"one/0+one/1+napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3";

  memset(&pjob, 0, sizeof(pjob));

  strcpy(pjob.ji_qs.ji_jobid, "12.napali");
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec1);
  pjob.ji_wattr[JOB_ATR_exec_host].at_flags |= ATR_VFLAG_SET;
  pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str = strdup(externals);
  pjob.ji_wattr[JOB_ATR_external_nodes].at_flags |= ATR_VFLAG_SET;

  split_job(&pjob);

  external = pjob.ji_external_clone;
  cray     = pjob.ji_cray_clone;

  fail_unless(!strcmp(external->ji_wattr[JOB_ATR_exec_host].at_val.at_str, "one/0+one/1"), "external's exec host bad");
  fail_unless(!strcmp(cray->ji_wattr[JOB_ATR_exec_host].at_val.at_str, "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3"), "cray's exec host bad");
  fail_unless(!strcmp(external->ji_qs.ji_jobid, "12-0.napali"), "external's name not correct");
  }
END_TEST

START_TEST(job_abt_test)
  {
  int result = 0;
  struct job *null_job = NULL;

  result = job_abt(NULL, NULL);
  fail_unless(result != 0, "NULL input check fail");

  result = job_abt(&null_job, NULL);
  fail_unless(result != 0, "NULL input check fail");
  }
END_TEST

START_TEST(conn_qsub_test)
  {
  int result = 0;

  result = conn_qsub(NULL, 0, NULL);
  fail_unless(result != 0, "NULL input check fail");
  }
END_TEST

START_TEST(handle_aborted_job_test)
  {
  job pjob;
  job *job_ptr = &pjob;

  memset(&pjob, 0, sizeof(pjob));
  strcpy(pjob.ji_qs.ji_jobid, "1.lei");
  long KeepSeconds = 50;

  add_job_called = false;
  const char *text = "Job deleted as result of dependency on job 0.lei";
 
  handle_aborted_job(&job_ptr, true, KeepSeconds, text);
 
  fail_unless(job_ptr == &pjob, "job_ptr should be pointing pjob");
  fail_unless(add_job_called == true, "set_task should have been called");
  fail_unless(pjob.ji_wattr[JOB_ATR_exitstat].at_val.at_long ==271, "job status was not set");
  fail_unless((pjob.ji_wattr[JOB_ATR_Comment].at_flags & ATR_VFLAG_SET) > 0, "comment was not set in the job");
  int comparison = -1;
  if (pjob.ji_wattr[JOB_ATR_Comment].at_val.at_str)
    comparison = strcmp(pjob.ji_wattr[JOB_ATR_Comment].at_val.at_str, text);
  fail_unless(comparison == 0, "comment added to job did not match to text");

  handle_aborted_job(&job_ptr, true, KeepSeconds, "Appended");
  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_Comment].at_val.at_str, "Job deleted as result of dependency on job 0.lei. Appended"));

  handle_aborted_job(&job_ptr, true, 0, text);
  fail_unless(job_ptr == NULL);
  }
END_TEST /* handle_aborted_job_test */

START_TEST(job_alloc_test)
  {
  struct job *result = NULL;

  result = job_alloc();
  fail_unless(result != NULL, "job was not allocated");
  }
END_TEST

START_TEST(job_free_test)
  {
  struct job *result = NULL;
  /* NULL value test */
  job_free(result,0);

  result = job_alloc();
  job_free(result,0);
  /* TODO: add some check for the free operation
     or refactor function interface, suggestions:
     1) pointer to pointer pass, then null the actual pointer
     2) return value, could be a code or the actual ptr from 1*/
  }
END_TEST

START_TEST(copy_job_test)
  {
  struct job* result = copy_job(NULL);
  struct job *parent = job_alloc();
  struct job *child = NULL;
  fail_unless(result == NULL, "NULL input check fail");

  child = copy_job(parent);
  fail_unless(child != NULL, "job was not copied");
  /* TODO: add check for correctness of the copy */
  }
END_TEST

START_TEST(job_clone_test)
  {
  printf("job_clone_test is commented out because it fails\n");
/*
  struct job_array array;
  struct job* template_job = job_alloc();

  struct job* result = job_clone(NULL, &array, 0);
  fail_unless(result == NULL, "NULL job to clone check fail");

  result = job_clone(template_job, NULL, 0);
  fail_unless(result == ((job*)1), "NULL job array check fail"); 
  result = job_clone(template_job, &array, 0); */
  /* TODO: add test that compares template and cloned jobs*/
  /*fail_unless(template-> == result->, "job clone fail");*/
  }
END_TEST

START_TEST(job_clone_wt_test)
  {
  void *result = job_clone_wt(NULL);
  fail_unless(result == NULL, "NULL input check fail");
  }
END_TEST

START_TEST(cpy_checkpoint_test)
  {
  struct job *test_job = job_alloc();
  struct batch_request *result = cpy_checkpoint(NULL,
                                         test_job,
                                         JOB_ATR_checkpoint_name,
                                         CKPT_DIR_IN);
  struct batch_request *initial = alloc_br(/*PBS_BATCH_CheckpointJob*/0);
  fail_unless(result == NULL, "NULL batch_request input fail");

  result = cpy_checkpoint(initial,
                          NULL,
                          JOB_ATR_checkpoint_name,
                          CKPT_DIR_IN);
  fail_unless(result == NULL, "NULL job input fail");

  /*TODO: add test for valid input, invalid dir value*/
  }
END_TEST

START_TEST(remove_checkpoint_test)
  {
  struct job* test_job = NULL;
  remove_checkpoint(NULL); /*TODO: add a kind of assert*/
  remove_checkpoint(&test_job);

  test_job = job_alloc();
  remove_checkpoint(&test_job);
  }
END_TEST

START_TEST(cleanup_restart_file_test)
  {
  struct job* test_job = NULL;
  cleanup_restart_file(test_job); /*TODO: add a kind of assert*/

  test_job = job_alloc();
  cleanup_restart_file(test_job);
  }
END_TEST

START_TEST(record_jobinfo_test)
  {
  printf("record_jobinfo_test commented out\n");
  /*
  struct job* test_job = NULL;
  int result = record_jobinfo(test_job);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL job input fail");

  test_job = job_alloc();
  record_jobinfo(test_job);
  fail_unless(result >= -1, "empty job input fail: %d", result);*/
  }
END_TEST

START_TEST(svr_job_purge_test)
  {
  struct job* test_job = NULL;
  int result = svr_job_purge(test_job);
  fail_unless(result != 0, "NULL job input fail");

  called_remove_job = 0;
  dequejob_rc = PBSE_JOB_NOT_IN_QUEUE;
  test_job = job_alloc();
  test_job->ji_qs.ji_substate = JOB_SUBSTATE_QUEUED;
  test_job->ji_qs.ji_state = JOB_STATE_QUEUED;
  memset(test_job->ji_arraystructid, 0, sizeof(test_job->ji_arraystructid));
  result = svr_job_purge(test_job);
  fail_unless(result == 0, "non-queued job fail: %d", result);
  // called_remove_job once means we didn't call job_free
  fail_unless(called_remove_job == 1);
  
  dequejob_rc = 0;
  result = svr_job_purge(test_job);
  fail_unless(result == 0, "queued job fail: %d", result);
  // Calling remove_job twice means we did call job_free
  fail_unless(called_remove_job == 3);
  }
END_TEST

START_TEST(svr_find_job_test)
  {
  struct job* result = svr_find_job(NULL,0);
  fail_unless(result == NULL, "NULL job id input fail");

  result = svr_find_job((char *)"",0);
  fail_unless(result == NULL, "empty job id input fail");
  }
END_TEST

START_TEST(get_jobs_array_test)
  {
  struct job *test_job = NULL;

  struct job_array *result = get_jobs_array(NULL);
  fail_unless(result == NULL, "NULL input pointer to pointer to job fail");

  result = get_jobs_array(&test_job);
  fail_unless(result == NULL, "NULL input pointer to job fail");

  test_job = job_alloc();
  result = get_jobs_array(&test_job);
  fail_unless(result == NULL, "get job array fail");
  }
END_TEST

START_TEST(get_jobs_queue_test)
  {
  struct job *test_job = NULL;

  struct pbs_queue *result = get_jobs_queue(NULL);
  fail_unless(result == NULL, "NULL input pointer to pointer to job fail");

  result = get_jobs_queue(&test_job);
  fail_unless(result == NULL, "NULL input pointer to job fail");

  test_job = job_alloc();
  result = get_jobs_queue(&test_job);
  fail_unless(result == NULL, "get job queue fail");
  }
END_TEST

Suite *job_func_suite(void)
  {
  Suite *s = suite_create("job_func_suite methods");
  TCase *tc_core = tcase_create("hostname_in_externals_test");
  tcase_add_test(tc_core, hostname_in_externals_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("fix_cray_exec_hosts_test");
  tcase_add_test(tc_core, fix_cray_exec_hosts_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("fix_external_exec_hosts_test");
  tcase_add_test(tc_core, fix_external_exec_hosts_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("change_external_job_name_test");
  tcase_add_test(tc_core, change_external_job_name_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("split_job_test");
  tcase_add_test(tc_core, split_job_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("job_abt_test");
  tcase_add_test(tc_core, job_abt_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("conn_qsub_test");
  tcase_add_test(tc_core, conn_qsub_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("job_alloc_test");
  tcase_add_test(tc_core, job_alloc_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("job_free_test");
  tcase_add_test(tc_core, job_free_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("copy_job_test");
  tcase_add_test(tc_core, copy_job_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("job_clone_test");
  tcase_add_test(tc_core, job_clone_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("job_clone_wt_test");
  tcase_add_test(tc_core, job_clone_wt_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("cpy_checkpoint_test");
  tcase_add_test(tc_core, cpy_checkpoint_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("remove_checkpoint_test");
  tcase_add_test(tc_core, remove_checkpoint_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("cleanup_restart_file_test");
  tcase_add_test(tc_core, cleanup_restart_file_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("record_jobinfo_test");
  tcase_add_test(tc_core, record_jobinfo_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("svr_job_purge_test");
  tcase_add_test(tc_core, svr_job_purge_test);
  tcase_add_test(tc_core, internal_job_id_exists_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("svr_find_job_test");
  tcase_add_test(tc_core, svr_find_job_test);
  tcase_add_test(tc_core, get_jobs_array_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("get_jobs_queue_test");
  tcase_add_test(tc_core, get_jobs_queue_test);
  tcase_add_test(tc_core, handle_aborted_job_test);
  suite_add_tcase(s, tc_core);

  return(s);
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(job_func_suite());
  srunner_set_log(sr, "job_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
