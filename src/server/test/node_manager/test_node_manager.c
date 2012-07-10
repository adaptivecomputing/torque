#include "license_pbs.h" /* See here for the software license */
#include "node_manager.h"
#include "test_node_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

char *exec_hosts = "napali/0+napali/1+napali/2+napali/50+napali/4+l11/0+l11/1+l11/2+l11/3";
char  buf[4096];
char *napali = "napali";
char *l11 =    "l11";

int   node_in_exechostlist(char *, char *);
char *get_next_exec_host(char **);
int   job_should_be_on_node(char *, struct pbsnode *);

START_TEST(get_next_exec_host_test)
  {
  char *exec = strdup(exec_hosts);
  char *ptr = exec;
  int   num_hosts = 9;
  int   i = 0;
  char *hostname;

  while ((hostname = get_next_exec_host(&ptr)) != NULL)
    {
    i++;

    if (i > 5)
      {
      snprintf(buf, sizeof(buf), "Expected hostname '%s' but instead received '%s'",
        l11, hostname);
      fail_unless(!strcmp(l11, hostname), buf);
      }
    else
      {
      snprintf(buf, sizeof(buf), "Expected hostname '%s' but instead received '%s'",
        napali, hostname);
      fail_unless(!strcmp(napali, hostname), buf);
      }
    }

  fail_unless(i == num_hosts, "Returned the wrong number of host names");

  free(exec);
  }
END_TEST




START_TEST(job_should_be_on_node_test)
  {
  struct pbsnode pnode;
  struct pbssubn subnode;
  struct jobinfo jinfo;

  memset(&pnode, 0, sizeof(pnode));
  memset(&jinfo, 0, sizeof(jinfo));
  memset(&subnode, 0, sizeof(subnode));

  pnode.nd_name = "tom";
  pnode.nd_psn = &subnode;
  subnode.jobs = &jinfo;
  strcpy(jinfo.jobid, "1");

  fail_unless(job_should_be_on_node("2", &pnode) == FALSE, "non-existent job shouldn't be on node");
  fail_unless(job_should_be_on_node("3", &pnode) == FALSE, "non-existent job shouldn't be on node");
  fail_unless(job_should_be_on_node("4", &pnode) == FALSE, "non-existent job shouldn't be on node");
  fail_unless(job_should_be_on_node("1", &pnode) == TRUE, "false positive");
  fail_unless(job_should_be_on_node("5", &pnode) == TRUE, "false positive");
  }
END_TEST




START_TEST(node_in_exechostlist_test)
  {
  char *eh1 = "tom/0+bob/0";
  char *eh2 = "mytom/0+tommy/0+tom1/0";
  char *node1 = "tom";
  char *node2 = "bob";
  char *node3 = "mytom";
  char *node4 = "tommy";
  char *node5 = "tom1";

  fail_unless(node_in_exechostlist(node1, eh1) == TRUE, "blah1");
  fail_unless(node_in_exechostlist(node2, eh1) == TRUE, "blah2");
  fail_unless(node_in_exechostlist(node3, eh1) == FALSE, "blah3");
  fail_unless(node_in_exechostlist(node4, eh1) == FALSE, "blah4");
  fail_unless(node_in_exechostlist(node5, eh1) == FALSE, "blah5");
  
  fail_unless(node_in_exechostlist(node1, eh2) == FALSE, "blah6");
  fail_unless(node_in_exechostlist(node2, eh2) == FALSE, "blah7");
  fail_unless(node_in_exechostlist(node3, eh2) == TRUE, "blah8");
  fail_unless(node_in_exechostlist(node4, eh2) == TRUE, "blah9");
  fail_unless(node_in_exechostlist(node5, eh2) == TRUE, "blah10");
  }
END_TEST




Suite *node_manager_suite(void)
  {
  Suite *s = suite_create("node_manager_suite methods");
  TCase *tc_core = tcase_create("get_next_exec_host_test");
  tcase_add_test(tc_core, get_next_exec_host_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("job_should_be_on_node_test");
  tcase_add_test(tc_core, job_should_be_on_node_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("node_in_exechostlist_test");
  tcase_add_test(tc_core, node_in_exechostlist_test);
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
  sr = srunner_create(node_manager_suite());
  srunner_set_log(sr, "node_manager_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }


