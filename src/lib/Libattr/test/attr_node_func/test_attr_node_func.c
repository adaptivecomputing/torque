#include "license_pbs.h" /* See here for the software license */
#include "attr_node_func.h"
#include "test_attr_node_func.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_one)
  {
  pbs_attribute pa;

  memset(&pa,0,sizeof(pa));
  int rc = decode_utc(&pa,"TTL",NULL,"2014-02-07T12:00:00Z",0);
  fail_unless(rc == 0);
  memset(&pa,0,sizeof(pa));
  rc = decode_utc(&pa,"TTL",NULL,"2014-02-07T12:00:00Z",0);
  fail_unless(rc == 0);
  memset(&pa,0,sizeof(pa));
  rc = decode_utc(&pa,"TTL",NULL,"2020-02-07T12:00:00+0700",0);
  fail_unless(rc == 0);
  memset(&pa,0,sizeof(pa));
  rc = decode_utc(&pa,"TTL",NULL,"2121-02-07T12:00:00-07",0);
  fail_unless(rc == 0);
  memset(&pa,0,sizeof(pa));
  rc = decode_utc(&pa,"TTL",NULL,"2121-02-07T12:00:00-0730",0);
  fail_unless(rc == 0);
  memset(&pa,0,sizeof(pa));
  rc = decode_utc(&pa,"TTL",NULL,"0",0);
  fail_unless(rc == 0);
  memset(&pa,0,sizeof(pa));
  rc = decode_utc(&pa,"TTL",NULL,"",0);
  fail_unless(rc == 0);

  memset(&pa,0,sizeof(pa));
  rc = decode_utc(&pa,"TTL",NULL,"2121-02-07T12:00:00-07:00",0);
  fail_unless(rc != 0);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *attr_node_func_suite(void)
  {
  Suite *s = suite_create("attr_node_func_suite methods");
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
  sr = srunner_create(attr_node_func_suite());
  srunner_set_log(sr, "attr_node_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
