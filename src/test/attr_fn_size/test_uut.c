#include "license_pbs.h" /* See here for the software license */
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"

START_TEST(test_one)
  {
  pbs_attribute attr;

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_size(&attr,NULL,NULL,NULL,0) == 0);
  fail_unless((attr.at_flags&(ATR_VFLAG_MODIFY|ATR_VFLAG_SET)) == ATR_VFLAG_MODIFY);

  fail_unless(decode_size(&attr,NULL,NULL,"100",0) == 0);
  fail_unless(decode_size(&attr,NULL,NULL,"100k",0) == 0);
  fail_unless(decode_size(&attr,NULL,NULL,"100m",0) == 0);
  fail_unless(decode_size(&attr,NULL,NULL,"100G",0) == 0);
  fail_unless(decode_size(&attr,NULL,NULL,"100t",0) == 0);
  fail_unless(decode_size(&attr,NULL,NULL,"100P",0) == 0);
  fail_unless(decode_size(&attr,NULL,NULL,"100W",0) == 0);
  fail_unless(decode_size(&attr,NULL,NULL,"100b",0) == 0);
  fail_unless((attr.at_flags&(ATR_VFLAG_MODIFY|ATR_VFLAG_SET)) == (ATR_VFLAG_MODIFY|ATR_VFLAG_SET));

  }
END_TEST

START_TEST(test_two)
  {
  svrattrl *attrl = attrlist_create("Fred","Wilma",20);
  pbs_attribute f;
  pbs_attribute t;

  memset(&f,0,sizeof(f));
  decode_size(&f,NULL,NULL,"100G",0);
  memset(&t,0,sizeof(t));
  decode_size(&t,NULL,NULL,"2P",0);

  fail_unless(encode_size(&f,(tlist_head *)attrl,"FALSE",NULL,0,0) == 1);
  fail_unless(encode_size(&t,(tlist_head *)attrl,"TRUE",NULL,0,0) == 1);
  }
END_TEST

START_TEST(test_three)
  {
  pbs_attribute f;
  pbs_attribute t;

  memset(&f,0,sizeof(f));
  decode_size(&f,NULL,NULL,"1G",0);
  memset(&t,0,sizeof(t));
  decode_size(&t,NULL,NULL,"1024M",0);

  fail_unless(comp_size(&t,&f)==0);

  memset(&f,0,sizeof(f));
  decode_size(&f,NULL,NULL,"11G",0);
  memset(&t,0,sizeof(t));
  decode_size(&t,NULL,NULL,"10000M",0);

  fail_unless(comp_size(&t,&f)==-1);
  fail_unless(comp_size(&f,&t)==1);


  set_size(&f,&t,SET);
  fail_unless(comp_size(&t,&f)==0);

  memset(&f,0,sizeof(f));
  decode_size(&f,NULL,NULL,"1G",0);
  memset(&t,0,sizeof(t));
  decode_size(&t,NULL,NULL,"1g",0);

  set_size(&f,&t,INCR);
  fail_unless(f.at_val.at_size.atsv_num == 2);
  fail_unless(f.at_val.at_size.atsv_shift == 30);

  memset(&f,0,sizeof(f));
  decode_size(&f,NULL,NULL,"2G",0);
  memset(&t,0,sizeof(t));
  decode_size(&t,NULL,NULL,"1024M",0);

  set_size(&f,&t,DECR);
  {
    long n = f.at_val.at_size.atsv_num;
    n <<= f.at_val.at_size.atsv_shift;
    fail_unless(n == (1<<30));
  }

  }
END_TEST


Suite *attr_fn_size_suite(void)
  {
  Suite *s = suite_create("attr_fn_size_suite methods");
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
  sr = srunner_create(attr_fn_size_suite());
  srunner_set_log(sr, "attr_fn_size_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
