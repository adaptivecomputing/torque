#include "pbs_config.h"
#include <string>
#include <sstream>
#include <vector>
#include "license_pbs.h" /* See here for the software license */
#include "node_manager.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include "pbs_error.h"
#include "server.h" /* server */

const char *exec_hosts = "napali/0+napali/1+napali/2+napali/50+napali/4+l11/0+l11/1+l11/2+l11/3";
char  buf[4096];
const char *napali = "napali";
const char *l11 =    "l11";
struct server server;

int   kill_job_on_mom(const char *job_id, struct pbsnode *pnode);
int   remove_job_from_node(struct pbsnode *pnode, int internal_job_id);
bool  node_in_exechostlist(const char *, char *, const char *);
char *get_next_exec_host(char **);
int   job_should_be_killed(std::string &, int, struct pbsnode *);
int   check_for_node_type(complete_spec_data &, enum node_types);
int   record_external_node(job *, struct pbsnode *);
int save_node_for_adding(std::list<node_job_add_info> *naji_list, struct pbsnode *pnode, single_spec_data &req, int first_node_id, int is_external_node, int req_rank);
void remove_job_from_already_killed_list(struct work_task *pwt);
bool job_already_being_killed(int internal_job_id);
void process_job_attribute_information(std::string &job_id, std::string &attributes);
bool process_as_node_list(const char *spec, std::list<node_job_add_info> *naji_list);
bool node_is_spec_acceptable(struct pbsnode *pnode, single_spec_data &spec, char *ProcBMStr, int *eligible_nodes, bool job_is_exclusive);
void populate_range_string_from_slot_tracker(const execution_slot_tracker &est, std::string &range_str);
int  translate_job_reservation_info_to_string(std::vector<job_reservation_info> &host_info, int *NCount, std::string &exec_host_output, std::stringstream *exec_port_output);
int place_subnodes_in_hostlist(job *pjob, struct pbsnode *pnode, node_job_add_info &naji, job_reservation_info &jri, char *ProcBMStr);
void record_fitting_node(int &num, struct pbsnode *pnode, std::list<node_job_add_info> *naji_list, single_spec_data &req, int first_node_id, int i, int num_alps_reqs, enum job_types jt, complete_spec_data &all_reqs, alps_req_data **ard_array);
int add_multi_reqs_to_job(job *pjob, int num_reqs, alps_req_data *ard_array);
int add_job_to_mic(struct pbsnode *pnode, int index, job *pjob);
int remove_job_from_nodes_mics(struct pbsnode *pnode, job *pjob);
void update_failure_counts(const char *node_name, int rc);
void check_node_jobs_existence(struct work_task *pwt);



extern std::vector<int> jobsKilled;

extern pbsnode napali_node;
extern int str_to_attr_count;
extern int decode_resc_count;
extern bool conn_success;
extern bool alloc_br_success;
extern bool cray_enabled;


START_TEST(check_node_jobs_exitence_test)
  {
  napali_node.change_name(napali);
  std::vector<job_usage_info> usages;
  
  for (int i = 5; i < 15; i++)
    {
    job_usage_info jui(i);
    usages.push_back(jui);
    }

  napali_node.nd_job_usages = usages;
  fail_unless(napali_node.nd_job_usages.size() == 10);

  work_task *pwt = (work_task *)calloc(1, sizeof(work_task));
  pwt->wt_parm1 = strdup(napali_node.get_name());
  pwt->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));

  check_node_jobs_existence(pwt);

  // Should have removed all usages 10 and higher -- see scaffolding.c
  fail_unless(napali_node.nd_job_usages.size() == 5);
  for (size_t i = 0; i < napali_node.nd_job_usages.size(); i++)
    fail_unless(napali_node.nd_job_usages[i].internal_job_id < 10);

  }
END_TEST

#ifdef PENABLE_LINUX_CGROUPS
void save_cpus_and_memory_cpusets(job *pjob, const char *host, std::string &cpus, std::string &mems);
START_TEST(test_save_cpus_and_memory_cpusets)
  {
  job         *pjob = new job();
  std::string  cpus("0-3");
  std::string  mems("0");

  save_cpus_and_memory_cpusets(pjob, "napali", cpus, mems);
  fail_unless(!strcmp(pjob->ji_wattr[JOB_ATR_cpuset_string].at_val.at_str, "napali:0-3"));
  fail_unless(!strcmp(pjob->ji_wattr[JOB_ATR_memset_string].at_val.at_str, "napali:0"));
  
  save_cpus_and_memory_cpusets(pjob, "wailua", cpus, mems);
  fail_unless(!strcmp(pjob->ji_wattr[JOB_ATR_cpuset_string].at_val.at_str, "napali:0-3+wailua:0-3"));
  fail_unless(!strcmp(pjob->ji_wattr[JOB_ATR_memset_string].at_val.at_str, "napali:0+wailua:0"));
  
  save_cpus_and_memory_cpusets(pjob, "waimea", cpus, mems);
  fail_unless(!strcmp(pjob->ji_wattr[JOB_ATR_cpuset_string].at_val.at_str, "napali:0-3+wailua:0-3+waimea:0-3"));
  fail_unless(!strcmp(pjob->ji_wattr[JOB_ATR_memset_string].at_val.at_str, "napali:0+wailua:0+waimea:0"));

  }
