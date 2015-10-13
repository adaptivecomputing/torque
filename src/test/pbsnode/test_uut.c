#include <fstream>
#include <sstream>
#include "license_pbs.h" /* See here for the software license */
#include "pbs_error.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <check.h>

#include "pbs_nodes.h" /* pbs_nodes, node_check_info, node_iterator, all_nodes */
#include "attribute.h" /* svrattrl, struct  */
#include "work_task.h"


START_TEST(test_constructors)
  {
  }
END_TEST

START_TEST(test_two)
  {
  }
END_TEST

Suite *pbsnode_suite(void)
  {
  Suite *s = suite_create("pbsnode test suite methods");
  TCase *tc_core = tcase_create("test_constructors");
  tcase_add_test(tc_core, test_constructors);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("more_tests");
  tcase_add_test(tc_core, test_two);
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
  sr = srunner_create(pbsnode_suite());
  srunner_set_log(sr, "pbsnode_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
