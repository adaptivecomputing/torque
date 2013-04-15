#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_one)
  {
  AvlTree tree = NULL;
  pbsnode node[21];
  pbsnode *pNode;
  char *bf = NULL;
  long len = 0;
  long maxLen = 0;

  memset(node,0,sizeof(node));

  tree = AVL_insert(500,500,node,NULL);

  tree = AVL_insert(499,499,node + 1,tree);
  tree = AVL_insert(498,498,node + 2,tree);
  tree = AVL_insert(497,497,node + 3,tree);
  tree = AVL_insert(496,496,node + 4,tree);
  tree = AVL_insert(500,499,node + 5,tree);
  tree = AVL_insert(500,498,node + 6,tree);
  tree = AVL_insert(500,497,node + 7,tree);
  tree = AVL_insert(500,496,node + 8,tree);
  tree = AVL_insert(500,495,node + 9,tree);

  tree = AVL_insert(501,501,node + 10,tree);
  tree = AVL_insert(502,502,node + 11,tree);
  tree = AVL_insert(503,503,node + 12,tree);
  tree = AVL_insert(504,504,node + 13,tree);
  tree = AVL_insert(505,505,node + 14,tree);
  tree = AVL_insert(506,506,node + 15,tree);

  tree = AVL_insert(500,501,node + 16,tree);
  tree = AVL_insert(500,502,node + 17,tree);
  tree = AVL_insert(500,503,node + 18,tree);
  tree = AVL_insert(500,504,node + 19,tree);
  tree = AVL_insert(500,505,node + 20,tree);

  pNode = AVL_find(503,503,tree);
  fail_unless(pNode == (node + 12));
  fail_unless(AVL_is_in_tree(500,505,tree) == 1);
  fail_unless(AVL_is_in_tree(510,505,tree) == 0);
  fail_unless(AVL_is_in_tree(500,515,tree) == 0);

  fail_unless(AVL_is_in_tree_no_port_compare(500,501,tree) == 1);
  fail_unless(AVL_is_in_tree_no_port_compare(500,521,tree) == 1);
  fail_unless(AVL_is_in_tree_no_port_compare(510,501,tree) == 0);

  fail_unless(AVL_get_port_by_ipaddr(506,tree) == 506);
  fail_unless(AVL_get_port_by_ipaddr(526,tree) == 0);

  fail_unless(AVL_is_in_tree(500,505,tree) == 1);
  tree = AVL_delete_node(500,505,tree);
  fail_unless(AVL_is_in_tree(500,505,tree) == 0);
  tree = AVL_delete_node(497,497,tree);
  tree = AVL_delete_node(497,495,tree);
  tree = AVL_delete_node(500,506,tree);
  tree = AVL_delete_node(506,506,tree);

  fail_unless(AVL_list(tree,&bf,&len,&maxLen) == PBSE_NONE);

  fail_unless(bf != NULL);
  fail_unless(len > 0);
  //fprintf(stderr,"%s\n",bf);




  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *u_tree_suite(void)
  {
  Suite *s = suite_create("u_tree_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
  sr = srunner_create(u_tree_suite());
  srunner_set_log(sr, "u_tree_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
