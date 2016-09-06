#include "license_pbs.h" /* See here for the software license */
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "req_quejob.h"
#include "test_req_quejob.h"
#include "pbs_error.h"
#include "server.h"
#include "pbs_job.h"

extern all_jobs newjobs;

extern int req_jobcredential(batch_request *preq);
int get_job_id(batch_request *preq, int &resc_access_perm, int &created_here, std::string &jobid);
bool job_exists(const char *job_id);
pbs_queue *get_queue_for_job(char *queue_name, int &rc);
int determine_job_file_name(batch_request *preq, std::string &jobid, std::string &filename);
job *create_and_initialize_job_structure(int created_here, std::string &jobid);

extern struct server server;
extern char *path_jobs;
char path_to_jobs[500];
extern char server_name[];
extern char str_to_set[];
extern long long_to_set;
extern bool default_queue;
extern bool mem_fail;
extern char *path_jobs;
extern bool set_ji_substate;


START_TEST(test_create_and_initialize_job_structure)
  {
  std::string jobid("1.napali");
 
  mem_fail = true;
  job *pjob = create_and_initialize_job_structure(1, jobid);
  fail_unless(pjob == NULL);

  mem_fail = false;
  pjob = create_and_initialize_job_structure(1, jobid);
  fail_unless(pjob != NULL);
  fail_unless(jobid == pjob->ji_qs.ji_jobid);
  fail_unless(pjob->ji_modified == 1);
  fail_unless(pjob->ji_qs.ji_svrflags == 1);
  fail_unless(pjob->ji_qs.ji_un_type == JOB_UNION_TYPE_NEW);
  fail_unless(pjob->ji_wattr[JOB_ATR_mailpnts].at_val.at_str == NULL);
  fail_unless(pjob->ji_mod_time > 0);
  }
END_TEST


START_TEST(test_determine_job_file_name)
  {
  std::string jobid("1.napali");
  std::string filename;
  path_jobs = strdup("./");

  system("rm ./1.napali.JB ./1.napalj.JB");
  fail_unless(determine_job_file_name(NULL, jobid, filename) == PBSE_NONE);
  fail_unless(filename == "1.napali", filename.c_str());
  fail_unless(determine_job_file_name(NULL, jobid, filename) == PBSE_NONE);
  fail_unless(filename == "1.napalj", filename.c_str());
  system("rm ./1.napali.JB ./1.napalj.JB");
  }
END_TEST


START_TEST(test_get_queue_for_job)
  {
  char queue_name[1024];
  int  rc = PBSE_NONE;
  
  default_queue = false;
  queue_name[0] = '@';
  fail_unless(get_queue_for_job(queue_name, rc) == NULL);
  fail_unless(rc == PBSE_QUENODFLT);

  default_queue = true;
  fail_unless(get_queue_for_job(queue_name, rc) != NULL);

  strcpy(queue_name, "batch2");
  fail_unless(get_queue_for_job(queue_name, rc) == NULL);
  fail_unless(rc == PBSE_UNKQUE);
  
  strcpy(queue_name, "batch");
  fail_unless(get_queue_for_job(queue_name, rc) != NULL);
  }
END_TEST


START_TEST(test_job_exists)
  {
  fail_unless(job_exists("0.napali") == false);
  fail_unless(job_exists("1.napali") == true);
  }
END_TEST


START_TEST(test_one)
  {
  struct batch_request req;
  job j;
  char cmd[500];

  memset(&j,0,sizeof(j));
  memset(&req,0,sizeof(req));

  fail_unless(req_jobcredential(&req) == PBSE_IVALREQ);

  strcpy(j.ji_qs.ji_jobid,"SomeJob");
  strcpy(j.ji_qs.ji_fileprefix,"prefix");
  newjobs.lock();
  newjobs.insert(&j,j.ji_qs.ji_jobid);
  newjobs.unlock();

  fail_unless(req_jobcredential(&req) == PBSE_NONE);

  memset(&req,0,sizeof(req));

  strcpy(req.rq_ind.rq_jobfile.rq_jobid,"NotThisJob");
  fail_unless(req_jobscript(&req) == PBSE_IVALREQ);

  path_jobs = getcwd(path_to_jobs,sizeof(path_to_jobs));
  strcat(path_jobs,"/");
  sprintf(cmd,"rm -f %s*.SC",path_jobs);
  system(cmd);
  strcpy(req.rq_ind.rq_jobfile.rq_jobid,"1.napali");
  fail_unless(req_jobscript(&req) == PBSE_NONE);
  system(cmd);
  }
