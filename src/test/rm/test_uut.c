#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include "test_rm.h"
#include <stdlib.h>
#include <stdio.h>
#include "rm.h"


#include "pbs_error.h"

extern int debug_read(int sock,char **bf,long long *len);
extern int debug_write(int sock,char *bf,long long len);

START_TEST(test_addreq)
  {

  int err = 0;
  //struct tcp_chan *chan;

  int sock = openrm((char *)"The Host",322);
  fail_unless(begin_rm_req(sock,&err,6) == 0);
  fail_unless(addreq_err(sock,&err,(char *)"hello") == 0);
  fail_unless(addreq_err(sock,&err,(char *)"to") == 0);
  fail_unless(addreq_err(sock,&err,(char *)"all") == 0);
  fail_unless(addreq_err(sock,&err,(char *)"the") == 0);
  fail_unless(addreq_err(sock,&err,(char *)"boys") == 0);
  fail_unless(addreq_err(sock,&err,(char *)"and girls") == 0);
  flushreq();
  char *bf = NULL;
  long long len = 0;
  fail_unless(debug_read(sock,&bf,&len) == 0);

  fail_unless(strcmp(bf,"+1+2+6+2+5hello+2to+3all+3the+4boys+9and girls") == 0);
  }
END_TEST

START_TEST(test_allreq)
  {

  int err = 0;
  //struct tcp_chan *chan;

  int sock = openrm((char *)"The Host",322);
  fail_unless(begin_rm_req(sock,&err,6) == 0);
  fail_unless(allreq((char *)"hello") > 0);
  fail_unless(allreq((char *)"to") > 0);
  fail_unless(allreq((char *)"all") > 0);
  fail_unless(allreq((char *)"the") > 0);
  fail_unless(allreq((char *)"boys") > 0);
  fail_unless(allreq((char *)"and girls") > 0);
  flushreq();
  char *bf = NULL;
  long long len = 0;
  fail_unless(debug_read(sock,&bf,&len) == 0);

  fail_unless(strcmp(bf,"+1+2+6+2+5hello+2to+3all+3the+4boys+9and girls") == 0);
  }
END_TEST

START_TEST(test_closerm)
  {
  int sock = openrm((char *)"The Host",322);
  fail_unless(closerm(sock) == 0);
  }
END_TEST

START_TEST(test_downrm)
  {
  int err = 0;

  int sock = openrm((char *)"The Host",322);

  fail_unless(debug_write(sock,(char *)"3+100",5) == 5);
  fail_unless(downrm(&err,sock) == 0);
  }
END_TEST

START_TEST(test_configrm)
  {
  int sock = openrm((char *)"The Host",322);
  int err = 0;

  fail_unless(debug_write(sock,(char *)"3+100",5) == 5);
  fail_unless(configrm(sock,&err,(char *)"Some file or other") == 0);

  }
END_TEST

START_TEST(test_getreq)
  {
  int sock = openrm((char *)"The Host",322);
  int err = 0;

  fullresp(0);
  fail_unless(debug_write(sock,(char *)"2+11fred=farmer2+13sweet=babaloo",32) == 32);
  char *r = getreq_err(&err,sock);
  fail_unless(err == 0);
  fail_unless(strcmp(r,"farmer") == 0);
  r = getreq_err(&err,sock);
  fail_unless(err == 0);
  fail_unless(strcmp(r,"babaloo") == 0);

  fullresp(1);
  fail_unless(debug_write(sock,(char *)"2+11fred=farmer2+13sweet=babaloo",32) == 32);
  r = getreq_err(&err,sock);
  fail_unless(err == 0);
  fail_unless(strcmp(r,"fred=farmer") == 0);
  r = getreq_err(&err,sock);
  fail_unless(err == 0);
  fail_unless(strcmp(r,"sweet=babaloo") == 0);

  fullresp(0);
  fail_unless(debug_write(sock,(char *)"2+11fred=farmer2+13sweet=babaloo",32) == 32);
  r = getreq(sock);
  fail_unless(err == 0);
  fail_unless(strcmp(r,"farmer") == 0);
  r = getreq(sock);
  fail_unless(err == 0);
  fail_unless(strcmp(r,"babaloo") == 0);


  }
END_TEST

START_TEST(test_activereq)
  {
  int rc;

  rc = activereq();
  fail_unless(rc == -2);
  }
END_TEST

Suite *rm_suite(void)
  {
  Suite *s = suite_create("rm_suite methods");
  TCase *tc_core = tcase_create("test_addreq");
  tcase_add_test(tc_core, test_addreq);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_allreq");
  tcase_add_test(tc_core, test_allreq);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_closerm");
  tcase_add_test(tc_core, test_closerm);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_downrm");
  tcase_add_test(tc_core, test_downrm);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_configrm");
  tcase_add_test(tc_core, test_configrm);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_getreq");
  tcase_add_test(tc_core, test_getreq);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_activereq");
  tcase_add_test(tc_core, test_activereq);
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
  sr = srunner_create(rm_suite());
  srunner_set_log(sr, "rm_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
