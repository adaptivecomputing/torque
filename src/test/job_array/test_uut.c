#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"
#include "array.h"
#include <check.h>

extern long max_slot;
extern long idle_max_slot;
extern long array_size;
extern bool unset;

START_TEST(test_constructors)
  {
  job_array pa;

  fail_unless(pa.ai_qs.struct_version == ARRAY_QS_STRUCT_VERSION);
  fail_unless(pa.ai_qs.array_size == 0);
  fail_unless(pa.ai_qs.num_jobs == 0);
  fail_unless(pa.ai_qs.slot_limit == NO_SLOT_LIMIT);
  fail_unless(pa.ai_qs.jobs_running == 0);
  fail_unless(pa.ai_qs.jobs_done == 0);
  fail_unless(pa.ai_qs.num_cloned == 0);
  fail_unless(pa.ai_qs.num_started == 0);
  fail_unless(pa.ai_qs.num_failed == 0);
  fail_unless(pa.ai_qs.num_successful == 0);
  fail_unless(pa.ai_qs.num_purged == 0);
  fail_unless(pa.ai_qs.num_idle == 0);
  fail_unless(pa.ai_qs.idle_slot_limit == DEFAULT_IDLE_SLOT_LIMIT);
  fail_unless(pa.ai_qs.highest_id_created == -1);
  fail_unless(pa.ai_qs.range_str.size() == 0);
  fail_unless(strlen(pa.ai_qs.owner) == 0);
  fail_unless(strlen(pa.ai_qs.parent_id) == 0);
  fail_unless(strlen(pa.ai_qs.fileprefix) == 0);
  fail_unless(strlen(pa.ai_qs.submit_host) == 0);

  fail_unless(pa.job_ids == NULL);
  fail_unless(pa.jobs_recovered == 0);
  fail_unless(pa.ai_ghost_recovered == false);
  fail_unless(pa.uncreated_ids.size() == 0);
  fail_unless(pa.ai_mutex != NULL);

  const char *array_id = "1[].napali";
  pa.set_array_id(array_id);
  fail_unless(!strcmp(pa.ai_qs.parent_id, array_id));

  const char *prefix = "bob";
  pa.set_arrays_fileprefix(prefix);
  fail_unless(!strcmp(pa.ai_qs.fileprefix, prefix));

  const char *kal = "kaladin";
  pa.set_owner(kal);
  fail_unless(!strcmp(pa.ai_qs.owner, kal));

  const char *sh = "roshar";
  pa.set_submit_host(sh);
  fail_unless(!strcmp(pa.ai_qs.submit_host, sh));
  }
END_TEST


START_TEST(test_set_slot_limit)
  {
  job_array pa;

  char *request = strdup("0-99");
  fail_unless(pa.set_slot_limit(request) == PBSE_NONE);
  fail_unless(pa.ai_qs.slot_limit == NO_SLOT_LIMIT);
  
  request = strdup("0-99%2");
  fail_unless(pa.set_slot_limit(request) == PBSE_NONE);
  fail_unless(pa.ai_qs.slot_limit == 2);
 
  max_slot = 1;
  request = strdup("0-99%2");
  fail_unless(pa.set_slot_limit(request) == INVALID_SLOT_LIMIT);
  
  request = strdup("0-99");
  fail_unless(pa.set_slot_limit(request) == PBSE_NONE);
  fail_unless(pa.ai_qs.slot_limit == 1);

  idle_max_slot = 400;
  // Cannot request more than the idle slot limit
  fail_unless(pa.set_idle_slot_limit(500) == -1);
  
  fail_unless(pa.set_idle_slot_limit(200) == PBSE_NONE);
  fail_unless(pa.ai_qs.idle_slot_limit == 200);
 
  // Not requesting should get us the idle max slot limit set on the server
  fail_unless(pa.set_idle_slot_limit(-1) == PBSE_NONE);
  fail_unless(pa.ai_qs.idle_slot_limit == idle_max_slot);

  // We should get the code's default if no server idle max slot limit is set
  unset = true;
  fail_unless(pa.set_idle_slot_limit(-1) == PBSE_NONE);
  fail_unless(pa.ai_qs.idle_slot_limit == DEFAULT_IDLE_SLOT_LIMIT);
  }
END_TEST


START_TEST(test_parse_array_request)
  {
  job_array pa;

  // Make sure an invalid range fails
  fail_unless(pa.parse_array_request("the Lopen") != PBSE_NONE);

  // Make sure an array above the max allowed size fails
  array_size = 5;
  fail_unless(pa.parse_array_request("0-9") == ARRAY_TOO_LARGE);

  // submit a valid array
  array_size = 10000;
  fail_unless(pa.parse_array_request("0-9") == PBSE_NONE);
  fail_unless(pa.ai_qs.array_size == 10);
  fail_unless(pa.ai_qs.num_jobs == 10);
  fail_unless(pa.ai_qs.range_str == "0-9");
  fail_unless(pa.job_ids != NULL);
  fail_unless(pa.uncreated_ids.size() == 10);

  pa.ai_qs.idle_slot_limit = 2;
  pa.ai_qs.num_idle = 0;
  int internal_index = -1;

  // It should tell us to create sub job 0 next
  fail_unless(pa.get_next_index_to_create(internal_index) == 0);
  fail_unless(internal_index == 0);

  // Make sure we'll create a job
  pa.create_job_if_needed();
  fail_unless(pa.job_ids[0] != NULL);
  fail_unless(pa.ai_qs.highest_id_created == 0);
  pa.create_job_if_needed();
  fail_unless(pa.job_ids[1] != NULL);
  fail_unless(pa.ai_qs.highest_id_created == 1);
  }
END_TEST


START_TEST(test_initialize_uncreated_ids)
  {
  job_array pa;
  pa.ai_qs.range_str = "0-9";
  pa.initialize_uncreated_ids();
  fail_unless(pa.uncreated_ids.size() == 10);

  // Make sure that we ignore ids equal to or below the highest created
  pa.ai_qs.highest_id_created = 4;
  pa.initialize_uncreated_ids();
  fail_unless(pa.uncreated_ids.size() == 5, "Size is %d", (int)pa.uncreated_ids.size());
  }
END_TEST


START_TEST(need_to_update_slot_limits_test)
  {
  job_array pa;

  pa.ai_qs.slot_limit = 2;
  pa.ai_qs.jobs_running = 2;

  fail_unless(pa.need_to_update_slot_limits() == false);
  pa.ai_qs.jobs_running = 3;
  fail_unless(pa.need_to_update_slot_limits() == false);
  pa.ai_qs.jobs_running = 1;
  fail_unless(pa.need_to_update_slot_limits() == true);
  pa.ai_qs.jobs_running = 0;
  fail_unless(pa.need_to_update_slot_limits() == true);
  }
END_TEST


START_TEST(update_array_values_test)
  {
  job_array  *pa = new job_array();
  const char *job_id = "1[0].napali";

  pa->ai_qs.num_jobs = 10;

  pa->update_array_values(JOB_STATE_TRANSIT, aeQueue, job_id, -1);
  pa->update_array_values(JOB_STATE_QUEUED, aeRun, job_id, -1);
  fail_unless(pa->ai_qs.jobs_running == 1, "Expected 1 running job, got %d", pa->ai_qs.jobs_running);
  fail_unless(pa->ai_qs.num_started == 1, "Expected 1 job started, got %d", pa->ai_qs.num_started);
 
  pa->update_array_values(JOB_STATE_RUNNING, aeRerun, job_id, -1);
  fail_unless(pa->ai_qs.jobs_running == 0, "Expected 0 running jobs, got %d", pa->ai_qs.jobs_running);
  fail_unless(pa->ai_qs.num_started == 0, "Expected 0 started jobs, got %d", pa->ai_qs.num_started);
  }
END_TEST

Suite *job_array_suite(void)
  {
  Suite *s = suite_create("job_array_suite methods");
  TCase *tc_core = tcase_create("need_to_update_slot_limits_test");
  tcase_add_test(tc_core, need_to_update_slot_limits_test);
  tcase_add_test(tc_core, test_constructors);
  tcase_add_test(tc_core, test_parse_array_request);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("update_array_values_test");
  tcase_add_test(tc_core, update_array_values_test);
  tcase_add_test(tc_core, test_set_slot_limit);
  tcase_add_test(tc_core, test_initialize_uncreated_ids);
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
  sr = srunner_create(job_array_suite());
  srunner_set_log(sr, "job_array_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
