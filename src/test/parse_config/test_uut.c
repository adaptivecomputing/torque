#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <check.h>
#include <unistd.h>

#include "pbs_job.h"
#include "mom_func.h"
#include "json/json.h"

extern int encode_used_ctr;
extern int encode_flagged_attrs_ctr;
extern int MOMCudaVisibleDevices;
extern struct config *config_array;

u_long setcudavisibledevices(const char *value);
unsigned long setjobstarterprivileged(const char *);

int jobstarter_privileged = 0;
char         PBSNodeMsgBuf[MAXLINE];
int          MOMJobDirStickySet;


START_TEST(test_setcudavisibledevices)
  {
  std::stringstream output;

  setcudavisibledevices("1");
  fail_unless(MOMCudaVisibleDevices == 1, "did not set cuda_visble_devices to 1");

  setcudavisibledevices("0");
  fail_unless(MOMCudaVisibleDevices == 0, "did not set cuda_visble_devices to 0");
  }
END_TEST

START_TEST(test_setjobstarterprivileged)
  {
  fail_unless(setjobstarterprivileged("") == 1);
  fail_unless(jobstarter_privileged == 0);

  fail_unless(setjobstarterprivileged("off") == 1);
  fail_unless(jobstarter_privileged == 0);

  fail_unless(setjobstarterprivileged("0") == 1);
  fail_unless(jobstarter_privileged == 0);

  fail_unless(setjobstarterprivileged("false") == 1);
  fail_unless(jobstarter_privileged == 0);

  fail_unless(setjobstarterprivileged("on") == 1);
  fail_unless(jobstarter_privileged == TRUE);

  fail_unless(setjobstarterprivileged("t") == 1);
  fail_unless(jobstarter_privileged == TRUE);

  fail_unless(setjobstarterprivileged("T") == 1);
  fail_unless(jobstarter_privileged == TRUE);

  fail_unless(setjobstarterprivileged("1") == 1);
  fail_unless(jobstarter_privileged == TRUE);
  }
END_TEST


START_TEST(test_reqgres)
  {

  // allocate array
  fail_unless((config_array = (struct config *)calloc(2, sizeof(struct config))) != NULL);

  // basic test
  fail_unless(strcmp(reqgres(NULL), "") == 0);

  // fill in array
  config_array[0].c_name = strdup("somegres");
  config_array[0].c_u.c_value = strdup("somestuff");
  fail_unless(strcmp(reqgres(NULL), "somegres:somestuff") == 0);

  config_array[0].c_u.c_value = strdup("!fakeshellescape");
  fail_unless(strcmp(reqgres(NULL), "somegres:scriptoutput") == 0);
  }
END_TEST


START_TEST(test_addclient)
  {
  char hostname[1024];
  u_long addr;
  u_long new_addr;
  struct addrinfo *addr_info;
  struct addrinfo hints;
  struct in_addr saddr;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
 
  // get IP addr of hostname from addclient
  fail_unless(gethostname(hostname, sizeof(hostname)) == 0);
  addr = addclient(hostname); 

  // now look up IPv4 one and confirm same as one from addclient
  fail_unless(getaddrinfo(hostname, NULL, &hints, &addr_info) == 0);
  saddr = ((struct sockaddr_in *)addr_info->ai_addr)->sin_addr;
  new_addr = ntohl(saddr.s_addr);
  fail_unless(addr == new_addr);
  }
END_TEST

Suite *parse_config_suite(void)
  {
  Suite *s = suite_create("parse_config test suite methods");
  TCase *tc_core = tcase_create("test_setcudavisibledevices");
  tcase_add_test(tc_core, test_setcudavisibledevices);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_setjobstarterprivileged");
  tcase_add_test(tc_core, test_setjobstarterprivileged);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_reqgres");
  tcase_add_test(tc_core, test_reqgres);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_addclient");
  tcase_add_test(tc_core, test_addclient);
  suite_add_tcase(s, tc_core);

  return(s);
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(parse_config_suite());
  srunner_set_log(sr, "parse_config_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
