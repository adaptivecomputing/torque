#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "alps_constants.h"
#include "pbs_nodes.h"

int set_ncpus(struct pbsnode *, char *);
int set_state(struct pbsnode *, char *);

char buf[4096];



START_TEST(set_ncpus_test)
  {
  struct pbsnode  pnode;
  char           *proc1 = "CPROC=2";
  char           *proc2 = "CPROC=4";
  char           *proc3 = "CPROC=8";

  pnode.nd_nsn = 0;
  fail_unless(set_ncpus(&pnode, proc1) == 0, "Couldn't set ncpus to 2");
  snprintf(buf, sizeof(buf), "ncpus should be 2 but is %d", pnode.nd_nsn);
  fail_unless(pnode.nd_nsn == 2, buf);

  pnode.nd_nsn = 0;
  fail_unless(set_ncpus(&pnode, proc2) == 0, "Couldn't set ncpus to 4");
  snprintf(buf, sizeof(buf), "ncpus should be 4 but is %d", pnode.nd_nsn);
  fail_unless(pnode.nd_nsn == 4, buf);

  pnode.nd_nsn = 0;
  fail_unless(set_ncpus(&pnode, proc3) == 0, "Couldn't set ncpus to 8");
  snprintf(buf, sizeof(buf), "ncpus should be 8 but is %d", pnode.nd_nsn);
  fail_unless(pnode.nd_nsn == 8, buf);
  }

END_TEST



START_TEST(set_state_test)
  {
  struct pbsnode  pnode;
  char           *up_str   = "state=UP";
  char           *down_str = "state=DOWN";

  memset(&pnode, 0, sizeof(pnode));

  set_state(&pnode, up_str);
  snprintf(buf, sizeof(buf), "Couldn't set state to up, state is %d", pnode.nd_state);
  fail_unless(pnode.nd_state == INUSE_FREE, buf);
  set_state(&pnode, down_str);
  fail_unless((pnode.nd_state & INUSE_DOWN) != 0, "Couldn't set the state to down");

  set_state(&pnode, up_str);
  fail_unless(pnode.nd_state == INUSE_FREE, "Couldn't set the state to up 2");
  set_state(&pnode, down_str);
  fail_unless((pnode.nd_state & INUSE_DOWN) != 0, "Couldn't set the state to down 2");
  }
END_TEST



START_TEST(whole_test)
  {
  }
END_TEST



Suite *node_func_suite(void)
  {
  Suite *s = suite_create("alps helper suite methods");
  TCase *tc_core = tcase_create("set_ncpus_test");
  tcase_add_test(tc_core, set_ncpus_test);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("set_state_test");
  tcase_add_test(tc_core, set_state_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("whole_test");
  tcase_add_test(tc_core, whole_test);
  suite_add_tcase(s, tc_core);
  
  return(s);
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(node_func_suite());
  srunner_set_log(sr, "node_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }

