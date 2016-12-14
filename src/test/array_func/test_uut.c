#include "license_pbs.h" /* See here for the software license */
#include "array_func.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pbs_error.h"
#include "list_link.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

int is_num(const char *);
int array_request_token_count(const char *);
int array_request_parse_token(char *, int *, int *);
int num_array_jobs(const char *str);
int array_recov_binary(const char *path, job_array **new_pa, char *log_buf, size_t buflen);
int parse_array_dom(job_array **pa, xmlNodePtr root_element, char *log_buf, size_t buflen);
void update_array_values(job_array *pa, int old_state, enum ArrayEventsEnum event, const char *job_id, long job_atr_hold, int job_exit_status);
void release_slot_hold(job *pjob, int &difference);
int update_slot_values(job_array *pa, int actually_running, int number_queued, job *held, std::vector<std::string> &candidates);
bool need_to_update_slot_limits(job_array *pa);
void set_slot_hold(job *pjob, int &difference);

extern std::string get_path_jobdata(const char *, const char *);
const char *array_sample = "<array>\n</array>";
extern char *path_arrays;

extern bool place_hold;
extern int  unlocked;


job_array *get_job_array(

  int size)

  {
  char       buf[1024];
  job_array *pa = (job_array *)calloc(1, sizeof(job_array));
  pa->ai_qs.num_jobs = size;
  pa->job_ids = (char **)calloc(size, sizeof(char *));

  for (int i = 0; i < size; i++)
    {
    snprintf(buf, sizeof(buf), "0[%d].napali", i);
    pa->job_ids[i] = strdup(buf);
    }

  return(pa);
  }


START_TEST(need_to_update_slot_limits_test)
  {
  job_array pa;

  memset(&pa, 0, sizeof(pa));

  pa.ai_qs.slot_limit = 2;
  pa.ai_qs.jobs_running = 2;

  fail_unless(need_to_update_slot_limits(&pa) == false);
  pa.ai_qs.jobs_running = 3;
  fail_unless(need_to_update_slot_limits(&pa) == false);
  pa.ai_qs.jobs_running = 1;
  fail_unless(need_to_update_slot_limits(&pa) == true);
  pa.ai_qs.jobs_running = 0;
  fail_unless(need_to_update_slot_limits(&pa) == true);
  }
END_TEST


START_TEST(set_slot_hold_test)
  {
  job pjob;
  int difference = -2;
  memset(&pjob, 0, sizeof(pjob));
  pjob.ji_wattr[JOB_ATR_hold].at_val.at_long = HOLD_l;

  // slot hold is already set, so we shouldn't change
  set_slot_hold(&pjob, difference);
  fail_unless(difference == -2);
  fail_unless(pjob.ji_wattr[JOB_ATR_hold].at_flags == 0);
  
  // unset the slot hold to see a change
  pjob.ji_wattr[JOB_ATR_hold].at_val.at_long = 0;
  set_slot_hold(&pjob, difference);
  fail_unless(difference == -1);
  fail_unless(pjob.ji_wattr[JOB_ATR_hold].at_flags == ATR_VFLAG_SET);
  fail_unless(pjob.ji_wattr[JOB_ATR_hold].at_val.at_long == HOLD_l);
  }
END_TEST


START_TEST(release_slot_hold_test)
  {
  int difference = 2;
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  pjob.ji_wattr[JOB_ATR_hold].at_val.at_long = HOLD_l;

  // Make sure that we release the hold and decrement difference
  release_slot_hold(&pjob, difference);
  fail_unless(difference == 1);
  fail_unless((pjob.ji_wattr[JOB_ATR_hold].at_val.at_long & HOLD_l) == 0);

  // Make sure calling this with a job that isn't set does nothing
  long current_hold = pjob.ji_wattr[JOB_ATR_hold].at_val.at_long;
  release_slot_hold(&pjob, difference);
  fail_unless(difference == 1);
  fail_unless(pjob.ji_wattr[JOB_ATR_hold].at_val.at_long == current_hold);
  }
END_TEST


START_TEST(update_slot_values_test)
  {
  std::vector<std::string>  candidates;
  char                      buf[1024];
  job                      *pjob = (job *)calloc(1, sizeof(job));
  job_array                *pa = get_job_array(10);

  sprintf(pjob->ji_qs.ji_jobid, "0[0].napali");
  pa->ai_qs.slot_limit = 2;
  pa->ai_qs.jobs_running = 2;

  for (int i = 0; i < 2; i++)
    {
    sprintf(buf, "0[%d].napali", i);
    candidates.push_back(buf);
    }

  place_hold = true;
  unlocked = 0;

  // Make sure the counts are corrected after calling update_slot_values()
  fail_unless(update_slot_values(pa, 0, 1, pjob, candidates) == PBSE_NONE);
  fail_unless(pa->ai_qs.jobs_running == 0);
  fail_unless(unlocked == 1);
  }
END_TEST


