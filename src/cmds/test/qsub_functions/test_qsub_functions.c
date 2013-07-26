
#include "test_qsub_functions.h"
#include "qsub_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
  s = isWindowsFormat(fp);
  fail_unless(s==1, "Failed to detect Windows format text file");
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
