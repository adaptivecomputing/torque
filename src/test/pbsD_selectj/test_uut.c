#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_pbsD_selectj.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

int PBSD_select_put(int c, int type, struct attropl *attrib, char *extend);
int PBSD_selectattr_put(int c, int type, struct attropl *attropl, struct attrl *attrib, char *extend);
char **PBSD_select_get(int *err, int c);


START_TEST(test_PBSD_select_get)
  {
  fail_unless(PBSD_select_get(NULL, -1) == NULL);
  fail_unless(PBSD_select_get(NULL, PBS_NET_MAX_CONNECTIONS) == NULL);
  }
END_TEST


START_TEST(test_PBSD_selectattr_put)
  {
  fail_unless(PBSD_selectattr_put(-1, 0, NULL, NULL, NULL) == PBSE_IVALREQ);
  fail_unless(PBSD_selectattr_put(PBS_NET_MAX_CONNECTIONS, 0, NULL, NULL, NULL) == PBSE_IVALREQ);
  }
END_TEST


START_TEST(test_PBSD_select_put)
  {
  fail_unless(PBSD_select_put(-1, 0, NULL, NULL) == PBSE_IVALREQ);
  fail_unless(PBSD_select_put(PBS_NET_MAX_CONNECTIONS, 0, NULL, NULL) == PBSE_IVALREQ);
  }
END_TEST


Suite *pbsD_selectj_suite(void)
  {
  Suite *s = suite_create("pbsD_selectj_suite methods");
  TCase *tc_core = tcase_create("test_PBSD_select_get");
  tcase_add_test(tc_core, test_PBSD_select_get);
  tcase_add_test(tc_core, test_PBSD_select_put);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_PBSD_selectattr_put");
  tcase_add_test(tc_core, test_PBSD_selectattr_put);
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
  sr = srunner_create(pbsD_selectj_suite());
  srunner_set_log(sr, "pbsD_selectj_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
