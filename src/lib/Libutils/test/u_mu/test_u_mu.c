#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "test_u_mu.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

char buf[4096];

START_TEST(threadsafe_tokenizer_test)
  {
  char  my_str[] = "one,two,three,four";
  char *strings[] = { "one", "two", "three", "four" };
  char *delim = ",";
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

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *u_mu_suite(void)
  {
  Suite *s = suite_create("u_mu_suite methods");
  TCase *tc_core = tcase_create("threadsafe_tokenizer_test");
  tcase_add_test(tc_core, threadsafe_tokenizer_test);
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
  sr = srunner_create(u_mu_suite());
  srunner_set_log(sr, "u_mu_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
