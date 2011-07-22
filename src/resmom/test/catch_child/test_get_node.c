#include "test_catch_child.h"
#include "catch_child.h"
#include "pbs_job.h"
#include <string.h>
#include <stdlib.h>


/*
 * Current test cases consist of:
 * Not found (not jobs)
 * Not found (multiple jobs)
 * Found (multiple jobs)
 */

void init_job(job *pjob)
  {
  memset(pjob, 0, sizeof(job));
  pjob->ji_numvnod = 2;
  pjob->ji_vnods = (vnodent *)calloc(pjob->ji_numvnod, sizeof(vnodent));
  pjob->ji_vnods[0].vn_node = 1;
  pjob->ji_vnods[0].vn_host = (hnodent *)calloc(sizeof(hnodent), 1);
  pjob->ji_vnods[0].vn_host->hn_node = 1;
  pjob->ji_vnods[1].vn_node = 2;
  pjob->ji_vnods[1].vn_host = (hnodent *)calloc(sizeof(hnodent), 1);
  pjob->ji_vnods[1].vn_host->hn_node = 2;
  }

void cleanup_job(job *pjob)
  {
  free(pjob->ji_vnods[0].vn_host);
  free(pjob->ji_vnods[1].vn_host);
  free(pjob->ji_vnods);
  pjob->ji_vnods = NULL;
  }

START_TEST(test_get_node_1)
  {
  job pjob;
  tm_node_id nodeid = 1;
  init_job(&pjob);
  hnodent *res;
  pjob.ji_numvnod = 0;
  res = get_node(&pjob, nodeid);
  fail_unless(res == NULL, "No data passed in, nothing should be found");
  cleanup_job(&pjob);
  }
END_TEST

START_TEST(test_get_node_2)
  {
  job pjob;
  tm_node_id nodeid = 3;
  init_job(&pjob);
  hnodent *res;
  res = get_node(&pjob, nodeid);
  fail_unless(res == NULL, "Node should not have been found");
  cleanup_job(&pjob);
  }
END_TEST

START_TEST(test_get_node_3)
  {
  job pjob;
  tm_node_id nodeid = 2;
  int res_int = 0;
  init_job(&pjob);
  hnodent *res;
  res = get_node(&pjob, nodeid);
  fail_if(res == NULL, "A valid node should have been returned");
  fail_unless(res->hn_node == 2, "Incorrect node was returned");
  cleanup_job(&pjob);
  }
END_TEST

Suite *get_node_suite(void)
  {
  Suite *s = suite_create("get_node methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_get_node_1);
  tcase_add_test(tc_core, test_get_node_2);
  tcase_add_test(tc_core, test_get_node_3);
  suite_add_tcase(s, tc_core);
  return s;
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = srunner_create(get_node_suite());
  srunner_set_log(sr, "get_node_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