END_TEST
#endif


START_TEST(test_update_failure_counts)
  {
  const char *name = "lihue";
  struct pbsnode *pnode = find_nodebyname(name);
  update_failure_counts(name, -1);
  update_failure_counts(name, -1);

  // Make sure the two failures are correctly counted
  fail_unless(pnode->nd_proximal_failures == 2);
  fail_unless(pnode->nd_consecutive_successes == 0);
  fail_unless(pnode->nd_state == INUSE_FREE);

  // One success shouldn't reset the failure counts
  update_failure_counts(name, 0);
  fail_unless(pnode->nd_proximal_failures == 2);
  fail_unless(pnode->nd_consecutive_successes == 1);
  fail_unless(pnode->nd_state == INUSE_FREE);
  
  // Two should
  update_failure_counts(name, 0);
  fail_unless(pnode->nd_proximal_failures == 0);
  fail_unless(pnode->nd_consecutive_successes == 2);
  fail_unless(pnode->nd_state == INUSE_FREE);

  // One failure should reset the success count
  update_failure_counts(name, 1);
  fail_unless(pnode->nd_proximal_failures == 1);
  fail_unless(pnode->nd_consecutive_successes == 0);
  fail_unless(pnode->nd_state == INUSE_FREE);

  // State shouldn't change until there are 3 proximal failures
  update_failure_counts(name, 1);
  fail_unless(pnode->nd_proximal_failures == 2);
  fail_unless(pnode->nd_consecutive_successes == 0);
  fail_unless(pnode->nd_state == INUSE_FREE);
  
  update_failure_counts(name, 1);
  fail_unless(pnode->nd_state != INUSE_FREE);
  fail_unless(pnode->nd_proximal_failures == 3);

  // State shouldn't reset until there are 2 consecutive successes
  update_failure_counts(name, 0);
  fail_unless(pnode->nd_state != INUSE_FREE);
  fail_unless(pnode->nd_proximal_failures == 3);
  fail_unless(pnode->nd_consecutive_successes == 1);
  
  update_failure_counts(name, 0);
  fail_unless(pnode->nd_state == INUSE_FREE);
  fail_unless(pnode->nd_proximal_failures == 0);
  fail_unless(pnode->nd_consecutive_successes == 2);
  }
END_TEST


START_TEST(test_add_remove_mic_jobs)
  {
  struct pbsnode      pnode;
  job                *pjobs = new job();
  
  pnode.nd_nmics = 5;
  pnode.nd_nmics_free = 5;
  pnode.nd_nmics_to_be_used = 3;

  for (short i = 0; i < pnode.nd_nmics; i++)
    pnode.nd_micjobids.push_back(-1);

  pjobs[0].ji_internal_id = 0;
  pjobs[1].ji_internal_id = 1;
  pjobs[2].ji_internal_id = 2;

  fail_unless(add_job_to_mic(&pnode, 0, pjobs + 0) == PBSE_NONE);
  fail_unless(pnode.nd_nmics_free == 4);
  fail_unless(pnode.nd_nmics_to_be_used == 2);
  
  fail_unless(add_job_to_mic(&pnode, 1, pjobs + 1) == PBSE_NONE);
  fail_unless(pnode.nd_nmics_free == 3);
  fail_unless(pnode.nd_nmics_to_be_used == 1);
  
  fail_unless(add_job_to_mic(&pnode, 2, pjobs + 2) == PBSE_NONE);
  fail_unless(pnode.nd_nmics_free == 2);
  fail_unless(pnode.nd_nmics_to_be_used == 0);
 
  // make sure an add to the same mic fails
  fail_unless(add_job_to_mic(&pnode, 2, pjobs + 2) != PBSE_NONE);
  fail_unless(pnode.nd_nmics_free == 2);
  fail_unless(pnode.nd_nmics_to_be_used == 0);

  remove_job_from_nodes_mics(&pnode, pjobs + 0);
  fail_unless(pnode.nd_nmics_free == 3);

  // make sure a repeat doesn't change things
  remove_job_from_nodes_mics(&pnode, pjobs + 0);
  fail_unless(pnode.nd_nmics_free == 3);

  remove_job_from_nodes_mics(&pnode, pjobs + 1);
  fail_unless(pnode.nd_nmics_free == 4);

  remove_job_from_nodes_mics(&pnode, pjobs + 2);
  fail_unless(pnode.nd_nmics_free == 5);
  }
