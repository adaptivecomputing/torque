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
  fail_unless(decode_frequency(&attr,NULL,NULL,NULL,0) == 0);
  fail_unless((attr.at_flags&(ATR_VFLAG_MODIFY|ATR_VFLAG_SET)) == ATR_VFLAG_MODIFY);

  fail_unless(decode_frequency(&attr,NULL,NULL,"100",0) == 0);
  fail_unless(decode_frequency(&attr,NULL,NULL,"100mhz",0) == 0);
  fail_unless(decode_frequency(&attr,NULL,NULL,"100MHz",0) == 0);
  fail_unless(decode_frequency(&attr,NULL,NULL,"P11",0) == 0);
  fail_unless(decode_frequency(&attr,NULL,NULL,"p2",0) == 0);
  fail_unless(decode_frequency(&attr,NULL,NULL,"perFormance",0) == 0);
  fail_unless(decode_frequency(&attr,NULL,NULL,"Ondemand",0) == 0);
  // UserSpace is disallowed 
  fail_unless(decode_frequency(&attr,NULL,NULL,"UserSpace",0) != 0);
  fail_unless(decode_frequency(&attr,NULL,NULL,"fred",0) != 0);
  fail_unless((attr.at_flags&(ATR_VFLAG_MODIFY|ATR_VFLAG_SET)) == (ATR_VFLAG_MODIFY|ATR_VFLAG_SET));

  }
END_TEST

START_TEST(test_two)
  {
  svrattrl *attrl = attrlist_create("Fred","Wilma",20);
  pbs_attribute f;
  pbs_attribute t;

  memset(&f,0,sizeof(f));
  decode_frequency(&f,NULL,NULL,"100Mhz",0);
  memset(&t,0,sizeof(t));
  decode_frequency(&t,NULL,NULL,"P4",0);

  fail_unless(encode_frequency(&f,(tlist_head *)attrl,"FALSE",NULL,0,0) == 1);
  fail_unless(encode_frequency(&t,(tlist_head *)attrl,"TRUE",NULL,0,0) == 1);
  }
END_TEST

START_TEST(test_three)
  {
  pbs_attribute f;
  pbs_attribute t;

  memset(&f,0,sizeof(f));
  decode_frequency(&f,NULL,NULL,"1000",0);
  memset(&t,0,sizeof(t));
  decode_frequency(&t,NULL,NULL,"1000MHz",0);

  fail_unless(comp_frequency(&t,&f)==0);

  memset(&f,0,sizeof(f));
  decode_frequency(&f,NULL,NULL,"1100",0);
  memset(&t,0,sizeof(t));
  decode_frequency(&t,NULL,NULL,"100MHz",0);

  fail_unless(comp_frequency(&t,&f)==-1);
  fail_unless(comp_frequency(&f,&t)==1);


  set_frequency(&f,&t,SET);
  fail_unless(comp_frequency(&t,&f)==0);

  }
END_TEST


Suite *attr_fn_freq_suite(void)
  {
  Suite *s = suite_create("attr_fn_freq_suite methods");
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
  sr = srunner_create(attr_fn_freq_suite());
  srunner_set_log(sr, "attr_fn_freq_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
