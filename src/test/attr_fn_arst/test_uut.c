#include "license_pbs.h" /* See here for the software license */
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"

START_TEST(test_one)
  {
  pbs_attribute a;
  pbs_attribute b;
  struct array_strings *s;

  memset(&a,0,sizeof(a));
  a.at_type = ATR_TYPE_ARST;
  fail_unless(decode_arst(&a,NULL,NULL,"this=this,is=is,a=a,comma=comma,delimited=bleck,string=strung",0)==0);
  fail_unless(a.at_flags == (ATR_VFLAG_SET | ATR_VFLAG_MODIFY));
  s = a.at_val.at_arst;
  fail_unless(s->as_npointers == 6);
  fail_unless(decode_arst_merge(&a,NULL,NULL,"another=other,comma=comma,delimited=blah,string=strange")==0);
  s = a.at_val.at_arst;
  fail_unless(s->as_usedptr == 7);
  free_arst(&a);

  memset(&a,0,sizeof(a));
  memset(&b,0,sizeof(b));
  a.at_type = ATR_TYPE_ARST;
  b.at_type = ATR_TYPE_ARST;
  decode_arst(&a,NULL,NULL,"this=this,is=is,a=a,comma=comma,delimited=bleck,string=strung",0);
  decode_arst(&b,NULL,NULL,"another=other,comma=comma,delimited=blah,string=strange",0);
  fail_unless(set_arst(&a,&b,SET)==0);
  fail_unless(a.at_val.at_arst->as_usedptr == 4);
  free_arst(&a);
  free_arst(&b);

  memset(&a,0,sizeof(a));
  memset(&b,0,sizeof(b));
  a.at_type = ATR_TYPE_ARST;
  b.at_type = ATR_TYPE_ARST;
  decode_arst(&a,NULL,NULL,"this=this,is=is,a=a,comma=comma,delimited=bleck,string=strung",0);
  decode_arst(&b,NULL,NULL,"another=other,comma=comma,delimited=blah,string=strange",0);
  fail_unless(set_arst(&a,&b,INCR)==0);
  fail_unless(a.at_val.at_arst->as_usedptr == 7);
  free_arst(&a);
  free_arst(&b);

  memset(&a,0,sizeof(a));
  memset(&b,0,sizeof(b));
  a.at_type = ATR_TYPE_ARST;
  b.at_type = ATR_TYPE_ARST;
  decode_arst(&a,NULL,NULL,"this=this,is=is,a=a,comma=comma,delimited=bleck,string=strung",0);
  decode_arst(&b,NULL,NULL,"another=other,comma=comma,delimited=blah,string=strange",0);
  fail_unless(set_arst(&a,&b,DECR)==0);
  fail_unless(a.at_val.at_arst->as_usedptr == 5);
  free_arst(&a);
  free_arst(&b);

  memset(&a,0,sizeof(a));
  memset(&b,0,sizeof(b));
  a.at_type = ATR_TYPE_ARST;
  b.at_type = ATR_TYPE_ARST;
  decode_arst(&a,NULL,NULL,"this=this,is=is,a=a,comma=comma,delimited=bleck,string=strung",0);
  decode_arst(&b,NULL,NULL,"another=other,comma=comma,delimited=blah,string=strange",0);

  fail_unless(comp_arst(&a,&b)==1);
  free_arst(&a);
  free_arst(&b);

  memset(&a,0,sizeof(a));
  memset(&b,0,sizeof(b));
  a.at_type = ATR_TYPE_ARST;
  b.at_type = ATR_TYPE_ARST;
  decode_arst(&a,NULL,NULL,"this=this,is=is,a=a,comma=comma,delimited=bleck,string=strung",0);
  decode_arst(&b,NULL,NULL,"is=is,a=a,this=this,comma=comma,delimited=bleck,string=strung",0);

  fail_unless(comp_arst(&a,&b)==0);

  free_arst(&a);
  free_arst(&b);

  memset(&a,0,sizeof(a));
  a.at_type = ATR_TYPE_ARST;
  decode_arst(&a,NULL,NULL,"this=this,is=is,a=a,comma=comma,delimited=bleck,string=strung",0);

  fail_unless(arst_string("is=is",&a) != NULL);
  fail_unless(arst_string("is=isn't",&a) == NULL);


  }
END_TEST

START_TEST(test_two)
  {
  svrattrl *attrl = attrlist_create("Fred","Wilma",20);
  pbs_attribute a;

  memset(&a,0,sizeof(a));
  fail_unless(decode_arst(&a,NULL,NULL,"th\"is=this,is=is,a=a,com\\ma=comma,del\nimited=bleck,string=strung",0)==0);
  fail_unless(encode_arst(&a,(tlist_head *)attrl,"Billy","bob",ATR_ENCODE_SAVE,0)==1);



  }
END_TEST

Suite *attr_fn_arst_suite(void)
  {
  Suite *s = suite_create("attr_fn_arst_suite methods");
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
  sr = srunner_create(attr_fn_arst_suite());
  srunner_set_log(sr, "attr_fn_arst_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