END_TEST


START_TEST(test_kill_job_on_mom)
  {
  struct pbsnode  pnode;
  char            job_id[20];
  int             rc;
  unsigned long   addr = 4567;

  strcpy(job_id, "33.torque-devtest-03");

  pnode.change_name("numa3.ac");
  pnode.nd_mom_port = 1234;
  pnode.nd_addrs.push_back(addr);

  rc = kill_job_on_mom(job_id, &pnode);
  fail_unless(rc == PBSE_NONE); 

  alloc_br_success = false;
  rc = kill_job_on_mom(job_id, &pnode);
  fail_unless(rc == -1); 

  alloc_br_success = true;;
  conn_success = false;
  rc = kill_job_on_mom(job_id, &pnode);
  fail_unless(rc == -1); 

  }
END_TEST

START_TEST(test_initialize_alps_req_data)
  {
  alps_req_data      *ard;
  std::list<node_job_add_info> naji_list;
  single_spec_data    req;
  complete_spec_data  csd;
  int                 num = 0;
  struct pbsnode      pnode;

  pnode.nd_id = 0;
  pnode.change_name("napali");
  req.req_id = 0;
  req.ppn = 32;

  ard = new alps_req_data[3];

  record_fitting_node(num, &pnode, &naji_list, req, 0, 0, 3, JOB_TYPE_cray, csd, &ard);

  pnode.nd_id = 1;
  pnode.change_name("waimea");
  req.req_id = 1;
  req.ppn = 1;
  
  record_fitting_node(num, &pnode, &naji_list, req, 0, 1, 3, JOB_TYPE_cray, csd, &ard);

  pnode.nd_id = 4;
  pnode.change_name("wailua");
  req.ppn = 2;
  
  record_fitting_node(num, &pnode, &naji_list, req, 0, 2, 3, JOB_TYPE_cray, csd, &ard);

  pnode.nd_id = 2;
  pnode.change_name("lihue");
  req.req_id = 2;
  req.ppn = 12;
  
  record_fitting_node(num, &pnode, &naji_list, req, 0, 3, 3, JOB_TYPE_cray, csd, &ard);

  fail_unless(!strcmp(ard[0].node_list.c_str(), "napali"), ard[0].node_list.c_str());
  fail_unless(ard[0].ppn == 32);
  fail_unless(!strcmp(ard[1].node_list.c_str(), "waimea,wailua"));
  fail_unless(ard[1].ppn == 2);
  fail_unless(!strcmp(ard[2].node_list.c_str(), "lihue"));
  fail_unless(ard[2].ppn == 12);

  job *pjob = new job();
  pjob->ji_wattr[JOB_ATR_multi_req_alps].at_val.at_str = strdup("bob");
  fail_unless(add_multi_reqs_to_job(pjob, 3, NULL) == PBSE_NONE);
  fail_unless(add_multi_reqs_to_job(pjob, 3, ard) == PBSE_NONE);
 
  fail_unless(pjob->ji_wattr[JOB_ATR_multi_req_alps].at_flags == ATR_VFLAG_SET);
  fail_unless(!strcmp(pjob->ji_wattr[JOB_ATR_multi_req_alps].at_val.at_str, "napali*32|waimea,wailua*2|lihue*12"));

  }
END_TEST


START_TEST(translate_job_reservation_info_to_string_test)
  {
  std::vector<job_reservation_info> host_info;
  job_reservation_info jri[5];
  std::string          exec_host;
  std::stringstream    exec_port;

  memset(jri, 0, 5 * sizeof(job_reservation_info));

  for (int i = 0; i < 5; i++)
    {
    for (int j = 0; j < 5; j++)
      jri[i].est.add_execution_slot();
    
    jri[i].est.mark_as_used(0);
    jri[i].est.mark_as_used(1);
    jri[i].est.mark_as_used(2);

    jri[i].port = 15002;
    jri[i].node_id = i;

    host_info.push_back(jri[i]);
    }

  int count = 0;

  translate_job_reservation_info_to_string(host_info, &count, exec_host, &exec_port);
  fail_unless(exec_host == "napali0/0-2+napali1/0-2+napali2/0-2+napali3/0-2+napali4/0-2", exec_host.c_str());
  fail_unless(exec_port.str() == "15002+15002+15002+15002+15002");
  fail_unless(count == 5);
  }
END_TEST


START_TEST(populate_range_string_from_job_reservation_info_test)
  {
  std::string          range_str;
  job_reservation_info jri;

  memset(&jri, 0, sizeof(jri));
  
  for (int i = 0; i < 8; i++)
    jri.est.add_execution_slot();

  jri.est.mark_as_used(0);
  jri.est.mark_as_used(3);
  jri.est.mark_as_used(4);
  jri.est.mark_as_used(5);
  jri.node_id = 0;

  populate_range_string_from_slot_tracker(jri.est, range_str);
  fail_unless(range_str == "0,3-5");

  jri.est.mark_as_free(0);
  populate_range_string_from_slot_tracker(jri.est, range_str);
  fail_unless(range_str == "3-5", range_str.c_str());

  jri.est.mark_as_used(0);
  jri.est.mark_as_free(4);
  populate_range_string_from_slot_tracker(jri.est, range_str);
  fail_unless(range_str == "0,3,5");

  jri.est.mark_as_used(1);
  populate_range_string_from_slot_tracker(jri.est, range_str);
  fail_unless(range_str == "0-1,3,5", range_str.c_str());
 
  jri.est.mark_as_free(3);
  populate_range_string_from_slot_tracker(jri.est, range_str);
  fail_unless(range_str == "0-1,5");
  }
END_TEST


START_TEST(node_is_spec_acceptable_test)
  {
  struct pbsnode   pnode;
  single_spec_data spec;
  int              eligible_nodes = 0;

  spec.ppn = 10;

  fail_unless(node_is_spec_acceptable(&pnode, spec, NULL, &eligible_nodes, false) == false);
  fail_unless(eligible_nodes == 0);

  for (int i = 0; i < 10; i++)
    pnode.nd_slots.add_execution_slot();
    
  pnode.nd_slots.mark_as_used(4);

  fail_unless(node_is_spec_acceptable(&pnode, spec, NULL, &eligible_nodes,false) == false);
  fail_unless(eligible_nodes == 1);

  eligible_nodes = 0;
  pnode.nd_slots.mark_as_free(4);
  pnode.nd_state |= INUSE_DOWN;  
  fail_unless(node_is_spec_acceptable(&pnode, spec, NULL, &eligible_nodes,false) == false);
  fail_unless(eligible_nodes == 1);
  
  eligible_nodes = 0;
  pnode.nd_state = INUSE_FREE;
  fail_unless(node_is_spec_acceptable(&pnode, spec, NULL, &eligible_nodes,false) == true);
  fail_unless(eligible_nodes == 1);
  }
END_TEST


START_TEST(process_as_node_list_test)
  {
  std::list<node_job_add_info> naji_list;

  fail_unless(process_as_node_list("", NULL) == false);
  fail_unless(process_as_node_list(NULL, &naji_list) == false);

  fail_unless(process_as_node_list("bob:ppn=10", &naji_list) == true);
  fail_unless(process_as_node_list("bob:ppn=12", NULL) == false);

  // cray can have numeric node names so it should attempt to find the node 2 
  // and 10 in the following tests to know if they exist
  fail_unless(process_as_node_list("2:ppn=10+3:ppn=10", &naji_list) == true);
  fail_unless(process_as_node_list("2:ppn=10", &naji_list) == true);
  fail_unless(process_as_node_list("10:ppn=10", &naji_list) == false);

  // should now check the first two nodes so that it doesn't think things 
  // like nodes=bob+10:ppn=10 are hostlists
  fail_unless(process_as_node_list("bob:ppn=10+10:ppn=10", &naji_list) == false);
  fail_unless(process_as_node_list("bob+10:ppn=10", &naji_list) == false);
  fail_unless(process_as_node_list("bob+10", &naji_list) == false);

  fail_unless(process_as_node_list("bob:ppn=4|napali:ppn=2", &naji_list) == true);
  fail_unless(process_as_node_list("bob+waimea|napali", &naji_list) == true);
  }
END_TEST


START_TEST(process_job_attribute_information_test)
  {
  std::string attr_str("(cput=100,vmem=100101,mem=100020)");
  std::string jobid("2.napali");

  str_to_attr_count = 0;
  process_job_attribute_information(jobid, attr_str);
  fail_unless(str_to_attr_count == 3);
  fail_unless(decode_resc_count == 3);
  }
END_TEST


START_TEST(job_already_being_killed_test)
  {
  jobsKilled.clear();
  jobsKilled.push_back(10);

  fail_unless(job_already_being_killed(1) == false);
  fail_unless(job_already_being_killed(10) == true);
  }
END_TEST


START_TEST(remove_job_from_already_killed_list_test)
  {
  struct work_task *pwt;
  jobsKilled.clear();

  pwt = (struct work_task *)calloc(1,sizeof(struct work_task));
  pwt->wt_mutex = (pthread_mutex_t *)calloc(1,sizeof(pthread_mutex_t));
  pwt->wt_parm1 = new int(5);
  jobsKilled.push_back(1);
  jobsKilled.push_back(2);
  jobsKilled.push_back(3);
  jobsKilled.push_back(4);
  jobsKilled.push_back(5);
  jobsKilled.push_back(6);

  remove_job_from_already_killed_list(pwt);

  for (unsigned int i = 0; i < jobsKilled.size(); i++)
    fail_unless(jobsKilled[i] != 5);

  pwt = (struct work_task *)calloc(1,sizeof(struct work_task));
  pwt->wt_mutex = (pthread_mutex_t *)calloc(1,sizeof(pthread_mutex_t));
  pwt->wt_parm1 = new int(6);

  remove_job_from_already_killed_list(pwt);

  for (unsigned int i = 0; i < jobsKilled.size(); i++)
    fail_unless(jobsKilled[i] != 6);

  }
END_TEST


START_TEST(remove_job_from_node_test)
  {
  job_usage_info jui(1);
  struct pbsnode *pnode = new pbsnode();
  pnode->change_name("napali");

  for (int i = 0; i < 10; i++)
    pnode->nd_slots.add_execution_slot();

  pnode->nd_slots.reserve_execution_slots(6, jui.est);
  pnode->nd_job_usages.push_back(jui);

  fail_unless(pnode->nd_slots.get_number_free() == 4);

  remove_job_from_node(pnode, 1);
  fail_unless(pnode->nd_slots.get_number_free() == 10);
  remove_job_from_node(pnode, 1);
  fail_unless(pnode->nd_slots.get_number_free() == 10);
  }
END_TEST


START_TEST(get_next_exec_host_test)
  {
  char *exec = strdup(exec_hosts);
  char *ptr = exec;
  int   num_hosts = 9;
  int   i = 0;
  char *hostname;

  while ((hostname = get_next_exec_host(&ptr)) != NULL)
    {
    i++;

    if (i > 5)
      {
      snprintf(buf, sizeof(buf), "Expected hostname '%s' but instead received '%s'",
        l11, hostname);
      fail_unless(!strcmp(l11, hostname), buf);
      }
    else
      {
      snprintf(buf, sizeof(buf), "Expected hostname '%s' but instead received '%s'",
        napali, hostname);
      fail_unless(!strcmp(napali, hostname), buf);
      }
    }

  fail_unless(i == num_hosts, "Returned the wrong number of host names");

  free(exec);
  }
END_TEST

START_TEST(sync_node_jobs_with_moms_test)
  {
  struct pbsnode *pnode = new pbsnode();
  extern bool     job_mode;

  pnode->change_name("napali");

  job_mode = true;
  for (int i = 0; i < 9; i++)
    pnode->nd_slots.add_execution_slot();

  /* Job #1 */
  job_usage_info jui(1);
  pnode->nd_slots.reserve_execution_slots(2, jui.est);
  pnode->nd_job_usages.push_back(jui);

  /* Job #2 */
  job_usage_info jui2(2);
  pnode->nd_slots.reserve_execution_slots(4, jui2.est);
  pnode->nd_job_usages.push_back(jui2);
  
  job_usage_info jui3(3);
  pnode->nd_slots.reserve_execution_slots(3, jui3.est);
  pnode->nd_job_usages.push_back(jui3);

  /* node is fully allocated for the 3 jobs above */
  fail_unless(pnode->nd_slots.get_number_free() == 0);

  /* No jobs to be cleaned from the node */
  sync_node_jobs_with_moms(pnode, "1.lei.ac 2.lei.ac 3.lei.ac");
  fail_unless(pnode->nd_slots.get_number_free() == 0);

  /* Clean the 2nd job from the node */
  sync_node_jobs_with_moms(pnode, "1.lei.ac 3.lei.ac");
  fail_unless(pnode->nd_slots.get_number_free() == 4);

  /* Clean all jobs from the node */
  sync_node_jobs_with_moms(pnode, "");
  fail_unless(pnode->nd_slots.get_number_free() == 9);

  /* This job should not be clean as svr_find_job should find it */
  job_usage_info jui4(4);
  pnode->nd_slots.reserve_execution_slots(3, jui4.est);
  pnode->nd_job_usages.push_back(jui4);
  sync_node_jobs_with_moms(pnode, "");
  fail_unless(pnode->nd_slots.get_number_free() == 6);
  job_mode = false;
  }
END_TEST



