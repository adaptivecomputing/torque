
#include "pmix_operation.hpp"

#include <check.h>
#include <string.h>
#include <stdio.h>

extern char mom_alias[];

extern int notified;
extern int killed_task;

job *get_initialized_job()
  {
  job *pjob = (job *)calloc(1, sizeof(job));

  strcpy(pjob->ji_qs.ji_jobid, "2.napali");
  pjob->ji_numvnod = 20;
  pjob->ji_hosts = (hnodent *)calloc(4, sizeof(hnodent));
  pjob->ji_vnods = (vnodent *)calloc(20, sizeof(vnodent));

  for (int i = 0; i < 8; i++)
    {
    pjob->ji_vnods[i].vn_host = pjob->ji_hosts;
    pjob->ji_vnods[i].vn_node = i;
    }

  for (int i = 8; i < 14; i++)
    {
    pjob->ji_vnods[i].vn_host = pjob->ji_hosts + 1;
    pjob->ji_vnods[i].vn_node = i;
    }
  
  for (int i = 14; i < 20; i++)
    {
    pjob->ji_vnods[i].vn_host = pjob->ji_hosts + 2;
    pjob->ji_vnods[i].vn_node = i;
    }

  pjob->ji_hosts[0].hn_node = 0;
  pjob->ji_hosts[1].hn_node = 1;
  pjob->ji_hosts[2].hn_node = 2;

  pjob->ji_hosts[0].hn_host = strdup("napali");
  pjob->ji_hosts[1].hn_host = strdup("waimea");
  pjob->ji_hosts[2].hn_host = strdup("wailua");
  pjob->ji_qs.ji_svrflags = JOB_SVFLG_HERE;

  return(pjob);
  }

START_TEST(test_constructors)
  {
  pmix_operation one;
  fail_unless(one.get_type() == FENCE_OPERATION);
  fail_unless(one.get_operation_id() == 0);
  fail_unless(one.get_hosts_reported().size() == 0);
  // make sure this doesn't segfault
  one.execute_callback();

  job *pjob = get_initialized_job();

  pmix_operation from_string(strdup("2:1,2,4,6,12,16,19"), pjob);
  fail_unless(from_string.has_local_rank(1) == true);
  fail_unless(from_string.has_local_rank(2) == true);
  fail_unless(from_string.has_local_rank(4) == true);
  fail_unless(from_string.has_local_rank(6) == true);
  fail_unless(from_string.has_local_rank(12) == false);
  fail_unless(from_string.has_local_rank(16) == false);
  fail_unless(from_string.has_local_rank(19) == false);

  const std::set<std::string> &reported = from_string.get_hosts_reported();
  fail_unless(reported.size() == 3);
  fail_unless(reported.find("napali") != reported.end());
  fail_unless(reported.find("waimea") != reported.end());
  fail_unless(reported.find("wailua") != reported.end());
  fail_unless(from_string.get_type() == CONNECT_OPERATION);

  pmix_proc_t procs[3];
  procs[0].rank = 2;
  procs[1].rank = 11;
  procs[2].rank = 15;
  pmix_operation conn(pjob, procs, 3, NULL, NULL);
  fail_unless(conn.get_type() == CONNECT_OPERATION);
  std::string ranks;
  conn.populate_rank_string(ranks);
  size_t pos = ranks.find(":2,11,15");
  fail_unless(pos != 0); // should be just after the id
  fail_unless(pos != std::string::npos); // should be found
  unsigned int id = strtol(ranks.c_str(), NULL, 10);
  fail_unless(id == conn.get_operation_id());
  fail_unless(conn.ranks_match(procs, 3) == true);
  fail_unless(conn.ranks_match(procs, 4) == false);
  fail_unless(conn.ranks_match(procs, 2) == false);
  sprintf(mom_alias, "napali");
  notified = 0;
  pmix_status_t rc = conn.complete_operation(pjob, 30);
  fail_unless(rc == PMIX_SUCCESS);
  fail_unless(notified == 2, "notified count %d", notified); // should have sent two remote notifications

  pmix_operation copy_conn(conn);
  fail_unless(copy_conn.ranks_match(procs, 3) == true);
  fail_unless(copy_conn.ranks_match(procs, 4) == false);
  fail_unless(copy_conn.ranks_match(procs, 2) == false);
  std::string copy_ranks;
  copy_conn.populate_rank_string(copy_ranks);
  fail_unless(copy_ranks == ranks);

  one = conn;
  std::string one_ranks;
  one.populate_rank_string(one_ranks);
  fail_unless(one_ranks == ranks);
  fail_unless(one == conn);
  fail_unless(copy_conn == conn);
  fail_unless((from_string == conn) == false);

  killed_task = 0;
  notified = 0;
  conn.set_id(1);
  one.set_id(2);
  existing_connections[1] = conn;
  existing_connections[2] = one;
  fail_unless(clean_up_connection(pjob, NULL, 1, false) == PBSE_NONE);
  fail_unless(notified == 0); // I passed false to MS, so I shouldn't notify anyone
  fail_unless(killed_task == 1, "Killed task = %d", killed_task);
  fail_unless(clean_up_connection(pjob, NULL, 1, true) == -1);

  killed_task = 0;
  check_and_act_on_obit(pjob, 2);
  fail_unless(killed_task == 1, "Killed task = %d", killed_task);
  fail_unless(notified == 2); // This should notify both sisters
  }
END_TEST


START_TEST(fence_test)
  {
  job         *pjob = get_initialized_job();
  pmix_proc_t procs[3];
  procs[0].rank = 0;
  procs[1].rank = 13;
  procs[2].rank = 17;

  pmix_operation fence(pjob, procs, 3, "", NULL, NULL);
  fail_unless(fence.get_type() == FENCE_OPERATION);
  fail_unless(fence.mark_reported("napali") == false);
  fail_unless(fence.mark_reported("waimea") == false);
  fail_unless(fence.mark_reported("bob") == false);
  fail_unless(fence.mark_reported("wailua") == true);
  }
END_TEST


Suite *pmix_operation_suite(void)
  {
  Suite *s = suite_create("pmix_operation_suite methods");
  TCase *tc_core = tcase_create("test_constructors");
  tcase_add_test(tc_core, test_constructors);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("fence_test");
  tcase_add_test(tc_core, fence_test);
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
  sr = srunner_create(pmix_operation_suite());
  srunner_set_log(sr, "pmix_operation_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }

