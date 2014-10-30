#include "license_pbs.h" /* See here for the software license */
#include "pbsdsh.h"
#include "test_pbsdsh.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

int numnodes = 5;

int gethostnames_from_nodefile(char **, char *);

START_TEST(test_gethostnames_from_nodefile)
  {
  int count;
  char *allnodes;

  count = gethostnames_from_nodefile(&allnodes, strdup("pbs_nodefile"));
  fail_unless(count == numnodes);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *pbsdsh_suite(void)
  {
  Suite *s = suite_create("pbsdsh_suite methods");
  TCase *tc_core = tcase_create("test_gethostnames_from_nodefile");
  tcase_add_test(tc_core, test_gethostnames_from_nodefile);
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
  sr = srunner_create(pbsdsh_suite());
  srunner_set_log(sr, "pbsdsh_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
