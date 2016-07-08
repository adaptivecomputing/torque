#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <check.h>

#include "pbs_job.h"
#include "mom_func.h"

extern int encode_used_ctr;
extern int encode_flagged_attrs_ctr;
extern int MOMCudaVisibleDevices;
extern struct config *config_array;

void add_job_status_information(job &pjob, std::stringstream &list);
u_long setcudavisibledevices(const char *value);
unsigned long setjobstarterprivileged(const char *);

int jobstarter_privileged = 0;
char         PBSNodeMsgBuf[MAXLINE];
int          MOMJobDirStickySet;


START_TEST(test_add_job_status_information)
  {
  std::stringstream list;
  job pjob;

  encode_used_ctr = 0;
  encode_flagged_attrs_ctr = 0;
  add_job_status_information(pjob, list);

  fail_unless(!strcmp(list.str().c_str(), "()"));
  }
END_TEST


START_TEST(test_setcudavisibledevices)
  {
  std::stringstream output;

  setcudavisibledevices("1");
  fail_unless(MOMCudaVisibleDevices == 1, "did not set cuda_visble_devices to 1");

  setcudavisibledevices("0");
  fail_unless(MOMCudaVisibleDevices == 0, "did not set cuda_visble_devices to 0");
  }
END_TEST

START_TEST(test_setjobstarterprivileged)
  {
  fail_unless(setjobstarterprivileged("") == 1);
  fail_unless(jobstarter_privileged == 0);

  fail_unless(setjobstarterprivileged("off") == 1);
  fail_unless(jobstarter_privileged == 0);

  fail_unless(setjobstarterprivileged("0") == 1);
  fail_unless(jobstarter_privileged == 0);

  fail_unless(setjobstarterprivileged("false") == 1);
  fail_unless(jobstarter_privileged == 0);

  fail_unless(setjobstarterprivileged("on") == 1);
  fail_unless(jobstarter_privileged == TRUE);

  fail_unless(setjobstarterprivileged("t") == 1);
  fail_unless(jobstarter_privileged == TRUE);

  fail_unless(setjobstarterprivileged("T") == 1);
  fail_unless(jobstarter_privileged == TRUE);

  fail_unless(setjobstarterprivileged("1") == 1);
  fail_unless(jobstarter_privileged == TRUE);
  }
END_TEST


START_TEST(test_reqgres)
  {

  // allocate array
  fail_unless((config_array = (struct config *)calloc(2, sizeof(struct config))) != NULL);

  // basic test
  fail_unless(strcmp(reqgres(NULL), "") == 0);

  // fill in array
  config_array[0].c_name = strdup("somegres");
  config_array[0].c_u.c_value = strdup("somestuff");
  fail_unless(strcmp(reqgres(NULL), "somegres:somestuff") == 0);

  config_array[0].c_u.c_value = strdup("!fakeshellescape");
  fail_unless(strcmp(reqgres(NULL), "somegres:scriptoutput") == 0);
  }
END_TEST

Suite *parse_config_suite(void)
  {
  Suite *s = suite_create("parse_config test suite methods");
  TCase *tc_core = tcase_create("test_add_job_status_information");
  tcase_add_test(tc_core, test_add_job_status_information);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_setcudavisibledevices");
  tcase_add_test(tc_core, test_setcudavisibledevices);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_setjobstarterprivileged");
  tcase_add_test(tc_core, test_setjobstarterprivileged);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_reqgres");
  tcase_add_test(tc_core, test_reqgres);
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
  sr = srunner_create(parse_config_suite());
  srunner_set_log(sr, "parse_config_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
