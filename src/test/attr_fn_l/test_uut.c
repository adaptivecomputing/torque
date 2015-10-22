#include "license_pbs.h" /* See here for the software license */
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"

START_TEST(test_one)
  {
  pbs_attribute attr;
  pbs_attribute otherattr;

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_l(&attr,NULL,NULL,"134569",0) == 0);
  fail_unless((attr.at_flags&ATR_VFLAG_MODIFY) != 0);
  fail_unless(attr.at_val.at_long == 134569);

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_l(&attr,NULL,NULL,NULL,0) == 0);
  fail_unless((attr.at_flags&ATR_VFLAG_MODIFY) != 0);
  fail_unless(attr.at_val.at_long == 0);

  memset(&attr,0,sizeof(attr));
  memset(&otherattr,0,sizeof(attr));
  decode_l(&attr,NULL,NULL,"-356778",0);
  decode_l(&otherattr,NULL,NULL,"23456789",0);
  fail_unless(set_l(&attr,&otherattr,SET) == 0);
  fail_unless(attr.at_val.at_long == 23456789);

  memset(&attr,0,sizeof(attr));
  memset(&otherattr,0,sizeof(attr));
  decode_l(&attr,NULL,NULL,"-35678",0);
  decode_l(&otherattr,NULL,NULL,"35678",0);
  fail_unless(set_l(&attr,&otherattr,INCR) == 0);
  fail_unless(attr.at_val.at_long == 0);

  memset(&attr,0,sizeof(attr));
  memset(&otherattr,0,sizeof(attr));
  decode_l(&attr,NULL,NULL,"-35678",0);
  decode_l(&otherattr,NULL,NULL,"-35678",0);
  fail_unless(set_l(&attr,&otherattr,DECR) == 0);
  fail_unless(attr.at_val.at_long == 0);

  memset(&attr,0,sizeof(attr));
  memset(&otherattr,0,sizeof(attr));
  decode_l(&attr,NULL,NULL,"5",0);
  decode_l(&otherattr,NULL,NULL,"6",0);
  fail_unless(comp_l(&attr,&otherattr) == -1);
  fail_unless(comp_l(&otherattr,&attr) == 1);

  memset(&attr,0,sizeof(attr));
  memset(&otherattr,0,sizeof(attr));
  decode_l(&attr,NULL,NULL,"123468",0);
  decode_l(&otherattr,NULL,NULL,"123468",0);
  fail_unless(comp_l(&attr,&otherattr) == 0);
  }
END_TEST

START_TEST(test_two)
  {
  svrattrl *attrl = attrlist_create("Fred","Wilma",20);
  pbs_attribute f;
  pbs_attribute t;

  memset(&f,0,sizeof(f));
  decode_l(&f,NULL,NULL,"567890",0);
  memset(&t,0,sizeof(t));
  decode_l(&t,NULL,NULL,"1235689",0);

  fail_unless(encode_l(&f,(tlist_head *)attrl,"FALSE",NULL,0,0) == 1);
  fail_unless(encode_l(&t,(tlist_head *)attrl,"TRUE",NULL,0,0) == 1);
  }
END_TEST

Suite *attr_fn_l_suite(void)
  {
  Suite *s = suite_create("attr_fn_l_suite methods");
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
  sr = srunner_create(attr_fn_l_suite());
  srunner_set_log(sr, "attr_fn_l_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
