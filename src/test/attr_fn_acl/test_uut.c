#include "license_pbs.h" /* See here for the software license */
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"

extern char *server_host;


START_TEST(test_one)
  {
  pbs_attribute attr;
  pbs_attribute newAttr;

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"brad@braddaw.com,lauradaw7@gmail.com,fred@farmer.org",0);

  memset(&newAttr,0,sizeof(newAttr));
  decode_arst(&newAttr,NULL,NULL,"brad@gmail.com,lauradaw7@bestgirl.com,fred@farming.org",0);

  fail_unless(set_uacl(&attr,&newAttr,SET) == 0);
  fail_unless(attr.at_val.at_arst->as_usedptr == 3);

  free_arst(&attr);
  free_arst(&newAttr);

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"brad@braddaw.com,lauradaw7@gmail.com,fred@farmer.org",0);

  memset(&newAttr,0,sizeof(newAttr));
  decode_arst(&newAttr,NULL,NULL,"brad@gmail.com,lauradaw7@bestgirl.com,fred@farmer.org",0);

  fail_unless(set_uacl(&attr,&newAttr,INCR) == PBSE_DUPLIST);

  free_arst(&attr);
  free_arst(&newAttr);

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"brad@braddaw.com,lauradaw7@gmail.com,fred@farmer.org",0);

  memset(&newAttr,0,sizeof(newAttr));
  decode_arst(&newAttr,NULL,NULL,"brad@gmail.com,lauradaw7@bestgirl.com,fred@farming.org",0);

  fail_unless(set_uacl(&attr,&newAttr,INCR) == 0);
  fail_unless(attr.at_val.at_arst->as_usedptr == 6);

  free_arst(&attr);
  free_arst(&newAttr);

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"brad@braddaw.com,lauradaw7@gmail.com,fred@farmer.org",0);

  memset(&newAttr,0,sizeof(newAttr));
  decode_arst(&newAttr,NULL,NULL,"brad@gmail.com,lauradaw7@bestgirl.com,fred@farmer.org",0);

  fail_unless(set_uacl(&attr,&newAttr,DECR) == 0);
  fail_unless(attr.at_val.at_arst->as_usedptr == 2);

  free_arst(&attr);
  free_arst(&newAttr);

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"brad@braddaw.com,lauradaw7@gmail.com,fred@farmer.org",0);

  memset(&newAttr,0,sizeof(newAttr));
  decode_arst(&newAttr,NULL,NULL,"brad@gmail.com,lauradaw7@bestgirl.com,fred@farming.org",0);

  fail_unless(set_hostacl(&attr,&newAttr,SET) == 0);
  fail_unless(attr.at_val.at_arst->as_usedptr == 3);

  free_arst(&attr);
  free_arst(&newAttr);

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"brad@braddaw.com,lauradaw7@gmail.com,fred@farmer.org",0);

  memset(&newAttr,0,sizeof(newAttr));
  decode_arst(&newAttr,NULL,NULL,"brad@gmail.com,lauradaw7@bestgirl.com,fred@farmer.org",0);

  fail_unless(set_hostacl(&attr,&newAttr,INCR) == PBSE_DUPLIST);

  free_arst(&attr);
  free_arst(&newAttr);

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"brad@braddaw.com,lauradaw7@gmail.com,fred@farmer.org",0);

  memset(&newAttr,0,sizeof(newAttr));
  decode_arst(&newAttr,NULL,NULL,"brad@gmail.com,lauradaw7@bestgirl.com,fred@farming.org",0);

  fail_unless(set_hostacl(&attr,&newAttr,INCR) == 0);
  fail_unless(attr.at_val.at_arst->as_usedptr == 6);

  free_arst(&attr);
  free_arst(&newAttr);

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"brad@braddaw.com,lauradaw7@gmail.com,fred@farmer.org",0);

  memset(&newAttr,0,sizeof(newAttr));
  decode_arst(&newAttr,NULL,NULL,"brad@gmail.com,lauradaw7@bestgirl.com,fred@farmer.org",0);

  fail_unless(set_hostacl(&attr,&newAttr,DECR) == 0);
  fail_unless(attr.at_val.at_arst->as_usedptr == 2);

  free_arst(&attr);
  free_arst(&newAttr);
  }
END_TEST

START_TEST(test_two)
  {
  pbs_attribute attr;
  server_host = (char *)"bdaw.ac";

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"brad@braddaw.com,lauradaw7@gmail.com,fred@farmer.org",0);
  fail_unless(acl_check(&attr,(char *)"fred@farmer.org",ACL_User)==1);
  free_arst(&attr);

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"brad@braddaw.com,lauradaw7@gmail.com,-fred@farmer.org",0);
  fail_unless(acl_check(&attr,(char *)"fred@farmer.org",ACL_User)==0);
  free_arst(&attr);

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"braddaw.com,gmail.com,farmer.org,host[0-7]",0);
  fail_unless(acl_check(&attr,(char *)"host0",ACL_Host)==1);
  fail_unless(acl_check(&attr,(char *)"host8",ACL_Host)==0);
  free_arst(&attr);

  memset(&attr,0,sizeof(attr));
  decode_arst(&attr,NULL,NULL,"braddaw.com,gmail.com,farmer.org,host*",0);
  fail_unless(acl_check(&attr,(char *)"host0",ACL_Host)==1);
  fail_unless(acl_check(&attr,(char *)"heist8",ACL_Host)==0);
  free_arst(&attr);



  }
END_TEST

Suite *attr_fn_acl_suite(void)
  {
  Suite *s = suite_create("attr_fn_acl_suite methods");
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
  sr = srunner_create(attr_fn_acl_suite());
  srunner_set_log(sr, "attr_fn_acl_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
