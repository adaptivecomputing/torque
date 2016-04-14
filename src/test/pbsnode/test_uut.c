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

std::string global_string = "";

START_TEST(test_constructors)
  {
  }
END_TEST

START_TEST(test_encode_properties)
  {
  struct pbsnode pn;
  tlist_head     tlh;
  std::string prop1 = "prop1";
  std::string prop2 = "prop2";

  // check NULL tlist
  fail_unless(0 != pn.encode_properties(NULL));

  // now add properties
  pn.add_property(prop1);
  pn.add_property(prop2);

  // check them
  global_string = "prop1,prop2";
  fail_unless(0 == pn.encode_properties(&tlh));
  }
END_TEST

Suite *pbsnode_suite(void)
  {
  Suite *s = suite_create("pbsnode test suite methods");
  TCase *tc_core = tcase_create("test_constructors");
  tcase_add_test(tc_core, test_constructors);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("more_tests");
  tcase_add_test(tc_core, test_encode_properties);
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
