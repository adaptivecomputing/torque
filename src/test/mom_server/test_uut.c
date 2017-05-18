#include "license_pbs.h" /* See here for the software license */
#include "mom_server_lib.h"
#include "test_mom_server.h"
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"
#include "mom_server.h"
#include "resmon.h"

#define MAXLINE 1024
#define NO_SERVER_CONFIGURED -1

extern mom_hierarchy_t *mh;

extern void sort_paths();

int mom_server_update_stat(mom_server *pms, std::vector<std::string> &strings);

char PBSNodeMsgBuf[MAXLINE];
char PBSNodeCheckPath[MAXLINE];
int  PBSNodeCheckInterval = 2;

extern bool no_error;
extern bool no_event;
extern int    ServerStatUpdateInterval;
extern time_t time_now;
extern bool   ForceServerUpdate;
extern time_t first_update_time;
extern int    UpdateFailCount;
extern time_t LastServerUpdateTime;
extern int    is_reporter_mom;
extern mom_server mom_servers[PBS_MAXSERVER];

bool is_for_this_host(std::string gpu_spec, const char *suffix);
void get_device_indices(const char *gpu_str, std::vector<int> &gpu_indices, const char *suffix);

START_TEST(test_sort_paths)
  {
  char before[500];
  char after[500];
  mh = initialize_mom_hierarchy();
  for(int paths = 0;paths < 5;paths++)
    {
    mom_levels lvl;
    for(int levels = 0;levels < (15 - paths);levels++)
      {
      mom_nodes nd;
      lvl.push_back(nd);
      }
    mh->paths.push_back(lvl);
    }
  before[0] = '\0';
  for(size_t i = 0;i < mh->paths.size();i++)
    {
    char num[10];
    sprintf(num,"%d ",(int)mh->paths.at(i).size());
    strcat(before,num);
    }

  sort_paths();

  after[0] = '\0';
  for(size_t i = 0;i < mh->paths.size();i++)
    {
    char num[10];
    sprintf(num,"%d ",(int)mh->paths.at(i).size());
    strcat(after,num);
    }
  fail_unless(strcmp(before,"15 14 13 12 11 ") == 0);
  fail_unless(strcmp(after,"11 12 13 14 15 ") == 0);
  }
END_TEST

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


START_TEST(test_mom_server_update_stat_clear_force)
  {
  ServerStatUpdateInterval = 45;
  std::vector<std::string> status(4, "Think of a status line");
  mom_server pms;
  strncpy(pms.pbs_servername, "test", PBS_MAXSERVERNAME);

  /* Force send status update */
  time_now = time(NULL);
  pms.MOMLastSendToServerTime = time_now - 20;
  ForceServerUpdate = true;

  fail_unless(mom_server_update_stat(&pms, status) == PBSE_NONE);
  fail_unless(ForceServerUpdate == false);
  fail_unless(pms.MOMLastSendToServerTime == time_now);

  /* Usual send status update */
  time_now = time(NULL);
  pms.MOMLastSendToServerTime = time_now - 100;
  ForceServerUpdate = false;

  fail_unless(mom_server_update_stat(&pms, status) == PBSE_NONE);
  fail_unless(ForceServerUpdate == false);
  fail_unless(pms.MOMLastSendToServerTime == time_now);

  /* It's too early to update even if force enabled */
  time_now = time(NULL);
  pms.MOMLastSendToServerTime = time_now - 5;
  time_t check_val = pms.MOMLastSendToServerTime;
  ForceServerUpdate = true;

  fail_unless(mom_server_update_stat(&pms, status) == NO_SERVER_CONFIGURED);
  fail_unless(ForceServerUpdate == true);
  fail_unless(pms.MOMLastSendToServerTime == check_val);
  }
END_TEST


START_TEST(test_send_update_force_flag)
  {
  first_update_time = 0;
  UpdateFailCount = 0;
  ServerStatUpdateInterval = 45;
  LastServerUpdateTime = 100;

  ForceServerUpdate = false;
  time_now = 100;
  fail_unless(!send_update());
  time_now = 144;
  fail_unless(!send_update());
  time_now = 145;
  fail_unless(send_update());

  ForceServerUpdate = true;
  time_now = 100;
  fail_unless(!send_update());
  time_now = 114;
  fail_unless(!send_update());
  time_now = 115;
  fail_unless(send_update());
  }
END_TEST


START_TEST(test_mom_server_all_update_stat_clear_force)
  {
  ServerStatUpdateInterval = 45;
  strncpy(mom_servers[0].pbs_servername, "test", PBS_MAXSERVERNAME);

  is_reporter_mom = true;

  LastServerUpdateTime = time(NULL) - 100;
  ForceServerUpdate = true;
  mom_server_all_update_stat();
  fail_unless(ForceServerUpdate == false);

  LastServerUpdateTime = time(NULL) - 100;
  ForceServerUpdate = false;
  mom_server_all_update_stat();
  fail_unless(ForceServerUpdate == false);

  is_reporter_mom = false;

  LastServerUpdateTime = time(NULL) - 100;
  ForceServerUpdate = true;
  mom_server_all_update_stat();
  fail_unless(ForceServerUpdate == false);

  LastServerUpdateTime = time(NULL) - 100;
  ForceServerUpdate = true;
  mom_server_all_update_stat();
  fail_unless(ForceServerUpdate == false);
  }
END_TEST

START_TEST(test_is_for_this_host)
  {
  std::string spec;
  std::string suffix;
  extern char mom_alias[];

  sprintf(mom_alias, "fattony3.ac");

  suffix = "-gpu";
  /* test the positive case */
  spec =  "fattony3.ac-gpu/0";
  fail_unless(is_for_this_host(spec, suffix.c_str()) == true);

  /* test the negative case */
  spec =  "numa3.ac-gpu/0";
  fail_unless(is_for_this_host(spec, suffix.c_str()) == false);

  /* test the negative case bad input*/
  spec =  "fattony3.ac/0";
  fail_unless(is_for_this_host(spec, suffix.c_str()) == false);

  }
END_TEST


START_TEST(test_get_device_indices)
  {
  std::string spec;
  std::vector<int> gpu_indices;
  char suffix[10];
  extern char mom_alias[];

  sprintf(mom_alias, "fattony3.ac");

  strcpy(suffix, "-gpu");
  spec = "fattony3.ac-gpu/0+fattony3.ac-gpu/1+numa3.ac-gpu/0";
  get_device_indices(spec.c_str(), gpu_indices, suffix);
  fail_unless(gpu_indices.size() == 2);
  fail_unless(gpu_indices[0] == 0);
  fail_unless(gpu_indices[1] == 1);

  }
END_TEST


Suite *mom_server_suite(void)
  {
  Suite *s = suite_create("mom_server_suite methods");
  TCase *tc_core = tcase_create("test_sort_paths");
  tcase_add_test(tc_core, test_sort_paths);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_check_state");
  tcase_add_test(tc_core, test_check_state);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_mom_server_update_stat_clear_force");
  tcase_add_test(tc_core, test_mom_server_update_stat_clear_force);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_send_update_force_flag");
  tcase_add_test(tc_core, test_send_update_force_flag);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_is_for_this_host");
  tcase_add_test(tc_core, test_is_for_this_host);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_get_device_indices");
  tcase_add_test(tc_core, test_get_device_indices);
  tcase_add_test(tc_core, test_mom_server_all_update_stat_clear_force);
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
