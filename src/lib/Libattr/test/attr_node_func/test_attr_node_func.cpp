#include "license_pbs.h" /* See here for the software license */
#include "attr_node_func.h"
#include "test_attr_node_func.h"
#include <stdlib.h>
#include <stdio.h>
#include "attribute.h"

#include "pbs_error.h"

START_TEST(test_one)
  {
  pbs_attribute pa;
  struct tm tm;
  time_t now = time(NULL) + 87000;
  localtime_r(&now,&tm);
  char str[30];


  memset(&pa,0,sizeof(pa));
  sprintf(str,"%04d-%02d-%02dT%02d:%02d:%02dZ",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
  int rc = decode_utc(&pa,"TTL",NULL,str,0);
  fail_unless(rc == 0);
  memset(&pa,0,sizeof(pa));
  sprintf(str,"%04d-%02d-%02dT%02d:%02d:%02d%c%02d",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,tm.tm_gmtoff < 0 ? '-':'+',abs(tm.tm_gmtoff)/3600);
  rc = decode_utc(&pa,"TTL",NULL,str,0);
  fail_unless(rc == 0);
  memset(&pa,0,sizeof(pa));
  sprintf(str,"%04d-%02d-%02dT%02d:%02d:%02d%c%02d%02d",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,tm.tm_gmtoff < 0 ? '-':'+',abs(tm.tm_gmtoff)/3600,(abs(tm.tm_gmtoff)/60)%60);
  rc = decode_utc(&pa,"TTL",NULL,str,0);
  fail_unless(rc == 0);
  memset(&pa,0,sizeof(pa));
  tm.tm_gmtoff -= 1800;
  sprintf(str,"%04d-%02d-%02dT%02d:%02d:%02d%c%02d%02d",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,tm.tm_gmtoff < 0 ? '-':'+',abs(tm.tm_gmtoff)/3600,(abs(tm.tm_gmtoff)/60)%60);
  rc = decode_utc(&pa,"TTL",NULL,str,0);
  fail_unless(rc == 0);
  now -= 90000;
  localtime_r(&now,&tm);
  memset(&pa,0,sizeof(pa));
  sprintf(str,"%04d-%02d-%02dT%02d:%02d:%02dZ",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
  rc = decode_utc(&pa,"TTL",NULL,str,0);
  fail_unless(rc == PBSE_BAD_UTC_RANGE);
  memset(&pa,0,sizeof(pa));
  sprintf(str,"%04d-%02d-%02dT%02d:%02d:%02d%c%02d",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,tm.tm_gmtoff < 0 ? '-':'+',abs(tm.tm_gmtoff)/3600);
  rc = decode_utc(&pa,"TTL",NULL,str,0);
  fail_unless(rc == PBSE_BAD_UTC_RANGE);
  memset(&pa,0,sizeof(pa));
  sprintf(str,"%04d-%02d-%02dT%02d:%02d:%02d%c%02d%02d",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,tm.tm_gmtoff < 0 ? '-':'+',abs(tm.tm_gmtoff)/3600,(abs(tm.tm_gmtoff)/60)%60);
  rc = decode_utc(&pa,"TTL",NULL,str,0);
  fail_unless(rc == PBSE_BAD_UTC_RANGE);
  }
END_TEST

START_TEST(test_two)
  {
  pbs_attribute attr;
  pbsnode nd;

  memset(&attr,0,sizeof(attr));
  memset(&nd,0,sizeof(pbsnode));
  nd.nd_state = INUSE_DOWN;

  int rc = node_state(&attr,&nd,ATR_ACTION_NEW);
  fail_unless(attr.at_val.at_short == INUSE_DOWN);
  fail_unless(rc == PBSE_NONE);

  nd.nd_state = 0;
  attr.at_val.at_short = INUSE_OFFLINE;

  rc = node_state(&attr,&nd,ATR_ACTION_ALTER);
  fail_unless(nd.nd_state == INUSE_OFFLINE);
  fail_unless(rc == PBSE_NONE);

  nd.nd_state = INUSE_NOHIERARCHY;
  attr.at_val.at_short = INUSE_OFFLINE;

  rc = node_state(&attr,&nd,ATR_ACTION_ALTER);
  fail_unless(nd.nd_state == INUSE_NOHIERARCHY);
  fail_unless(rc == PBSE_HIERARCHY_NOT_SENT);

  rc = node_state(&attr,&nd,DECR);
  fail_unless(rc == PBSE_INTERNAL);


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
