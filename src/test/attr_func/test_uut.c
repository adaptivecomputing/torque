#include "license_pbs.h" /* See here for the software license */
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"

extern void im_dying();

START_TEST(test_one)
  {
  char s1[] = "this, is, at \\, test, hello.,";
  char s2[] = "Let's , test \\, count,substrings.,";
  char *str = strdup(s1);
  char *ptr;
  char *ret;
  int rc;
  int ocnt;
  ptr = str;
  ret  = parse_comma_string(str,&ptr);
  fail_unless(strcmp(ret,"this") == 0);
  ret = parse_comma_string(NULL,&ptr);
  fail_unless(strcmp(ret,"is") == 0);
  ret = parse_comma_string(NULL,&ptr);
  fail_unless(strcmp(ret,"at \\, test") == 0);
  ret = parse_comma_string(NULL,&ptr);
  fail_unless(strcmp(ret,"hello.") == 0);
  ret = parse_comma_string(NULL,&ptr);
  fail_unless(ret == NULL);

  free(str);

  char *dep_str = strdup("afterok:0[].napali");
  ret = parse_comma_string(dep_str, &ptr);
  fail_unless(!strcmp(ret, "afterok:0[].napali"));
  fail_unless(parse_comma_string(NULL, &ptr) == NULL);
  fail_unless(parse_comma_string(NULL, &ptr) == NULL);

  str = strdup(s2);
  rc = count_substrings(str,&ocnt);
  fail_unless(rc == 0);
  fail_unless(ocnt == 3);
  fail_unless(strcmp(str,"Let's , test \\, count,substrings.") == 0);

  free(str);
  }
END_TEST

START_TEST(test_two)
  {
  svrattrl *attrl = attrlist_create("Fred","Wilma",20);
  svrattrl *attrl2 = attrlist_create("Barney","Betty",20);
  fail_unless(strcmp(attrl->al_name,"Fred") == 0);
  fail_unless(strcmp(attrl->al_resc,"Wilma") == 0);
  fail_unless(attrl->al_nameln == 5);
  fail_unless(attrl->al_rescln == 6);
  fail_unless(!is_link_initialized((list_link *)attrl));
  append_link((tlist_head *)attrl,(list_link *)attrl2,(void *)attrl2);
  fail_unless(is_link_initialized((list_link *)attrl));
  attrl2->al_atopl.op = UNSET;
  attrl_fixlink((tlist_head *)attrl);
  fail_unless(attrl2->al_atopl.op == SET);
  free_attrlist((tlist_head *)attrl);

  }
END_TEST

START_TEST(test_three)
  {
  pbs_attribute attr;
  attribute_def def;

  pbs_attribute attrA;
  pbs_attribute attrB;

  memset(&attr,(unsigned)~0,sizeof(pbs_attribute));
  memset(&def,0,sizeof(attribute_def));
  memset(&attrA,0,sizeof(pbs_attribute));
  memset(&attrB,0,sizeof(pbs_attribute));


  def.at_type = ATR_TYPE_LIST;

  clear_attr(&attr,&def);
  fail_unless(attr.at_type == ATR_TYPE_LIST);

  attribute_def defa[3];

  memset(defa,0,sizeof(defa));

  defa[0].at_name = "Hello There.";
  defa[1].at_name = "Howdy Pard.";
  defa[2].at_name = "hello";

  fail_unless(find_attr(defa,"HeLlo",3) == 2);
  fail_unless(find_attr(defa,"Hello there. ",3) == -1);

  attrA.at_val.at_long = 5;
  attrB.at_val.at_long = 6;

  fail_unless(attr_ifelse_long(&attrA,&attrB,3) == 3);
  attrB.at_flags = ATR_VFLAG_SET;
  fail_unless(attr_ifelse_long(&attrA,&attrB,3) == 6);
  attrA.at_flags = ATR_VFLAG_SET;
  fail_unless(attr_ifelse_long(&attrA,&attrB,3) == 5);

  free_null(&attrA);
  fail_unless(attrA.at_flags == 0);
  free_noop(&attrA);
  comp_null(&attrA,&attrB);
  }
END_TEST


Suite *attr_func_suite(void)
  {
  Suite *s = suite_create("attr_func_suite methods");
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
  sr = srunner_create(attr_func_suite());
  srunner_set_log(sr, "attr_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