START_TEST(update_array_values_test)
  {
  job_array  *pa = (job_array *)calloc(1, sizeof(job_array));
  const char *job_id = "1[0].napali";

  pa->ai_qs.num_jobs = 10;

  update_array_values(pa, JOB_STATE_TRANSIT, aeQueue, job_id, -1, -1);
  update_array_values(pa, JOB_STATE_QUEUED, aeRun, job_id, -1, -1);
  fail_unless(pa->ai_qs.jobs_running == 1);
  fail_unless(pa->ai_qs.num_started == 1);
 
  update_array_values(pa, JOB_STATE_RUNNING, aeRerun, job_id, -1, -1);
  fail_unless(pa->ai_qs.jobs_running == 0);
  fail_unless(pa->ai_qs.num_started == 0);
  }
END_TEST


START_TEST(parse_array_dom_test)
  {
  xmlDocPtr doc = xmlReadMemory(array_sample, strlen(array_sample), "array", NULL, 0);
  job_array *pa = NULL;
  char buf[1024];

  /* this test fails prior to the fix for TRQ-2219 */
  parse_array_dom(&pa, xmlDocGetRootElement(doc), buf, sizeof(buf));
  }
END_TEST


START_TEST(set_slot_limit_test)
  {
  job_array pa;
  char *p;

  memset(&pa, 0, sizeof(job_array));

  fail_unless(set_slot_limit(strdup("tom"), &pa) == 0, "failed with no slot limit");
  fail_unless(pa.ai_qs.slot_limit == NO_SLOT_LIMIT, "set no slot limit incorrectly");
 
  fail_unless(set_slot_limit(strdup("tom%10"), &pa) == INVALID_SLOT_LIMIT, "set slot limit higher than allowed");
  
  p = strdup("tom%3"); 
  fail_unless(set_slot_limit(p, &pa) == 0, "didn't allow legal slot limit");
  fail_unless(pa.ai_qs.slot_limit == 3, "set slot limit incorrectly");

  // confirm set_slot_limit() non-descructive to string with %
  fail_unless(strcmp(p, "tom%3") == 0);
  
  fail_unless(set_slot_limit(strdup("tom%"), &pa) == 0, "failing because of stray '%'");
  /* 5 comes from the scaffolding */
  fail_unless(pa.ai_qs.slot_limit == 5, "set max slot limit incorrectly");
  }
END_TEST




START_TEST(is_num_test)
  {
  fail_unless(is_num("") == 0, "empty string isn't a number");
  fail_unless(is_num("1a") == 0, "1a isn't a number");
  fail_unless(is_num("a1") == 0, "a1 isn't a number");
  fail_unless(is_num("1234") == 1, "1234 is a number");
  fail_unless(is_num("12143141324132434") == 1, "12143141324132434 is a number");
  }
END_TEST




START_TEST(array_request_token_count_test)
  {
  fail_unless(array_request_token_count("") == 0, "counted tokens wrong 0");
  fail_unless(array_request_token_count("0-4") == 1, "counted tokens wrong 1");
  fail_unless(array_request_token_count("0-4,10") == 2, "counted tokens wrong 2");
  fail_unless(array_request_token_count("0-4,7-9") == 2, "counted tokens wrong 3");
  fail_unless(array_request_token_count("0-4,100,200") == 3, "counted tokens wrong 4");
  fail_unless(array_request_token_count("0,10,100,1000") == 4, "counted tokens wrong 5");
  }
END_TEST




START_TEST(array_request_parse_token_test)
  {
  int left;
  int right;

  /* test for bad parameters */
  fail_unless(array_request_parse_token(NULL,  &left, &right) == 0, "unhandled bad parameter (NULL str)");
  fail_unless(array_request_parse_token((char *)"1-2", NULL,  &right) == 0, "unhandled bad parameter (NULL left)");
  fail_unless(array_request_parse_token((char *)"1-2", &left, NULL)   == 0, "unhandled bad parameter (NULL right)");

  fail_unless(array_request_parse_token(strdup("--"), &left, &right) == 0, "bad range fail");
  fail_unless(left == -1, "start set incorrectly with bad range");
  fail_unless(right == -1, "end set incorrectly with bad range");

  fail_unless(array_request_parse_token(strdup("10"), &left, &right) == 1, "single number fail");
  fail_unless(left == 10, "start set bad");
  fail_unless(right == 10, "end set bad");
  
  fail_unless(array_request_parse_token(strdup("0-10"), &left, &right) == 11, "range fail");
  fail_unless(left == 0, "start set bad");
  fail_unless(right == 10, "end set bad");

  fail_unless(array_request_parse_token(strdup("1a"), &left, &right) == 0, "bad num fail");
  fail_unless(left == -1, "start set incorrectly with bad num");
  fail_unless(right == -1, "end set incorrectly with bad num");

  fail_unless(array_request_parse_token(strdup("a-5"), &left, &right) == 0, "bad range fail");
  fail_unless(left == -1, "start set incorrectly with bad range");
  fail_unless(right == -1, "end set incorrectly with bad range");
  }
END_TEST