START_TEST(job_should_be_killed_test)
  {
  struct pbsnode pnode;
  std::string    job_id;

  pnode.change_name("tom");

  fail_unless(job_should_be_killed(job_id, 2, &pnode) == true, "non-existent job shouldn't be on node");
  fail_unless(job_should_be_killed(job_id, 3, &pnode) == true, "non-existent job shouldn't be on node");
  fail_unless(job_should_be_killed(job_id, 4, &pnode) == true, "non-existent job shouldn't be on node");
  fail_unless(job_should_be_killed(job_id, 1, &pnode) == false, "false positive");
  fail_unless(job_should_be_killed(job_id, 5, &pnode) == false, "false positive");
  }
END_TEST




START_TEST(node_in_exechostlist_test)
  {
  char *eh1 = (char *)"tom/0+bob/0";
  char *eh2 = (char *)"mytom/0+tommy/0+tom1/0";
  char *node1 = (char *)"tom";
  char *node2 = (char *)"bob";
  char *node3 = (char *)"mytom";
  char *node4 = (char *)"tommy";
  char *node5 = (char *)"tom1";

  fail_unless(node_in_exechostlist(node1, eh1, NULL) == true, "blah1");
  fail_unless(node_in_exechostlist(node2, eh1, NULL) == true, "blah2");
  fail_unless(node_in_exechostlist(node3, eh1, NULL) == false, "blah3");
  fail_unless(node_in_exechostlist(node4, eh1, NULL) == false, "blah4");
  fail_unless(node_in_exechostlist(node5, eh1, NULL) == false, "blah5");
  
  fail_unless(node_in_exechostlist(node1, eh2, NULL) == false, "blah6");
  fail_unless(node_in_exechostlist(node1, eh2, NULL) == false, "blah6");
  fail_unless(node_in_exechostlist(node2, eh2, NULL) == false, "blah7");
  fail_unless(node_in_exechostlist(node3, eh2, NULL) == true, "blah8");
  fail_unless(node_in_exechostlist(node4, eh2, NULL) == true, "blah9");
  fail_unless(node_in_exechostlist(node5, eh2, NULL) == true, "blah10");
  
  // Test the login node piece working
  cray_enabled = true;
  fail_unless(node_in_exechostlist(node1, eh2, node1) == true);
  fail_unless(node_in_exechostlist(node3, eh1, node3) == true);
  fail_unless(node_in_exechostlist(node3, eh1, node1) == false);
  fail_unless(node_in_exechostlist(node4, eh1, node3) == false);
  }
END_TEST




START_TEST(check_for_node_type_test)
  {
  complete_spec_data all_reqs;
  single_spec_data   req;
  enum node_types    nt = ND_TYPE_CRAY;
  prop               p;

  memset(&all_reqs, 0, sizeof(all_reqs));

  all_reqs.num_reqs = 1;
  all_reqs.reqs.push_back(req);

  fail_unless(check_for_node_type(all_reqs, nt) == FALSE, "empty prop should always return false");
  nt = ND_TYPE_EXTERNAL;
  
  p.name = (char *)"bob";
  req.plist.push_back(p);
  all_reqs.reqs.pop_back();
  all_reqs.reqs.push_back(req);

  fail_unless(check_for_node_type(all_reqs, nt) == TRUE, "didn't find the external node");
  nt = ND_TYPE_CRAY;
  fail_unless(check_for_node_type(all_reqs, nt) == FALSE, "found a cray when only the external was requested");

  p.name = (char *)"cray";
  req.plist.pop_back();
  req.plist.push_back(p);
  all_reqs.reqs.pop_back();
  all_reqs.reqs.push_back(req);

  fail_unless(check_for_node_type(all_reqs, nt) == TRUE, "found a cray when only the external was requested");
  nt = ND_TYPE_EXTERNAL;
  fail_unless(check_for_node_type(all_reqs, nt) == FALSE, "found a cray when only the external was requested");

  }
END_TEST

