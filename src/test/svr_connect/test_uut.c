#include "license_pbs.h" /* See here for the software license */
#include "svr_connect.h"
#include "test_svr_connect.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"


extern bool free_node_on_unlock;
extern int  node_unlocked;
extern int  find_node_called;

int connect_while_handling_mutex(pbs_net_t hostaddr, unsigned int port, char *EMsg, struct pbsnode **pnode);


START_TEST(test_connect_while_handling_mutex)
  {
  struct pbsnode *pnode = (struct pbsnode *)calloc(1, sizeof(struct pbsnode));

  find_node_called = 0;
  node_unlocked = 0;
  connect_while_handling_mutex(0, 0, NULL, NULL);
  fail_unless(find_node_called == 0);
  fail_unless(node_unlocked == 0);

  // This test will segfault without the fix for TRQ-2997.
  free_node_on_unlock = true;
  connect_while_handling_mutex(0, 0, NULL, &pnode);
  fail_unless(node_unlocked == 1);
  fail_unless(find_node_called == 1);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *svr_connect_suite(void)
  {
  Suite *s = suite_create("svr_connect_suite methods");
  TCase *tc_core = tcase_create("test_connect_while_handling_mutex");
  tcase_add_test(tc_core, test_connect_while_handling_mutex);
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
  sr = srunner_create(svr_connect_suite());
  srunner_set_log(sr, "svr_connect_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
