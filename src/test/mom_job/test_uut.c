#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include <stdlib.h>
#include <stdio.h>

#include "pbs_job.h"

#include "pbs_error.h"
#include <check.h>

extern const std::string NUMNODES;
extern const std::string PORTOUT;
extern const std::string PORTERR;


START_TEST(test_join_json)
  {
  mom_job *pjob = new mom_job();

  pjob->ji_numnodes=10;
  pjob->ji_portout=10020;
  pjob->ji_porterr=10021;

  Json::Value join_json;
  pjob->join_job_info_to_json(join_json);

  fail_unless(join_json["numnodes"].asInt() == 10);
  fail_unless(join_json["portout"].asInt() == 10020);
  fail_unless(join_json["porterr"].asInt() == 10021);

  mom_job *pjob2 = new mom_job();

  pjob2->initialize_joined_job_from_json(join_json);
  fail_unless(pjob2->ji_numnodes == 10);
  fail_unless(pjob2->ji_portout == 10020);
  fail_unless(pjob2->ji_porterr == 10021);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *mom_job_suite(void)
  {
  Suite *s = suite_create("mom_job_suite methods");
  TCase *tc_core = tcase_create("test_join_json");
  tcase_add_test(tc_core, test_join_json);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
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
  sr = srunner_create(mom_job_suite());
  srunner_set_log(sr, "mom_job_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
