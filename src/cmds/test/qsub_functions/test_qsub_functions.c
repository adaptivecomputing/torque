#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "qsub_functions.h"
#include "test_qsub_functions.h"
#include "complete_req.hpp"

void process_opt_L(job_info *ji, const char *str);
void validate_basic_resourcing(job_info *ji);
void add_new_request_if_present(job_info *ji);

extern complete_req cr;
extern bool         submission_string_fail;
extern bool         added_req;
extern bool         find_nodes;
extern bool         find_mpp;
extern bool         find_size;

START_TEST(test_process_opt_L)
  {
  job_info    ji;
  const char *arg = "tasks=6";

  added_req = false;
  process_opt_L(&ji, arg);
  fail_unless(added_req == true);
  }
END_TEST

/*
START_TEST(test_process_opt_L_fail1)
  {
  job_info    ji;
  // create a bad string failure
  process_opt_L(&ji, "bob");
  }
END_TEST


START_TEST(test_process_opt_L_fail2)
  {
  job_info    ji;
  const char *arg = "tasks=6";

  submission_string_fail = true;
  process_opt_L(&ji, arg);
  }
END_TEST*/


START_TEST(test_add_new_request_if_present)
  {
  extern int req_val;
  extern bool stored_complete_req;
  job_info    ji;

  req_val = 3;
  stored_complete_req = false;
  add_new_request_if_present(&ji);
  fail_unless(stored_complete_req == true);
  }
END_TEST

START_TEST(test_validate_basic_resourcing)
  {
  extern int req_val;
  job_info   ji;

  // as long we don't exit here we're good, all failures exit
  req_val = 3;
  find_mpp = false;
  find_nodes = false;
  find_size = false;
  validate_basic_resourcing(&ji);

  req_val = 0;
  find_mpp = true;
  validate_basic_resourcing(&ji);

  find_mpp = false;
  find_nodes = true;
  validate_basic_resourcing(&ji);

  find_nodes = false;
  find_size = true;
  validate_basic_resourcing(&ji);

  find_size = false;
  // none should also work
  validate_basic_resourcing(&ji);

  }
END_TEST




START_TEST(test_x11_get_proto_1)
  {
  char *param1 = NULL;
  char *resp = NULL;
  int debug = 1;
  resp = x11_get_proto(param1, debug);
  fail_unless(resp == NULL);
  }
END_TEST


START_TEST(test_isWindowsFormat)
  {
  char *tempfilename = tempnam("/tmp", "test");
  fail_unless((tempfilename != NULL), "Failed to create a temporary filename");
  FILE *fp = fopen(tempfilename, "w");
  fail_unless(fp != NULL, "Failed to create a file to test on Windows text format");
  fprintf(fp, "line 1, blah blah blah\n");
  fprintf(fp, "line 2, blah blah blah\n");
  fprintf(fp, "line 3, blah blah blah\n");
  int s = fclose(fp);
  fail_unless(s == 0, "Failed to close test file");
  char command[1024];
  snprintf(command, sizeof(command), "/usr/bin/unix2dos %s > /dev/null", tempfilename);
  s = system(command);
  fail_unless((s != -1), "Failed to execute %s", command);
  fp = fopen(tempfilename, "r");
  fail_unless(fp != NULL, "Failed to open file %s for read", tempfilename);
  //s = isWindowsFormat(fp);
  //fail_unless(s==1, "Failed to detect Windows format text file");
  fclose(fp);
  snprintf(command, sizeof(command), "/usr/bin/dos2unix %s > /dev/null", tempfilename);
  s = system(command);
  fail_unless((s != -1), "Failed to execute %s", command);
  fp = fopen(tempfilename, "r");
  fail_unless(fp != NULL, "Failed to open file %s for read", tempfilename);
  s = isWindowsFormat(fp);
  fail_unless(s==0, "Failed to detect UNIX format text file");
  fclose(fp);
  unlink(tempfilename);
  }
END_TEST

Suite *qsub_functions_suite(void)
  {
  Suite *s = suite_create("qsub_functions methods");
  TCase *tc_core = tcase_create("test x11_get_proto_1");
  tcase_add_test(tc_core, test_x11_get_proto_1);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test isWindowsFormat");
  tcase_add_test(tc_core, test_isWindowsFormat);
  tcase_add_test(tc_core, test_process_opt_L);
//  tcase_add_exit_test(tc_core, test_process_opt_L_fail1, 2);
//  tcase_add_exit_test(tc_core, test_process_opt_L_fail2, 2);
  tcase_add_test(tc_core, test_add_new_request_if_present);
  tcase_add_test(tc_core, test_validate_basic_resourcing);
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

  sr = srunner_create(qsub_functions_suite());
  srunner_set_log(sr, "qsub_functions_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
