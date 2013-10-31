#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include "test_rm.h"
#include <stdlib.h>
#include <stdio.h>
#include "rm.h"


#include "pbs_error.h"

extern int debug_read(int sock,char **bf,long long *len);


START_TEST(test_one)
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

START_TEST(test_two)
  {


  }
END_TEST

Suite *rm_suite(void)
  {
  Suite *s = suite_create("rm_suite methods");
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
  sr = srunner_create(rm_suite());
  srunner_set_log(sr, "rm_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
