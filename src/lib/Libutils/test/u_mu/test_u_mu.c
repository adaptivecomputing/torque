#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "test_u_mu.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

char  buf[4096];
char *strings[] = { (char *)"one", (char *)"two", (char *)"three", (char *)"four" };

START_TEST(threadsafe_tokenizer_test)
  {
  char  my_str[] = "one,two,three,four";
  char *delim = (char *)",";
  char *tok;
  char *str_ptr = my_str;
  int   i = 0;

  while ((tok = threadsafe_tokenizer(&str_ptr, delim)) != NULL)
    {
    fail_unless(i < 4, "Too many strings were returned");

    snprintf(buf, sizeof(buf),
      "token should be '%s' but is '%s' on iteration %d",
      strings[i], tok, i);
    fail_unless((!strcmp(tok, strings[i])), buf);
    i++;
    }

  fail_unless(threadsafe_tokenizer(NULL, NULL) == NULL, "returned a non-null for a null string");
  tok = NULL;
  fail_unless(threadsafe_tokenizer(&tok, NULL) == NULL, "returned a non-null for a null string");
  }
END_TEST

START_TEST(safe_strncat_test)
  {
  char *names[] = { (char *)"slartibartfast", (char *)"arthur dent", (char *)"trillian", (char *)"zaphod beeblebrox", (char *)"ford prefect", (char *)"marvin the robot" };
  /*int   name_lens[] = {15, 12, 9, 18, 13, 17};*/
  char  small_buf[32];
  int   ret;

  strcpy(small_buf, names[0]);
  ret = safe_strncat(small_buf, names[1], sizeof(small_buf) - strlen(small_buf) - 1);
  fail_unless(ret == PBSE_NONE, "reported not enough space when space exists");
  ret = safe_strncat(small_buf, names[2], sizeof(small_buf) - strlen(small_buf) - 1);
  fail_unless(ret == -1, "reported enough space when space does not exist");

  memset(small_buf, 0, sizeof(small_buf));

  strcpy(small_buf, names[3]);
  ret = safe_strncat(small_buf, names[4], sizeof(small_buf) - strlen(small_buf) - 1);
  fail_unless(ret == PBSE_NONE, "reported not enough space when space exists (round 2)");
  ret = safe_strncat(small_buf, names[5], sizeof(small_buf) - strlen(small_buf) - 1);
  fail_unless(ret == -1, "reported enough space when space does not exist (round 2)");
  }
END_TEST


START_TEST(trim_whitespace_test)
  {
  char *strs[] = { (char *)"   one ", (char *)" \ntwo", (char *)"three   ", (char *)" four", (char *)"five ", (char *)" six ", (char *)"\t" };
  char  firsts[] = { 'o', 't', 't', 'f', 'f', 's', '\0' };
  char  lasts[] = { 'e', 'o', 'e', 'r', 'e', 'x' };
  int   num_strs = 7;
  int   i;
  int   last;
  char *duped;
  char *retd;

  fail_unless(is_whitespace(' ') == TRUE, "doesn't think a space is whitespace");
  fail_unless(is_whitespace('\n') == TRUE, "doesn't think a newline is whitespace");
  fail_unless(is_whitespace('\f') == TRUE, "doesn't think a formfeed is whitespace");
  fail_unless(is_whitespace('\t') == TRUE, "doesn't think a tab is whitespace");
  fail_unless(is_whitespace('\r') == TRUE, "doesn't think a carriage return is whitespace");

  for (i = 0; i < num_strs; i++)
    {
    duped = strdup(strs[i]);
    retd = trim(duped);
    snprintf(buf, sizeof(buf), "didn't trim the front correctly, found '%s' but expected to start with %c",
      retd, firsts[i]);
    fail_unless(retd[0] == firsts[i], buf);
    last = strlen(retd) - 1;
    if (last > 0)
      {
      snprintf(buf, sizeof(buf), "didn't trim the back correctly, found '%s' but expected to end with %c",
        retd, lasts[i]);
      fail_unless(retd[last] == lasts[i], buf);
      }
    }

  fail_unless(trim(NULL) == NULL, "returned a non-null for null string");
  }
END_TEST


Suite *u_mu_suite(void)
  {
  Suite *s = suite_create("u_mu_suite methods");
  TCase *tc_core = tcase_create("threadsafe_tokenizer_test");
  tcase_add_test(tc_core, threadsafe_tokenizer_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("safe_strncat_test");
  tcase_add_test(tc_core, safe_strncat_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("trim_whitespace_test");
  tcase_add_test(tc_core, trim_whitespace_test);
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
  sr = srunner_create(u_mu_suite());
  srunner_set_log(sr, "u_mu_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
