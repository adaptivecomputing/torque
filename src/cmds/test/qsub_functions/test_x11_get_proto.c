
#include "test_qsub_functions.h"
#include "qsub_functions.h"

START_TEST(test_x11_get_proto_1)
  {
  char *param1 = NULL;
  char *resp = NULL;
  int debug = 1;
  resp = x11_get_proto(param1, debug);
  }
END_TEST

Suite *x11_get_proto_suite(void)
  {
  Suite *s = suite_create("x11_get_proto methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_x11_get_proto_1);
  suite_add_tcase(s, tc_core);
  return s;
  }

int main(void)
  {
  int number_failed = 0;
  sr = srunner_create(x11_get_proto_suite());
  srunner_set_log(sr, "x11_get_proto_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