START_TEST(check_node_order_test)
  {
  std::list<node_job_add_info> naji_list;
  struct pbsnode     node;
  single_spec_data   req;

  node.nd_id = 0;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 4, 0, 6) == PBSE_NONE);

  node.nd_id = 1;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 4, 0, 3) == PBSE_NONE);

  node.nd_id = 2;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 4, 0, 11) == PBSE_NONE);

  node.nd_id = 3;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 4, 0, 1) == PBSE_NONE);

  node.nd_id = 4;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 4, 0, 15) == PBSE_NONE);

  node.nd_id = 5;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 4, 0, 4) == PBSE_NONE);

  node.nd_id = 6;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 4, 0, 10) == PBSE_NONE);

  node.nd_id = 7;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 4, 0, 61) == PBSE_NONE);

  std::list<node_job_add_info>::iterator it = naji_list.begin();

  fail_unless(it->node_id == 4, "id is %d", it->node_id);
  fail_unless(it->req_order == 0, "order is %d", it->req_order);
  fail_unless(it->req_index == 0, "index is %d", it->req_index);
  it++;
  fail_unless(it->node_id == 3, "id is %d", it->node_id);
  fail_unless(it->req_order == 2, "order is %d", it->req_order);
  fail_unless(it->req_index == 1, "index is %d", it->req_index);
  it++;
  fail_unless(it->node_id == 1, "id is %d", it->node_id);
  fail_unless(it->req_order == 4, "order is %d", it->req_order);
  fail_unless(it->req_index == 3, "index is %d", it->req_index);
  it++;
  fail_unless(it->node_id == 5, "id is %d", it->node_id);
  fail_unless(it->req_order == 5, "order is %d", it->req_order);
  fail_unless(it->req_index == 4, "index is %d", it->req_index);
  it++;
  fail_unless(it->node_id == 0, "id is %d", it->node_id);
  fail_unless(it->req_order == 7, "order is %d", it->req_order);
  fail_unless(it->req_index == 6, "index is %d", it->req_index);
  it++;
  fail_unless(it->node_id == 6, "id is %d", it->node_id);
  fail_unless(it->req_order == 11, "order is %d", it->req_order);
  fail_unless(it->req_index == 10, "index is %d", it->req_index);
  it++;
  fail_unless(it->node_id == 2, "id is %d", it->node_id);
  fail_unless(it->req_order == 12, "order is %d", it->req_order);
  fail_unless(it->req_index == 11, "index is %d", it->req_index);
  it++;
  fail_unless(it->node_id == 7, "id is %d", it->node_id);
  fail_unless(it->req_order == 62, "order is %d", it->req_order);
  fail_unless(it->req_index == 61, "index is %d", it->req_index);
  
  // Make sure that req_rank -1 is placed at the front. This is how login nodes are 
  // added to jobs
  node.nd_id = 8;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 8, 0, -1) == PBSE_NONE);
  it = naji_list.begin();
  fail_unless(it->node_id == 8);

  // Now try a more realistic example - usually there are only 1 or 2 reqs in a job
  naji_list.clear();
  node.nd_id = 0;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 2, 0, 0) == PBSE_NONE);
  node.nd_id = 1;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 2, 0, 0) == PBSE_NONE);
  node.nd_id = 2;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 2, 0, 0) == PBSE_NONE);
  node.nd_id = 3;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 2, 0, 1) == PBSE_NONE);
  node.nd_id = 4;
  fail_unless(save_node_for_adding(&naji_list, &node, req, 2, 0, 1) == PBSE_NONE);
  
  it = naji_list.begin();
  fail_unless(it->node_id == 2, "id is %d", it->node_id);
  fail_unless(it->req_order == 0, "order is %d", it->req_order);
  fail_unless(it->req_index == 0, "index is %d", it->req_index);
  it++;
  fail_unless(it->node_id == 0, "id is %d", it->node_id);
  fail_unless(it->req_order == 1, "order is %d", it->req_order);
  fail_unless(it->req_index == 0, "index is %d", it->req_index);
  it++;
  fail_unless(it->node_id == 1, "id is %d", it->node_id);
  fail_unless(it->req_order == 1, "order is %d", it->req_order);
  fail_unless(it->req_index == 0, "index is %d", it->req_index);
  it++;
  fail_unless(it->node_id == 3, "id is %d", it->node_id);
  fail_unless(it->req_order == 2, "order is %d", it->req_order);
  fail_unless(it->req_index == 1, "index is %d", it->req_index);
  it++;
  fail_unless(it->node_id == 4, "id is %d", it->node_id);
  fail_unless(it->req_order == 2, "order is %d", it->req_order);
  fail_unless(it->req_index == 1, "index is %d", it->req_index);
  }
END_TEST


START_TEST(record_external_node_test)
  {
  job            pjob;
  struct pbsnode pnode1;
  struct pbsnode pnode2;
  struct pbsnode pnode3;
  char           buf[4096];

  pnode1.change_name("tom");
  pnode2.change_name("bob");
  pnode3.change_name("jim");

  record_external_node(&pjob, &pnode1);
  snprintf(buf, sizeof(buf), "attr should be tom but is %s",
    pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str);
  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str, "tom"), buf);

  record_external_node(&pjob, &pnode2);
  snprintf(buf, sizeof(buf), "attr should be tom+bob but is %s",
    pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str);
  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str, "tom+bob"), buf);

  record_external_node(&pjob, &pnode3);
  snprintf(buf, sizeof(buf), "attr should be tom+bob+jim but is %s",
    pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str);
  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str, "tom+bob+jim"), buf);
  }
END_TEST