END_TEST


START_TEST(test_get_job_id)
  {
  std::string   job_id;
  int           resc_access_perm = ATR_DFLAG_USWR | ATR_DFLAG_Creat;
  int           created_here = 0;
  batch_request preq;

  memset(&preq, 0, sizeof(preq));

  preq.rq_fromsvr = 1;
  strcpy(preq.rq_ind.rq_queuejob.rq_jid, "1.napali");
  fail_unless(get_job_id(&preq, resc_access_perm, created_here, job_id) == PBSE_NONE);
  fail_unless(job_id == "1.napali");
  fail_unless(resc_access_perm & (ATR_DFLAG_MGWR | ATR_DFLAG_SvWR));
  fail_unless(created_here == 0);

  preq.rq_fromsvr = 0;
  resc_access_perm = ATR_DFLAG_USWR | ATR_DFLAG_Creat;
  fail_unless(get_job_id(&preq, resc_access_perm, created_here, job_id) != PBSE_NONE);

  preq.rq_ind.rq_queuejob.rq_jid[0] = '\0';
  sprintf(server_name, "napali");
  fail_unless(get_job_id(&preq, resc_access_perm, created_here, job_id) == PBSE_NONE);
  fail_unless(job_id == "0.napali", job_id.c_str());
  fail_unless(created_here == 1);
  fail_unless(resc_access_perm == (ATR_DFLAG_USWR | ATR_DFLAG_Creat));

  strcpy(str_to_set, "tom");
  created_here = 0;
  fail_unless(get_job_id(&preq, resc_access_perm, created_here, job_id) == PBSE_NONE);
  fail_unless(created_here == 1);
  fail_unless(job_id == "1.napali.tom", job_id.c_str());

  created_here = 0;
  long_to_set = 0;
  fail_unless(get_job_id(&preq, resc_access_perm, created_here, job_id) == PBSE_NONE);
  fail_unless(created_here == 1);
  fail_unless(job_id == "2.tom", job_id.c_str());

  created_here = 0;
  str_to_set[0] = '\0';
  fail_unless(get_job_id(&preq, resc_access_perm, created_here, job_id) == PBSE_NONE);
  fail_unless(created_here == 1);
  fail_unless(job_id == "3", job_id.c_str());

  server.sv_qs.sv_jobidnumber = PBS_SEQNUMTOP;
  fail_unless(get_job_id(&preq, resc_access_perm, created_here, job_id) == PBSE_NONE);
  fail_unless(job_id == "99999999", job_id.c_str());
  
  fail_unless(get_job_id(&preq, resc_access_perm, created_here, job_id) == PBSE_NONE);
  fail_unless(job_id == "0", job_id.c_str());
  }
END_TEST


START_TEST(test_req_commit)
  {
  struct batch_request *preq;

  fail_unless((preq = (struct batch_request *)calloc(1, sizeof(struct batch_request))) != NULL);

  strcpy(preq->rq_ind.rq_commit, "1.napali");
  default_queue = false;
  set_ji_substate = true;
  fail_unless(PBSE_MUTEX_ALREADY_UNLOCKED == req_commit(preq));
  }
END_TEST


Suite *req_quejob_suite(void)
  {
  Suite *s = suite_create("req_quejob_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_get_job_id");
  tcase_add_test(tc_core, test_get_job_id);
  tcase_add_test(tc_core, test_job_exists);
  tcase_add_test(tc_core, test_get_queue_for_job);
  tcase_add_test(tc_core, test_determine_job_file_name);
  tcase_add_test(tc_core, test_create_and_initialize_job_structure);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_req_commit");
  tcase_add_test(tc_core, test_req_commit);
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
  server.sv_attr_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  rundebug();
  sr = srunner_create(req_quejob_suite());
  srunner_set_log(sr, "req_quejob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
