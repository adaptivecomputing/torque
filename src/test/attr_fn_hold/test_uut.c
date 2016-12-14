#include "license_pbs.h" /* See here for the software license */
#include "pbs_job.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"

START_TEST(test_one)
  {
  pbs_attribute attr;

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_hold(&attr,NULL,NULL,NULL,0) == 0);
  fail_unless((attr.at_flags&ATR_VFLAG_MODIFY) != 0);
  fail_unless(attr.at_val.at_long == 0);

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_hold(&attr,NULL,NULL,"n",0) == 0);
  fail_unless(attr.at_val.at_long == HOLD_n);
  fail_unless(attr.at_flags == (ATR_VFLAG_SET|ATR_VFLAG_MODIFY));

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_hold(&attr,NULL,NULL,"son",0) == 0);
  fail_unless(attr.at_val.at_long == HOLD_n);
  fail_unless(attr.at_flags == (ATR_VFLAG_SET|ATR_VFLAG_MODIFY));

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_hold(&attr,NULL,NULL,"o",0) == 0);
  fail_unless(attr.at_val.at_long == HOLD_o);
  fail_unless(attr.at_flags == (ATR_VFLAG_SET|ATR_VFLAG_MODIFY));

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_hold(&attr,NULL,NULL,"sou",0) == 0);
  fail_unless(attr.at_val.at_long == (HOLD_s|HOLD_o|HOLD_u));
  fail_unless(attr.at_flags == (ATR_VFLAG_SET|ATR_VFLAG_MODIFY));

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_hold(&attr,NULL,NULL,"banana",0) == PBSE_BADATVAL);

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_hold(&attr,NULL,NULL,"a",0) == 0);
  fail_unless(attr.at_flags == (ATR_VFLAG_SET|ATR_VFLAG_MODIFY));

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_hold(&attr,NULL,NULL,"l",0) == 0);
  fail_unless(attr.at_flags == (ATR_VFLAG_SET|ATR_VFLAG_MODIFY));
  }
END_TEST

START_TEST(test_two)
  {
  svrattrl *attrl = attrlist_create("Fred","Wilma",20);
  pbs_attribute f;
  pbs_attribute t;
  pbs_attribute t2;
  pbs_attribute t3;

  memset(&f, 0, sizeof(f));
  decode_hold(&f, NULL, NULL, "so", 0);
  memset(&t, 0, sizeof(t));
  decode_hold(&t, NULL, NULL, "nu", 0);
  memset(&t2, 0, sizeof(t2));
  decode_hold(&t2, NULL, NULL, "a", 0);
  memset(&t3, 0, sizeof(t3));
  decode_hold(&t3, NULL, NULL, "l", 0);

  fail_unless(encode_hold(&f, (tlist_head *)attrl, "FALSE", NULL, 0, 0) == 1);
  fail_unless(encode_hold(&t, (tlist_head *)attrl, "TRUE", NULL, 0, 0) == 1);
  fail_unless(encode_hold(&t2, (tlist_head *)attrl, "TRUE", NULL, 0, 0) == 1);
  fail_unless(encode_hold(&t3, (tlist_head *)attrl, "TRUE", NULL, 0, 0) == 1);
  }
END_TEST

START_TEST(test_three)
  {
  pbs_attribute f;
  pbs_attribute t;

  memset(&f,0,sizeof(f));
  decode_hold(&f,NULL,NULL,"son",0);
  memset(&t,0,sizeof(t));
  decode_hold(&t,NULL,NULL,"n",0);

  fail_unless(comp_hold(&t,&f)==0);

  memset(&f,0,sizeof(f));
  decode_hold(&f,NULL,NULL,"son",0);
  memset(&t,0,sizeof(t));
  decode_hold(&t,NULL,NULL,"nos",0);

  fail_unless(comp_hold(&t,&f)==1);


  }
END_TEST


Suite *attr_fn_hold_suite(void)
  {
  Suite *s = suite_create("attr_fn_hold_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_three");
  tcase_add_test(tc_core, test_three);
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
  sr = srunner_create(attr_fn_hold_suite());
  srunner_set_log(sr, "attr_fn_hold_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
