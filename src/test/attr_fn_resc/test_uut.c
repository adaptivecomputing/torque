#include "license_pbs.h" /* See here for the software license */
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"

START_TEST(test_one)
  {
  pbs_attribute attr;

  memset(&attr,0,sizeof(pbs_attribute));

  fail_unless(decode_resc(NULL,NULL,NULL,NULL,0) == PBSE_INTERNAL);
  fail_unless(decode_resc(&attr,NULL,NULL,NULL,0) == PBSE_UNKRESC);
  fail_unless(decode_resc(&attr,"attrName",NULL,NULL,0) == PBSE_UNKRESC);
  fail_unless(decode_resc(&attr,"attrName","arch",NULL,0) == PBSE_ATTRRO);
  fail_unless(decode_resc(&attr,"notherAttrName","string","Pie",0) == PBSE_ATTRRO);
  fail_unless(decode_resc(&attr,"notherAttrName","string","Pie",ATR_DFLAG_ACCESS) == 0);
  fail_unless(decode_resc(&attr,"someLong","long","314179",ATR_DFLAG_ACCESS) == 0);
  fail_unless(decode_resc(&attr,"someBit","bit","1",ATR_DFLAG_ACCESS) == 0);
  fail_unless(decode_resc(&attr,"notherString","string","poop",ATR_DFLAG_ACCESS) == 0);
  free_resc(&attr);
  free_resc(&attr);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *attr_fn_resc_suite(void)
  {
  Suite *s = suite_create("attr_fn_resc_suite methods");
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
  sr = srunner_create(attr_fn_resc_suite());
  srunner_set_log(sr, "attr_fn_resc_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
