#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_tm.h"
#include <stdlib.h>
#include <stdio.h>

#include "tm.h"

#include "pbs_error.h"

typedef int tm_event_t;

extern int init_done;

int fake_tm_init(

  void             *info,  /* in, currently unused */
  struct  tm_roots *roots) /* out */

  {
  tm_event_t  nevent, revent;
  char   *env, *hold;
  int   err;
  int   nerr = 0;
  struct tcp_chan *chan = NULL;

  init_done = 1;

  nevent = new_event();

  add_event(nevent, TM_ERROR_NODE, TM_INIT, (void *)roots);
  tm_poll(TM_NULL_EVENT, &revent, 1, &nerr);
  }


START_TEST(test_tm_poll_bad_init)
  {
  fail_unless(TM_BADINIT == tm_poll(0, 0, 0, 0), "expected poll to return TM_BADINIT if no init called first.");
  }
END_TEST

START_TEST(test_tm_poll_bad_result)
  {
  struct tm_roots roots;
  fake_tm_init(NULL, &roots);
  fail_unless(TM_EBADENVIRONMENT == tm_poll(0, 0, 0, 0), "expected poll to return TM_BADINIT if no init called first.");
  }
END_TEST

Suite *tm_suite(void)
  {
  Suite *s = suite_create("tm_suite methods");
  TCase * tc = tcase_create("tm");

  tcase_add_test(tc, test_tm_poll_bad_init);
  tcase_add_test(tc, test_tm_poll_bad_result);
  
  suite_add_tcase(s, tc);
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
  sr = srunner_create(tm_suite());
  srunner_set_log(sr, "tm_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