START_TEST(place_subnodes_in_hostlist_job_exclusive_test)
  {
  job pjob;
  strcpy(pjob.ji_qs.ji_jobid, "1.lei");

  struct pbsnode *pnode = new pbsnode();
  pnode->change_name("napali");

  for (int i = 0; i < 9; i++)
    pnode->nd_slots.add_execution_slot();

  pnode->nd_state = INUSE_FREE;

  job_usage_info *jui = (job_usage_info *)calloc(1, sizeof(job_usage_info));
  jui->internal_job_id = 1;

  pnode->nd_slots.reserve_execution_slots(1, jui->est);
  pnode->nd_job_usages.push_back(*jui);

  fail_unless(pnode->nd_state == 0, "Node state has garbage");

  node_job_add_info *naji = (node_job_add_info *)calloc(1,sizeof(node_job_add_info));
  naji->node_id = 1;
  naji->req_order = 1;

  char buf[10];
  buf[0] = '\0';

  /* set job_exclusive_on_use true */
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_flags=ATR_VFLAG_SET;
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_val.at_long = 1;

  job_reservation_info jri;
  int rc =  place_subnodes_in_hostlist(&pjob, pnode, *naji, jri, buf);

  fail_unless((rc == PBSE_NONE), "Call to place_subnodes_in_hostlit failed");
  fail_unless(pnode->nd_state == INUSE_JOB, "Call to place_subnodes_in_hostlit was not set to job exclusive state");

  /* turn job_exclusive_on_use off and reset the node state */
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_flags=ATR_VFLAG_SET;
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_val.at_long = 0;
  pnode->nd_state = 0;

  job_reservation_info jri2;
  rc = place_subnodes_in_hostlist(&pjob, pnode, *naji, jri2, buf);
  fail_unless((rc == PBSE_NONE), "2nd call to place_subnodes_in_hostlit failed");
  fail_unless(pnode->nd_state != INUSE_JOB, "2nd call to place_subnodes_in_hostlit was not set to job exclusive state");

  /* test case when the attribute SVR_ATR_JobExclusiveOnUse was never set */
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_flags=0;
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_val.at_long = 0;
  pnode->nd_state = 0;

  job_reservation_info jri3;
  rc = place_subnodes_in_hostlist(&pjob, pnode, *naji, jri3, buf);
  fail_unless((rc == PBSE_NONE), "3rd call to place_subnodes_in_hostlit failed");
  fail_unless(pnode->nd_state != INUSE_JOB, "3rd call to place_subnodes_in_hostlit was not set to job exclusive state");
  }
END_TEST


Suite *node_manager_suite(void)
  {
  Suite *s = suite_create("node_manager_suite methods");
  TCase *tc_core = tcase_create("get_next_exec_host_test");
  tcase_add_test(tc_core, get_next_exec_host_test);
  tcase_add_test(tc_core, test_add_remove_mic_jobs);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("job_should_be_killed_test");
  tcase_add_test(tc_core, job_should_be_killed_test);
  tcase_add_test(tc_core, remove_job_from_already_killed_list_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_kill_job_on_mom");
  tcase_add_test(tc_core, test_kill_job_on_mom);
  suite_add_tcase(s, tc_core); 

  tc_core = tcase_create("node_in_exechostlist_test");
  tcase_add_test(tc_core, node_in_exechostlist_test);
  tcase_add_test(tc_core, remove_job_from_node_test);
  tcase_add_test(tc_core, job_already_being_killed_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("check_for_node_type_test");
  tcase_add_test(tc_core, check_for_node_type_test);
  tcase_add_test(tc_core, process_job_attribute_information_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("check_node_order_test");
  tcase_add_test(tc_core, check_node_order_test);
  tcase_add_test(tc_core, process_as_node_list_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("sync_node_jobs_with_moms_test");
  tcase_add_test(tc_core, sync_node_jobs_with_moms_test);
#ifdef PENABLE_LINUX_CGROUPS
  tcase_add_test(tc_core, test_save_cpus_and_memory_cpusets);
#endif
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("place_subnodes_in_hostlist_job_exclusive_test");
  tcase_add_test(tc_core, place_subnodes_in_hostlist_job_exclusive_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("record_external_node_test");
  tcase_add_test(tc_core, record_external_node_test);
  tcase_add_test(tc_core, test_update_failure_counts);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("more tests");
  tcase_add_test(tc_core, translate_job_reservation_info_to_string_test);
  tcase_add_test(tc_core, test_initialize_alps_req_data);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("even more tests");
  tcase_add_test(tc_core, node_is_spec_acceptable_test);
  tcase_add_test(tc_core, populate_range_string_from_job_reservation_info_test);
  tcase_add_test(tc_core, check_node_jobs_exitence_test);
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
  sr = srunner_create(node_manager_suite());
  srunner_set_log(sr, "node_manager_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }


