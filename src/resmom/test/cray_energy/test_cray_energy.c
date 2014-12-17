#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include "pbs_job.h"
#include "test_cray_energy.h"

#include "pbs_error.h"

extern job* alloc_job();
extern u_long get_energy_from_cray_file(const char *path,job *pjob);
extern std::string get_path_to_rur_log(const char *configPath);

START_TEST(test_get_energy_from_file)
  {
  job *pj = alloc_job();
  strcpy(pj->ji_qs.ji_jobid,"860.opal-p1");
  u_long joules = get_energy_from_cray_file("./test_log_2up01",pj);
  fail_unless(joules == 5186);

  joules = get_energy_from_cray_file("./test_log_2up00",pj);
  fail_unless(joules == 5206);

  }
END_TEST

START_TEST(test_get_path_to_rur_file)
  {
  std::string p = get_path_to_rur_log("./rur.conf");

  fail_unless(strcmp(p.c_str(),"/lus/scratch/RUR/output/rur.output") == 0);
  }
END_TEST

Suite *mom_job_func_suite(void)
  {
  Suite *s = suite_create("cray_energy_suite methods");
  TCase *tc_core = tcase_create("test_get_energy_from_file");
  tcase_add_test(tc_core, test_get_energy_from_file);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_get_path_to_rur_file");
  tcase_add_test(tc_core, test_get_path_to_rur_file);
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
  sr = srunner_create(mom_job_func_suite());
  srunner_set_log(sr, "cray_energy_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
