#include "license_pbs.h" /* See here for the software license */
#include "test_momctl.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "momctl.h"

extern int   flush_rc;
extern char *string_read;

enum MOMCmdEnum
  {
  momNONE = 0,
  momClear,
  momQuery,
  momReconfig,
  momShutdown,
  momLayout
  };

START_TEST(test_do_mom)
  {
  flush_rc = -1;
  fail_unless(do_mom(NULL, 0, momLayout) == -1, "Failure to flush should cause an error");

  flush_rc = 0;
  string_read = NULL;
  fail_unless(do_mom(NULL, 0, momLayout) == -1, "NULL reply from mom should fail");

  string_read = strdup("");
  fail_unless(do_mom(NULL, 0, momLayout) == 0, "Basic case should succeed");
  }
END_TEST


START_TEST(test_two)
  {


  }
END_TEST


Suite *momctl_suite(void)
  {
  Suite *s = suite_create("momctl_suite methods");
  TCase *tc_core = tcase_create("test_do_mom");
  tcase_add_test(tc_core, test_do_mom);
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
  sr = srunner_create(momctl_suite());
  srunner_set_log(sr, "momctl_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
