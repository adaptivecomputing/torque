#include <check.h>

#include "alps_constants.h"

char *apbasil_protocol = "1.0";
char *release_path = "../../../test/test_scripts/release_reservation.sh";

int destroy_alps_reservation(char *, char *, char *);

START_TEST(destroy_alps_reservation_test)
  {
  char *rsv1 = "12";
  char *rsv2 = "34";
  int rc;
  
  rc = destroy_alps_reservation(rsv1, release_path, apbasil_protocol);
  fail_unless(rc == 0, "couldn't release rsv 1");
  
  rc = destroy_alps_reservation(rsv2, release_path, apbasil_protocol);
  fail_unless(rc == 0, "couldn't release rsv 2");
  }

END_TEST

START_TEST(test_two)
  {
  }
END_TEST

Suite *node_func_suite(void)
  {
  Suite *s = suite_create("alps helper suite methods");
  TCase *tc_core = tcase_create("destroy_alps_reservation_test");
  tcase_add_test(tc_core, destroy_alps_reservation_test);
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
  return(number_failed);
  }

