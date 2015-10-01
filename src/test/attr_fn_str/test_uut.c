#include "license_pbs.h" /* See here for the software license */
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"
#include "csv.h"

START_TEST(test_one)
  {
  pbs_attribute attr;

  memset(&attr,0,sizeof(attr));
  fail_unless(decode_str(&attr,NULL,NULL,NULL,0) == 0);
  fail_unless((attr.at_flags&(ATR_VFLAG_MODIFY|ATR_VFLAG_SET)) == ATR_VFLAG_MODIFY);
  fail_unless(decode_str(&attr,NULL,NULL,"",0) == 0);
  fail_unless((attr.at_flags&(ATR_VFLAG_MODIFY|ATR_VFLAG_SET)) == ATR_VFLAG_MODIFY);

  fail_unless(decode_str(&attr,NULL,NULL,"a string",0) == 0);
  fail_unless((attr.at_flags&(ATR_VFLAG_MODIFY|ATR_VFLAG_SET)) == (ATR_VFLAG_MODIFY|ATR_VFLAG_SET));
  fail_unless(strcmp(attr.at_val.at_str,"a string") == 0);
  }
END_TEST

START_TEST(test_two)
  {
  svrattrl *attrl = attrlist_create("Fred","Wilma",20);
  pbs_attribute f;
  pbs_attribute t;

  memset(&f,0,sizeof(f));
  decode_str(&f,NULL,NULL,"String 1",0);
  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"String 2",0);

  fail_unless(encode_str(&f,(tlist_head *)attrl,"FALSE",NULL,0,0) == 1);
  fail_unless(encode_str(&t,(tlist_head *)attrl,"TRUE",NULL,0,0) == 1);
  }
END_TEST

START_TEST(test_three)
  {
  pbs_attribute f;
  pbs_attribute t;

  memset(&f,0,sizeof(f));
  decode_str(&f,NULL,NULL,"A String",0);
  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"A String",0);

  fail_unless(comp_str(&t,&f)==0);

  free_str(&f);
  free_str(&t);
  memset(&f,0,sizeof(f));
  decode_str(&f,NULL,NULL,"hello",0);
  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"jello",0);

  fail_unless(comp_str(&t,&f)>0);
  fail_unless(comp_str(&f,&t)<0);


  set_str(&f,&t,SET);
  fail_unless(comp_str(&t,&f)==0);

  free_str(&f);
  free_str(&t);
  memset(&f,0,sizeof(f));
  decode_str(&f,NULL,NULL,"Brad ",0);
  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"Daw",0);

  set_str(&f,&t,INCR);
  fail_unless(strcmp(f.at_val.at_str,"Brad Daw")==0);

  free_str(&f);
  free_str(&t);
  memset(&f,0,sizeof(f));
  decode_str(&f,NULL,NULL,"Bradley Daw",0);
  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"ley",0);

  set_str(&f,&t,DECR);
  fail_unless(strcmp(f.at_val.at_str,"Brad Daw")==0);

  replace_attr_string(&t,strdup("New String"));
  fail_unless(strcmp(t.at_val.at_str,"New String")==0);


  free_str(&f);
  free_str(&t);

  }
END_TEST

START_TEST(test_csv)
  {
  pbs_attribute f;
  pbs_attribute t;
  int rc = 0;
  memset(&f,0,sizeof(f));
  decode_str(&f,NULL,NULL,NULL,0);
  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"cmd2, cmd3",0);
  rc = set_str_csv(&f, &t, SET);
  fail_unless(strcmp(f.at_val.at_str,"cmd2, cmd3")==0);
  fail_unless(rc==0);
  free_str(&t);

  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"cmd3",0);
  rc = set_str_csv(&f, &t, DECR);
  fail_unless(strcmp(f.at_val.at_str,"cmd2")==0);
  fail_unless(rc == 0);
  free_str(&t);

  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"cmd4",0);

  rc = set_str_csv(&f, &t, INCR);
  fail_unless(strcmp(f.at_val.at_str,"cmd2,cmd4")==0);
  fail_unless(rc == 0);
  free_str(&t);

  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"cmd2",0);
  rc = set_str_csv(&f, &t, DECR);
  fail_unless(strcmp(f.at_val.at_str,"cmd4")==0);
  fail_unless(rc == 0);
  free_str(&t);

  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"cmd4",0);
  rc = set_str_csv(&f, &t, DECR);
  fail_unless(f.at_val.at_str == 0);
  fail_unless(rc == 0);
  free_str(&t);

  /* more complex cases*/
  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"cmd5, cmd6",0);
  rc = set_str_csv(&f, &t, SET);
  fail_unless(strcmp(f.at_val.at_str,"cmd5, cmd6")==0);
  fail_unless(rc == 0);
  free_str(&t);

  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"cmd5,,,,cmd7,cmd6,",0);
  rc = set_str_csv(&f, &t, INCR_OLD);
  fail_unless(strcmp(f.at_val.at_str,"cmd5, cmd6,cmd7")==0);
  fail_unless(rc == 0);
  free_str(&t);

  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"cmd5,,,,cmd6,  cmd6, ",0);
  rc = set_str_csv(&f, &t, DECR);
  fail_unless(strcmp(f.at_val.at_str,"cmd7")==0);
  fail_unless(rc == 0);
  free_str(&t);

  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"cmd8, cmd9 ",0);
  rc = set_str_csv(&f, &t, DECR);
  fail_unless(strcmp(f.at_val.at_str,"cmd7")==0);
  fail_unless(rc == 0);
  free_str(&t);

  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"cmd8,cmd9, cmd10",0);
  rc = set_str_csv(&f, &t, INCR_OLD);
  fail_unless(strcmp(f.at_val.at_str,"cmd7,cmd8,cmd9,cmd10")==0);
  fail_unless(rc == 0);
  free_str(&t);

  memset(&t,0,sizeof(t));
  decode_str(&t,NULL,NULL,"cmd7,cmd10,cmd8,cmd9",0);
  rc = set_str_csv(&f, &t, DECR);
  fail_unless(f.at_val.at_str == 0);
  fail_unless(rc == 0);
  free_str(&t);

  free_str(&f);
  }
END_TEST

Suite *attr_fn_str_suite(void)
  {
  Suite *s = suite_create("attr_fn_str_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_three");
  tcase_add_test(tc_core, test_three);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_CSV");
  tcase_add_test(tc_core, test_csv);
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
  sr = srunner_create(attr_fn_str_suite());
  srunner_set_log(sr, "attr_fn_str_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
