
/*
 * This have to be included here, before check.h because it defines a function check() as a macro
 * that breaks STL basic_ios.h class definition
 */
#include "qsub_functions.h"

#include "test_qsub_functions.h"
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

  //write in dos format
  fprintf(fp, "line 1, blah blah blah\r\n");
  fprintf(fp, "line 2, blah blah blah\r\n");
  fprintf(fp, "line 3, blah blah blah\r\n");
  int s = fclose(fp);
  fp = fopen(tempfilename, "r");
  fail_unless(fp != NULL, "Failed to open file %s for read", tempfilename);
  s = isWindowsFormat(fp);
  fail_unless(s==1, "Failed to detect Windows format text file");
  fclose(fp);

  fopen(tempfilename, "w");
  fail_unless(fp != NULL, "Failed to create a file to test on Windows text format");
  
  //write in unix format
  fprintf(fp, "line 1, blah blah blah\n");
  fprintf(fp, "line 2, blah blah blah\n");
  fprintf(fp, "line 3, blah blah blah\n");
  s = fclose(fp);

  fp = fopen(tempfilename, "r");
  fail_unless(fp != NULL, "Failed to open file %s for read", tempfilename);
  s = isWindowsFormat(fp);
  fail_unless(s==0, "Failed to detect UNIX format text file");
  fclose(fp);
  unlink(tempfilename);
  }
END_TEST

START_TEST(test_make_argv)
  {
#define MAX_ARGV_LEN 16
  int argc;
  char *vect[MAX_ARGV_LEN + 1] = {};

  /* 0: "qsub"         1            2                3                     4    5    6   7 8 */
  char const * line = "simple_arg \"quoted ' arg\" \'s\"quoted \" arg\' \\\\ \\\" \\\' \\  end";
  make_argv(&argc, vect, line);
  fail_unless(argc == 9);
  fail_unless(strcmp(vect[0], "qsub") == 0);
  fail_unless(strcmp(vect[1], "simple_arg") == 0);
  fail_unless(strcmp(vect[2], "quoted ' arg") == 0);
  fail_unless(strcmp(vect[3], "s\"quoted \" arg") == 0);
  fail_unless(strcmp(vect[4], "\\") == 0);
  fail_unless(strcmp(vect[5], "\"") == 0);
  fail_unless(strcmp(vect[6], "\'") == 0);
  fail_unless(strcmp(vect[7], " ") == 0);
  fail_unless(strcmp(vect[8], "end") == 0);
  fail_unless(vect[9] == NULL);

  /* two args that are (escaped) spaces + test mem free/alloc no-fail test */
  line = "\\  \\\t";
  make_argv(&argc, vect, line);
  fail_unless(argc == 3);
  fail_unless(strcmp(vect[0], "qsub") == 0);
  fail_unless(strcmp(vect[1], " ") == 0);
  fail_unless(strcmp(vect[2], "\t") == 0);

  /* no arguments + mem free/alloc no-fail test */
  line = "      \t     ";
  make_argv(&argc, vect, line);
  fail_unless(argc == 1);
  fail_unless(strcmp(vect[0], "qsub") == 0);
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

  tc_core = tcase_create("test_make_argv");
  tcase_add_test(tc_core, test_make_argv);
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
