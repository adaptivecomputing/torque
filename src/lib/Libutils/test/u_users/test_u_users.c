#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "test_u_users.h"
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>

#include "pbs_error.h"

struct passwd *get_password_entry_by_uid(char **user_buf, uid_t uid);
extern int uid_i;
extern int nam_i;

START_TEST(get_password_entry_by_uid_test)
  {
  char *buf;
  struct passwd *root = get_password_entry_by_uid(&buf, 0);
  fail_unless(uid_i == 6);
  fail_unless(root != NULL);
  }
END_TEST

START_TEST(null_check)
  {
  char *user_buf = NULL;
  char *user_name = NULL;
  struct passwd *ret_user = NULL;
  ret_user = getpwnam_ext(&user_buf, user_name);
  fail_unless(ret_user == NULL, "null shouldn't work as user_name");
  }
END_TEST

START_TEST(invalid_user)
  {
  char *user_buf = NULL;
  char *user_name = strdup("unknown_user");
  struct passwd *ret_user = NULL;
  ret_user = getpwnam_ext(&user_buf, user_name);
  fail_unless(ret_user == NULL, "unknown_user is a valid user????");
  free(user_name);
  }
END_TEST

START_TEST(valid_user)
  {
  char *user_buf = NULL;
  char *user_name = strdup("root");
  struct passwd *ret_user = NULL;
  ret_user = getpwnam_ext(&user_buf, user_name);
  fail_if(ret_user == NULL, "unknown_user is a valid user????");
  fail_unless(strcmp(user_name, ret_user->pw_name) == 0, "Incorrect user returned (%s != %s)", user_name, ret_user->pw_name);
  free(user_name);
  }
END_TEST

START_TEST(invalid_user_with_error)
  {
  char *user_buf = NULL;
  /* How do I lock memory allocation again.... */
  char *user_name = strdup("unknown_user");
  struct passwd *ret_user = NULL;
  ret_user = getpwnam_ext(&user_buf, user_name);
  fail_unless(ret_user == NULL, "unknown_user is a valid user????");
  free(user_name);
  }
END_TEST

Suite *u_users_suite(void)
  {
  Suite *s = suite_create("u_users_suite methods");
  TCase *tc_core = tcase_create("null_check");
  tcase_add_test(tc_core, null_check);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("invalid_user");
  tcase_add_test(tc_core, invalid_user);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("valid_user");
  tcase_add_test(tc_core, valid_user);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("invalid_user_with_error");
  tcase_add_test(tc_core, invalid_user_with_error);
  tcase_add_test(tc_core, get_password_entry_by_uid_test);
  suite_add_tcase(s, tc_core);
  return s;
  }

void rundebug()
  {
  /*char *user_name = strdup("root");
  struct passwd *ret_user = NULL;
  ret_user = getpwnam_ext(user_name);
  fail_if(ret_user == NULL, "unknown_user ");
  free(user_name);*/
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(u_users_suite());
  srunner_set_log(sr, "u_users_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
