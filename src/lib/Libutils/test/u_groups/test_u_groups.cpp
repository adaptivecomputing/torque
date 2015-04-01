#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "test_u_groups.h"
#include <stdlib.h>
#include <stdio.h>
#include <grp.h>

#include "pbs_error.h"

START_TEST(null_check)
  {
  char *grp_name = NULL;
  struct group *grp = NULL;
  grp = getgrnam_ext(grp_name);
  fail_unless(grp == NULL, "null check failed, value being returned ?!?");
  }
END_TEST

START_TEST(find_by_name)
  {
  /* Check by name */
  char *grp_name = strdup("root");
  struct group *grp = NULL;
  grp = getgrnam_ext(grp_name);
  fail_if(grp == NULL, "root group not found???");
  fail_unless(strcmp(grp->gr_name, grp_name) == 0, "incorrect group found");
  free(grp_name);
  }
END_TEST

START_TEST(find_by_value)
  {
  /* Check by name */
  char *grp_name = strdup("0");
  struct group *grp = NULL;
  grp = getgrnam_ext(grp_name);
  fail_if(grp == NULL, "root group not found???");
  fail_unless(strcmp(grp->gr_name, "root") == 0, "incorrect group found");
  free(grp_name);
  }
END_TEST

START_TEST(unknown_name)
  {
  /* Check by name */
  char *grp_name = strdup("abc");
  struct group *grp = NULL;
  grp = getgrnam_ext(grp_name);
  fail_unless(grp == NULL, "group abc found?!?");
  free(grp_name);
  }
END_TEST

Suite *u_groups_suite(void)
  {
  Suite *s = suite_create("u_groups_suite methods");
  TCase *tc_core = tcase_create("null_check");
  tcase_add_test(tc_core, null_check);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("find_by_name");
  tcase_add_test(tc_core, find_by_name);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("find_by_value");
  tcase_add_test(tc_core, find_by_value);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("unknown_name");
  tcase_add_test(tc_core, unknown_name);
  suite_add_tcase(s, tc_core);

  return s;
  }

void rundebug()
  {
  /*
  char *grp_name = strdup("root");
  struct group *grp = NULL;
  grp == getgrnam_ext(grp_name);*/
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(u_groups_suite());
  srunner_set_log(sr, "u_groups_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
