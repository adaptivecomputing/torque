#include "license_pbs.h" /* See here for the software license */
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"

const char *tVals[] =
{
    "TRUE",
    "true",
    "t",
    "T",
    "1",
    "y",
    "Y",
    ATR_TRUE
};

const char *fVals[] =
{
    "FALSE",
    "false",
    "f",
    "F",
    "0",
    "n",
    "N",
    ATR_FALSE
};


START_TEST(test_one)
  {
  unsigned int i;
  pbs_attribute attr;

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_b(&attr,NULL,NULL,NULL,0) == 0);
  fail_unless((attr.at_flags&ATR_VFLAG_MODIFY) != 0);
  fail_unless(attr.at_val.at_long == 0);

  for(i = 0;i < (sizeof(tVals)/sizeof(char *));i++)
    {
    memset(&attr,0,sizeof(attr));
    fail_unless(decode_b(&attr,NULL,NULL,tVals[i],0) == 0);
    fail_unless(attr.at_val.at_long == 1);
    fail_unless(attr.at_flags == (ATR_VFLAG_SET|ATR_VFLAG_MODIFY));
    }

  for(i = 0;i < (sizeof(fVals)/sizeof(char *));i++)
    {
    memset(&attr,0,sizeof(attr));
    fail_unless(decode_b(&attr,NULL,NULL,fVals[i],0) == 0);
    fail_unless(attr.at_val.at_long == 0);
    fail_unless(attr.at_flags == (ATR_VFLAG_SET|ATR_VFLAG_MODIFY));
    }
  }
END_TEST

START_TEST(test_two)
  {
  svrattrl *attrl = attrlist_create("Fred","Wilma",20);
  pbs_attribute f;
  pbs_attribute t;

  memset(&f,0,sizeof(f));
  decode_b(&f,NULL,NULL,"f",0);
  memset(&t,0,sizeof(t));
  decode_b(&t,NULL,NULL,"t",0);

  fail_unless(encode_b(&f,(tlist_head *)attrl,"FALSE",NULL,0,0) == 1);
  fail_unless(encode_b(&t,(tlist_head *)attrl,"TRUE",NULL,0,0) == 1);
  }
END_TEST

START_TEST(test_three)
  {
  pbs_attribute f;
  pbs_attribute t;

  memset(&f,0,sizeof(f));
  decode_b(&f,NULL,NULL,"f",0);
  memset(&t,0,sizeof(t));
  decode_b(&t,NULL,NULL,"t",0);

  fail_unless(comp_b(&t,&f)==1);

  set_b(&f,&t,SET);
  fail_unless(f.at_val.at_long == 1);
  fail_unless(comp_b(&t,&f)==0);

  memset(&f,0,sizeof(f));
  decode_b(&f,NULL,NULL,"f",0);
  memset(&t,0,sizeof(t));
  decode_b(&t,NULL,NULL,"t",0);

  set_b(&f,&t,INCR);
  fail_unless(f.at_val.at_long == 1);

  memset(&f,0,sizeof(f));
  decode_b(&f,NULL,NULL,"f",0);
  memset(&t,0,sizeof(t));
  decode_b(&t,NULL,NULL,"t",0);

  set_b(&f,&t,DECR);
  fail_unless(f.at_val.at_long == 0);

  }
END_TEST


Suite *attr_fn_b_suite(void)
  {
  Suite *s = suite_create("attr_fn_b_suite methods");
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
  sr = srunner_create(attr_fn_b_suite());
  srunner_set_log(sr, "attr_fn_b_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