START_TEST(first_job_index_test)
  {
  job_array pa;
  int index;
  char buf[4096];

  memset(&pa, 0, sizeof(pa));
  pa.job_ids = (char **)calloc(10, sizeof(char *));
  pa.ai_qs.array_size = 10;

  fail_unless(first_job_index(&pa) == -1, "no jobs fail");
  
  pa.job_ids[8] = (char *)"bob";
  index = first_job_index(&pa);
  snprintf(buf, sizeof(buf), "first job index should be 8 but is %d", index);
  fail_unless(index == 8, buf);

  pa.job_ids[4] = (char *)"tom";
  index = first_job_index(&pa);
  snprintf(buf, sizeof(buf), "first job index should be 4 but is %d", index);
  fail_unless(index == 4, buf);
  }
END_TEST


START_TEST(num_array_jobs_test)
  {
  fail_unless(num_array_jobs(NULL) == -1, "null fail");
  fail_unless(num_array_jobs("10") == 1, "single job fail");
  fail_unless(num_array_jobs("0-10") == 11, "range fail 1");
  fail_unless(num_array_jobs("4-2") == -1, "bad range fail");
  fail_unless(num_array_jobs("0-10,13") == 12, "range fail 2");
  fail_unless(num_array_jobs("0-10,13-20") == 19, "range fail 3");
  fail_unless(num_array_jobs("0-20,23-24,30-40") == 34, "range fail 4");

  }
END_TEST

START_TEST(array_delete_test)
  {
  job_array *pa;

  pa = (job_array *)calloc(1,sizeof(job_array));
  pa->job_ids = (char **)calloc(10, sizeof(char *));
  pa->ai_qs.array_size = 10;

  //Set up the save path

  path_arrays = (char *)"./";
  strcpy(pa->ai_qs.fileprefix,"tempFile");
  char path[256];
  snprintf(path, sizeof(path), "%s%s%s",
    path_arrays, pa->ai_qs.fileprefix, ARRAY_FILE_SUFFIX);
  FILE *fp = fopen(path,"w");
  fprintf(fp,"I'm a very temporary file.\n");
  fclose(fp);



  struct array_depend *pdep = (struct array_depend *)calloc(1,sizeof(struct array_depend));
  pa->ai_qs.deps.ll_next = &pdep->dp_link;
  pdep->dp_link.ll_prior = (list_link *)&pa->ai_qs.deps;
  pdep->dp_link.ll_struct = (void *)pdep;
  pdep->dp_jobs.push_back(new (array_depend_job));
  pdep->dp_jobs.push_back(new (array_depend_job));
  pdep->dp_jobs.push_back(new (array_depend_job));
  pdep->dp_jobs.push_back(new (array_depend_job));
  pdep->dp_jobs.push_back(new (array_depend_job));

  array_delete(pa);

  }
END_TEST


START_TEST(array_recov_binary_test)
  {
  job_array *pa;
  char       log_buf[LOCAL_LOG_BUF_SIZE];

  /* missing file */
  fail_unless(array_recov_binary((char *)"", &pa, log_buf, sizeof(log_buf)) == PBSE_SYSTEM, "failed array_recov");

  /* zero length file */
  fail_unless(array_recov_binary((char *)"file_zero", &pa, log_buf, sizeof(log_buf)) == PBSE_SYSTEM, "failed array_recov");
  }
END_TEST


START_TEST(parse_array_request_test)
  {
  char *request;
  tlist_head h;

  CLEAR_HEAD(h);

#define TSTRING "0-1,10%1"

  request = strdup(TSTRING);
  fail_unless(parse_array_request(request, &h) == 0);
  fail_unless(strcmp(request, TSTRING) == 0);
  }
END_TEST


Suite *array_func_suite(void)
  {
  Suite *s = suite_create("array_func_suite methods");
  TCase *tc_core = tcase_create("set_slot_limit_test");
  tcase_add_test(tc_core, set_slot_limit_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("is_num_test");
  tcase_add_test(tc_core, is_num_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("array_request_token_count_test");
  tcase_add_test(tc_core, array_request_token_count_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("array_request_parse_token_test");
  tcase_add_test(tc_core, array_request_parse_token_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("num_array_jobs_test");
  tcase_add_test(tc_core, num_array_jobs_test);
  tcase_add_test(tc_core, need_to_update_slot_limits_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("first_job_index_test");
  tcase_add_test(tc_core, first_job_index_test);
  tcase_add_test(tc_core, parse_array_dom_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("array_delete_test");
  tcase_add_test(tc_core,array_delete_test);
  tcase_add_test(tc_core, release_slot_hold_test);
  tcase_add_test(tc_core, set_slot_hold_test);
  suite_add_tcase(s,tc_core);

  tc_core = tcase_create("array_recov_binary_test");
  tcase_add_test(tc_core, array_recov_binary_test);
  tcase_add_test(tc_core, update_array_values_test);
  tcase_add_test(tc_core, update_slot_values_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("parse_array_request_test");
  tcase_add_test(tc_core, parse_array_request_test);
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
  sr = srunner_create(array_func_suite());
  srunner_set_log(sr, "array_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
