#include "license_pbs.h" /* See here for the software license */
#include "pbs_log.h"
#include "test_pbs_log.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include <string>

#include "pbs_error.h"

extern bool dot;
extern bool double_dot;
extern bool first_time;
extern bool dir_is_null;
extern bool time_expired;
extern bool stat_fail;

START_TEST(test_one)
  {
  int rc;
  char *null_path = NULL;
  char path[20];

  /* Validate input */
  /* NULL path */
  rc = log_remove_old(null_path, 100);
  fail_unless(rc == -1);

  /* Expire time of 0 */
  strcpy(path, "some_dir");
  rc = log_remove_old(path, 0);
  fail_unless(rc == 0);

  dir_is_null = true;
  rc = log_remove_old(path, 10);
  fail_unless(rc == -1);

  /* get just a . for the directory entry */
  stat_fail = false;
  dir_is_null = false;
  dot = true;
  double_dot = false;
  first_time = true;

  rc = log_remove_old(path, 100);
  fail_unless(rc == 0);

  /* get just a .. for the directory entry */
  dot = false;
  double_dot = true;
  first_time = true;

  rc = log_remove_old(path, 100);
  fail_unless(rc == 0);

  /* Time not yet expired */
  dot = false;
  double_dot = false;
  first_time = true;
  time_expired = false;

  rc = log_remove_old(path, 100);
  fail_unless(rc == 0);

   /* Time expired */
  dot = false;
  double_dot = false;
  first_time = true;
  time_expired = true;

  rc = log_remove_old(path, 100);
  fail_unless(rc == 0);

   /* stat failure  */
  stat_fail = true;
  dot = false;
  double_dot = false;
  first_time = true;
  time_expired = true;

  rc = log_remove_old(path, 100);
  fail_unless(rc == 0);


  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *pbs_log_suite(void)
  {
  Suite *s = suite_create("pbs_log_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
  sr = srunner_create(pbs_log_suite());
  srunner_set_log(sr, "pbs_log_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
