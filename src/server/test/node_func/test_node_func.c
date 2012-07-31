#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "test_node_func.h"
#include "node_func.h"
#include "pbs_error.h"
#include "resizable_array.h"


struct pbsnode *find_nodebyname(char *nodeid);
int cray_enabled;


void initialize_allnodes(all_nodes *an, struct pbsnode *n1, struct pbsnode *n2)
  {
  an->allnodes_mutex = calloc(1, sizeof(pthread_mutex_t));
  an->ra = calloc(1, sizeof(resizable_array));
  an->ra->slots = calloc(3, sizeof(slot));
  
  an->ra->slots[0].item = NULL;
  an->ra->slots[1].item = n1;
  an->ra->slots[2].item = n2;
  }



START_TEST(find_nodebyname_test)
  {
  struct pbsnode  node1;
  struct pbsnode  node2;
  struct pbsnode  reporter;
  struct pbsnode *pnode;

  alps_reporter = &reporter;

  memset(&node1, 0, sizeof(node1));
  memset(&node2, 0, sizeof(node2));

  node1.nd_name = "bob";
  node2.nd_name = "tom";
  initialize_allnodes(&allnodes, &node1, &node2);
  initialize_allnodes(&alps_reporter->alps_subnodes, &node1, &node2);

  cray_enabled = FALSE;

  pnode = find_nodebyname("george");
  fail_unless(pnode == NULL, "george found but doesn't exist");

  pnode = find_nodebyname("bob");
  fail_unless(pnode == &node1, "couldn't find bob?");

  pnode = find_nodebyname("tom");
  fail_unless(pnode == &node2, "couldn't find tom?");

  pnode = find_nodebyname(strdup("tom-0"));
  fail_unless(!strcmp(pnode->nd_name, "0"), "found an incorrect node name");

  pnode = find_nodebyname(strdup("tom-1"));
  fail_unless(!strcmp(pnode->nd_name, "1"), "found an incorrect node name");

  pnode = find_nodebyname(strdup("tom-10"));
  fail_unless(pnode == NULL, "found an incorrect node name");

  pnode = find_nodebyname(strdup("bob/0"));
  fail_unless(pnode == &node1, "couldn't find bob with the exec_host format");

  allnodes.ra->slots[1].item = NULL;
  allnodes.ra->slots[2].item = NULL;

  cray_enabled = TRUE;

  pnode = find_nodebyname("tom");
  fail_unless(pnode == &node2, "couldn't find tom?");
  
  cray_enabled = TRUE;

  pnode = find_nodebyname("bob");
  fail_unless(pnode == &node1, "couldn't find bob?");

  cray_enabled = TRUE;

  pnode = find_nodebyname("george");
  fail_unless(pnode == NULL, "george found but doesn't exist");
 
  }
END_TEST




START_TEST(test_two)
  {


  }
END_TEST




Suite *node_func_suite(void)
  {
  Suite *s = suite_create("node_func_suite methods");
  TCase *tc_core = tcase_create("find_nodebyname_test");
  tcase_add_test(tc_core, find_nodebyname_test);
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
  sr = srunner_create(node_func_suite());
  srunner_set_log(sr, "node_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
