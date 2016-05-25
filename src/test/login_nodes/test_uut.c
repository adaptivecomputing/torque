#include <stdio.h>

#include "login_nodes.h"
#include "pbs_nodes.h"
#include "attribute.h"
#include "job_usage_info.hpp"
#include <check.h>

extern login_holder logins;
char                buf[4096];
int proplist(char **str, struct prop **plist, int *node_req, int *gpu_req);
struct pbsnode *check_node(login_node *ln, struct prop *needed);
void update_next_node_index(unsigned int to_beat, struct pbsnode *held);


void initialize_node_for_testing(

  struct pbsnode *pnode)

  {
  memset(pnode, 0, sizeof(struct pbsnode));
  for (int i = 0; i < 5; i++)
    pnode->nd_slots.add_execution_slot();
  }


START_TEST(initialize_test)
  {
  initialize_login_holder();

  fail_unless(logins.next_node == 0, "Next node set incorrectly");
  fail_unless(logins.ln_mutex != 0, "Mutex wasn't initialized correctly");
  fail_unless(logins.iterate_backwards == 0, "Set to iterate backwards from beginning");
  }
END_TEST




START_TEST(add_test)
  {
  struct pbsnode *n1 = (struct pbsnode *)1;
  struct pbsnode *n2 = (struct pbsnode *)2;
  struct pbsnode *n3 = (struct pbsnode *)3;
  struct pbsnode *n4 = (struct pbsnode *)4;
  int            rc;
  
  initialize_login_holder();

  rc = add_to_login_holder(n1);
  fail_unless(rc == 0, "Couldn't add n1");

  rc = add_to_login_holder(n2);
  fail_unless(rc == 0, "Couldn't add n2");

  rc = add_to_login_holder(n3);
  fail_unless(rc == 0, "Couldn't add n3");

  rc = add_to_login_holder(n4);
  fail_unless(rc == 0, "Couldn't add n4");
  }
END_TEST




void increment_counts(

  struct pbsnode *n1,
  struct pbsnode *n2,
  struct pbsnode *n3,
  struct pbsnode *n4,
  struct pbsnode *rtd,
  int            *n1_cnt,
  int            *n2_cnt,
  int            *n3_cnt,
  int            *n4_cnt)

  {
  if (rtd == n1)
    *n1_cnt += 1;
  else if (rtd == n2)
    *n2_cnt += 1;
  else if (rtd == n3)
    *n3_cnt += 1;
  else if (rtd == n4)
    *n4_cnt += 1;
  } /* END increment_counts() */



START_TEST(retrieval_test)
  {
  struct pbsnode  n1;
  struct pbsnode  n2;
  struct pbsnode  n3;
  struct pbsnode  n4;
  struct pbsnode *rtd;
  int             rc;
  int             n1_rtd = 0;
  int             n2_rtd = 0;
  int             n3_rtd = 0;
  int             n4_rtd = 0;

  initialize_login_holder();
  initialize_node_for_testing(&n1);
  initialize_node_for_testing(&n2);
  initialize_node_for_testing(&n3);
  initialize_node_for_testing(&n4);
  rc = add_to_login_holder(&n1);
  fail_unless(rc == 0);
  rc = add_to_login_holder(&n2);
  fail_unless(rc == 0);
  rc = add_to_login_holder(&n3);
  fail_unless(rc == 0);
  rc = add_to_login_holder(&n4);
  fail_unless(rc == 0);

  rtd = get_next_login_node(NULL);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  rtd = get_next_login_node(NULL);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  rtd = get_next_login_node(NULL);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  rtd = get_next_login_node(NULL);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  snprintf(buf, sizeof(buf), "Should have used n1 once but is %d", n1_rtd);
  fail_unless(n1_rtd == 1, buf);
  snprintf(buf, sizeof(buf), "Should have used n2 once but is %d", n2_rtd);
  fail_unless(n2_rtd == 1, buf);
  snprintf(buf, sizeof(buf), "Should have used n3 once but is %d", n3_rtd);
  fail_unless(n3_rtd == 1, buf);
  snprintf(buf, sizeof(buf), "Should have used n4 once but is %d", n4_rtd);
  fail_unless(n4_rtd == 1, buf);
  
  rtd = get_next_login_node(NULL);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  rtd = get_next_login_node(NULL);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  rtd = get_next_login_node(NULL);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  rtd = get_next_login_node(NULL);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  fail_unless(n1_rtd == 2, "Should have used n1 twice");
  fail_unless(n2_rtd == 2, "Should have used n2 twice");
  fail_unless(n3_rtd == 2, "Should have used n3 twice");
  fail_unless(n4_rtd == 2, "Should have used n4 twice");

  /* Set a node down and make sure it doesn't get used */
  n2.nd_state = INUSE_DOWN;

  n1.nd_name = strdup("n1");
  n2.nd_name = strdup("n2");
  n3.nd_name = strdup("n3");
  n4.nd_name = strdup("n4");

  rtd = get_next_login_node(NULL);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  rtd = get_next_login_node(NULL);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  rtd = get_next_login_node(NULL);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  fail_unless(((n1_rtd == 3) && (n2_rtd == 2) && (n3_rtd == 3) && (n4_rtd == 3)),
    "Should have used n1,n2,n3,n4 3,2,3,3 times but found %d,%d,%d,%d",
    n1_rtd, n2_rtd, n3_rtd, n4_rtd);
  
  // Make sure load balancing works
  n2.nd_state = INUSE_FREE;
  for (int i = 0; i < 51; i++)
    {
    job_usage_info ju(i);
    n1.nd_job_usages.push_back(ju);
    }
  
  // Make sure we don't pick n1 as the next node 
  for (int i = 0; i < 200; i++)
    {
    update_next_node_index(0, NULL);
    fail_unless(logins.next_node != 0);
    }
  
  for (int i = 0; i < 51; i++)
    {
    job_usage_info ju(i);
    n2.nd_job_usages.push_back(ju);
    }
  
  // make sure we're excluding n1 and n2
  for (int i = 0; i < 200; i++)
    {
    update_next_node_index(0, NULL);
    fail_unless(logins.next_node != 0);
    fail_unless(logins.next_node != 1);
    }
  
  for (int i = 0; i < 51; i++)
    {
    job_usage_info ju(i);
    n3.nd_job_usages.push_back(ju);
    }

  // Make sure we use n4, the only empty node
  for (int i = 0; i < 200; i++)
    {
    update_next_node_index(0, NULL);
    fail_unless(logins.next_node == 3);
    }
  
  for (int i = 0; i < 51; i++)
    {
    job_usage_info ju(i);
    n4.nd_job_usages.push_back(ju);
    }

  n1.nd_job_usages.clear();
  // now n1 is empty 
  for (int i = 0; i < 200; i++)
    {
    update_next_node_index(0, NULL);
    fail_unless(logins.next_node == 0);
    }

  n2.nd_job_usages.clear();
  for (int i = 0; i < 200; i++)
    {
    update_next_node_index(0, NULL);
    fail_unless(logins.next_node == 0 || logins.next_node == 1);
    }

  n3.nd_job_usages.clear();
  for (int i = 0; i < 200; i++)
    {
    update_next_node_index(0, NULL);
    fail_unless(logins.next_node == 0 || logins.next_node == 1 || logins.next_node == 2);
    }
  }
END_TEST


START_TEST(check_node_test)
  {
  struct pbsnode pnode;
  login_node ln(&pnode);

  memset(&pnode, 0, sizeof(pnode));

  pnode.nd_slots.add_execution_slot();
  fail_unless(check_node(&ln, NULL) != NULL);
  pnode.nd_slots.mark_as_used(0);
  fail_unless(check_node(&ln, NULL) == NULL);

  }
END_TEST


START_TEST(prop_test)
  {
  struct pbsnode        n1;
  struct pbsnode        n2;
  struct pbsnode        n3;
  struct pbsnode        n4;
  struct pbsnode       *rtd;
  int                   rc;
  int                   dummy1;
  int                   dummy2;
  int                   n1_rtd = 0;
  int                   n2_rtd = 0;
  int                   n3_rtd = 0;
  int                   n4_rtd = 0;
  char                 *feature = (char *)"tom";
  char                 *feature2 = (char *)"bob";
  struct prop          *props = NULL;
  struct prop          *props2 = NULL;

  initialize_node_for_testing(&n1);
  initialize_node_for_testing(&n2);
  initialize_node_for_testing(&n3);
  initialize_node_for_testing(&n4);

  proplist(&feature, &props, &dummy1, &dummy2);
  proplist(&feature2, &props2, &dummy1, &dummy2);

  n1.nd_first = props;
  n2.nd_first = props;

  initialize_login_holder();
  rc = add_to_login_holder(&n1);
  fail_unless(rc == 0);
  rc = add_to_login_holder(&n2);
  fail_unless(rc == 0);
  rc = add_to_login_holder(&n3);
  fail_unless(rc == 0);
  rc = add_to_login_holder(&n4);
  fail_unless(rc == 0);

  rtd = get_next_login_node(props);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  rtd = get_next_login_node(props);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  snprintf(buf, sizeof(buf), "Should have used n1 once but is %d", n1_rtd);
  fail_unless(n1_rtd == 1, buf);
  snprintf(buf, sizeof(buf), "Should have used n2 once but is %d", n2_rtd);
  fail_unless(n2_rtd == 1, buf);
  snprintf(buf, sizeof(buf), "Shouldn't have used n3 but is %d", n3_rtd);
  fail_unless(n3_rtd == 0, buf);
  snprintf(buf, sizeof(buf), "Shouldn't have used n4 but is %d", n4_rtd);
  fail_unless(n4_rtd == 0, buf);

  rtd = get_next_login_node(props);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  rtd = get_next_login_node(props);
  increment_counts(&n1, &n2, &n3, &n4, rtd, &n1_rtd, &n2_rtd, &n3_rtd, &n4_rtd);
  fail_unless(n1_rtd == 2, "Should have used n1 twice");
  fail_unless(n2_rtd == 2, "Should have used n2 twice");
  fail_unless(n3_rtd == 0, "Shouldn't have used n3");
  fail_unless(n4_rtd == 0, "Shouldn't have used n4");

  fail_unless(get_next_login_node(props2) == NULL, "Somehow found a node when none have the property");
  }
END_TEST




Suite *node_func_suite(void)
  {
  Suite *s = suite_create("alps helper suite methods");

  TCase *tc_core = tcase_create("initialize_test");
  tcase_add_test(tc_core, initialize_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("add_test");
  tcase_add_test(tc_core, add_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("retrieval_test");
  tcase_add_test(tc_core, retrieval_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("prop_test");
  tcase_add_test(tc_core, prop_test);
  tcase_add_test(tc_core, check_node_test);
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

