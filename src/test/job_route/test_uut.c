#include "license_pbs.h" /* See here for the software license */
#include "job_route.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "mutex_mgr.hpp"

bool is_bad_dest(job  *jobp, char *dest);

START_TEST(test_add_dest_null)
  {
  /* This used to cause a seg fault. If it executes without
     crashing, the test passes. */
  add_dest(NULL); 
  }
END_TEST

START_TEST(test_add_dest)
  {
  struct job j;
  j.ji_rejectdest = new std::vector<std::string>();
  strcpy(j.ji_qs.ji_destin, "test");
  add_dest(&j);
  }
END_TEST

START_TEST(test_queue_route_null)
  {
  fail_unless(NULL == queue_route(NULL), "expected queue_route(NULL) to return NULL");
  }
END_TEST

START_TEST(test_is_bad_dest)
  {
  struct job j;
  sprintf(j.ji_qs.ji_destin, "test");
  j.ji_rejectdest = new std::vector<std::string>();
  add_dest(&j);
  fail_unless(false == is_bad_dest(&j, (char *)"random"));
  fail_unless(true == is_bad_dest(&j, (char *)"test"));
  }
END_TEST

Suite *job_route_suite(void)
  {
  Suite *s = suite_create("job_route_suite methods");
  TCase * tc = tcase_create("job_route");

  tcase_add_test(tc, test_add_dest_null);
  tcase_add_test(tc, test_add_dest);
  tcase_add_test(tc, test_queue_route_null);
  tcase_add_test(tc, test_is_bad_dest);
  
  suite_add_tcase(s, tc);
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
  sr = srunner_create(job_route_suite());
  srunner_set_log(sr, "job_route_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
