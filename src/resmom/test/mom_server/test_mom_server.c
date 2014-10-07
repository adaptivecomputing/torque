#include "license_pbs.h" /* See here for the software license */
#include "mom_server_lib.h"
#include "test_mom_server.h"
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"

#define MAXLINE 1024

char PBSNodeMsgBuf[MAXLINE];
char PBSNodeCheckPath[MAXLINE];
int  PBSNodeCheckInterval = 2;

extern bool no_error;
extern bool no_event;

START_TEST(test_check_state)
  {
  int force = 1;
  int no_force = 0;

  /* call check_state with no node health check script. i.e. PBSNodeCheckPath is NULL */
  PBSNodeCheckPath[0] = '\0';
  memset(PBSNodeMsgBuf, 0, MAXLINE);
  check_state(force);
  fail_unless(PBSNodeMsgBuf[0] == '\0', "check_state force with  no check script failed");

  PBSNodeCheckPath[0] = '\0';
  memset(PBSNodeMsgBuf, 0, MAXLINE);
  check_state(no_force);
  fail_unless(PBSNodeMsgBuf[0] == '\0', "check_state no force with  no check script failed");

  strcpy(PBSNodeCheckPath, "/var/spool/torque/mom_priv/nhc");

  /* call check_state with a node heath check script */
  /* call check_state with force */
  memset(PBSNodeMsgBuf, 0, MAXLINE);
  /* No error returned by node health script */
  check_state(force);
  fail_unless(PBSNodeMsgBuf[0] == '\0', "check_state force failed");
  
  /* ERROR returned by node health script */
  no_error = false;
  check_state(force);
  fail_unless(!strcmp(PBSNodeMsgBuf, "ERROR"), "check_state with error failed");

  /* no force */
  no_error = false;
  check_state(no_force);
  fail_unless(!strcmp(PBSNodeMsgBuf, "ERROR"), "check_state with error failed");

  /* EVENT: returned by node health check script */
  no_error = true;
  no_event = false;
  check_state(force);
  fail_unless(!strcmp(PBSNodeMsgBuf, "EVENT:"), "check_state failed for EVENT:");

  /* clear PBSNodeMsgBuf. Simulates momctl -q clearmsg */
  /* call check_state with no force. the ICount should keep the node
     health check from being called in this iteration. 
     PBSNodeMsgBuf should still be empty after call to check_state */
  memset(PBSNodeMsgBuf, 0, MAXLINE);
  check_state(no_force);
  fail_unless(PBSNodeMsgBuf[0] == '\0', "check_state failed after clearing message buffer");

  }
END_TEST


Suite *mom_server_suite(void)
  {
  Suite *s = suite_create("mom_server_suite methods");
  TCase *tc_core = tcase_create("test_check_state");
  tcase_add_test(tc_core, test_check_state);
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
  sr = srunner_create(mom_server_suite());
  srunner_set_log(sr, "mom_server_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
