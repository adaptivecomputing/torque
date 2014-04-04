#include <string>
#include <sstream>
#include <boost/ptr_container/ptr_vector.hpp>
#include "license_pbs.h" /* See here for the software license */
#include "node_manager.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"


const char *exec_hosts = "napali/0+napali/1+napali/2+napali/50+napali/4+l11/0+l11/1+l11/2+l11/3";
char  buf[4096];
const char *napali = "napali";
const char *l11 =    "l11";

int   remove_job_from_node(struct pbsnode *pnode, const char *jobid);
int   node_in_exechostlist(char *, char *);
char *get_next_exec_host(char **);
int   job_should_be_killed(char *, struct pbsnode *);
int   check_for_node_type(complete_spec_data *, enum node_types);
int   record_external_node(job *, struct pbsnode *);
int save_node_for_adding(node_job_add_info *naji, struct pbsnode *pnode, single_spec_data *req, int first_node_id, int is_external_node, int req_rank);
void remove_job_from_already_killed_list(struct work_task *pwt);
bool job_already_being_killed(const char *jobid);
void process_job_attribute_information(std::string &job_id, std::string &attributes);
bool process_as_node_list(const char *spec, const node_job_add_info *naji);
bool node_is_spec_acceptable(struct pbsnode *pnode, single_spec_data *spec, char *ProcBMStr, int *eligible_nodes,bool isExclusive);
void populate_range_string_from_slot_tracker(execution_slot_tracker &est, std::string &range_str);
int  translate_job_reservation_info_to_string(std::vector<job_reservation_info *> &host_info, int *NCount, std::string &exec_host_output, std::stringstream *exec_port_output);

extern boost::ptr_vector<std::string> jobsKilled;

extern int str_to_attr_count;
extern int decode_resc_count;


START_TEST(translate_job_reservation_info_to_stirng_test)
  {
  std::vector<job_reservation_info *> host_info;
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

    host_info.push_back(jri + i);
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

  memset(&pnode, 0, sizeof(pnode));
  memset(&spec, 0, sizeof(spec));

  spec.ppn = 10;

  fail_unless(node_is_spec_acceptable(&pnode, &spec, NULL, &eligible_nodes,false) == false);
  fail_unless(eligible_nodes == 0);

  for (int i = 0; i < 10; i++)
    pnode.nd_slots.add_execution_slot();
    
  pnode.nd_slots.mark_as_used(4);

  fail_unless(node_is_spec_acceptable(&pnode, &spec, NULL, &eligible_nodes,false) == false);
  fail_unless(eligible_nodes == 1);

  eligible_nodes = 0;
  pnode.nd_slots.mark_as_free(4);
  pnode.nd_state |= INUSE_DOWN;  
  fail_unless(node_is_spec_acceptable(&pnode, &spec, NULL, &eligible_nodes,false) == false);
  fail_unless(eligible_nodes == 1);
  
  eligible_nodes = 0;
  pnode.nd_state = INUSE_FREE;
  fail_unless(node_is_spec_acceptable(&pnode, &spec, NULL, &eligible_nodes,false) == true);
  fail_unless(eligible_nodes == 1);
  }
END_TEST


START_TEST(process_as_node_list_test)
  {
  node_job_add_info naji;

  fail_unless(process_as_node_list("", NULL) == false);
  fail_unless(process_as_node_list(NULL, &naji) == false);

  fail_unless(process_as_node_list("bob:ppn=10", &naji) == true);
  fail_unless(process_as_node_list("bob:ppn=12", NULL) == false);

  // cray can have numeric node names so it should attempt to find the node 2 
  // and 10 in the following tests to know if they exist
  fail_unless(process_as_node_list("2:ppn=10+3:ppn=10", &naji) == true);
  fail_unless(process_as_node_list("2:ppn=10", &naji) == true);
  fail_unless(process_as_node_list("10:ppn=10", &naji) == false);

  // should now check the first two nodes so that it doesn't think things 
  // like nodes=bob+10:ppn=10 are hostlists
  fail_unless(process_as_node_list("bob:ppn=10+10:ppn=10", &naji) == false);
  fail_unless(process_as_node_list("bob+10:ppn=10", &naji) == false);
  fail_unless(process_as_node_list("bob+10", &naji) == false);
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
  jobsKilled.push_back(new std::string("10.napali"));

  fail_unless(job_already_being_killed("1.napali") == false);
  fail_unless(job_already_being_killed("10.napali") == true);
  }
END_TEST


START_TEST(remove_job_from_already_killed_list_test)
  {
  struct work_task *pwt;

  pwt = (struct work_task *)calloc(1,sizeof(struct work_task));
  pwt->wt_mutex = (pthread_mutex_t *)calloc(1,sizeof(pthread_mutex_t));
  pwt->wt_parm1 = (void *)new std::string("Job 5");
  jobsKilled.push_back(new std::string("Job 1"));
  jobsKilled.push_back(new std::string("Job 2"));
  jobsKilled.push_back(new std::string("Job 3"));
  jobsKilled.push_back(new std::string("Job 4"));
  jobsKilled.push_back(new std::string("Job 5"));
  jobsKilled.push_back(new std::string("Job 6"));

  remove_job_from_already_killed_list(pwt);

  for(boost::ptr_vector<std::string>::iterator i = jobsKilled.begin();i != jobsKilled.end();i++)
    {
    fail_unless(strcmp(i->c_str(),"Job 5") != 0);
    }

  pwt = (struct work_task *)calloc(1,sizeof(struct work_task));
  pwt->wt_mutex = (pthread_mutex_t *)calloc(1,sizeof(pthread_mutex_t));
  pwt->wt_parm1 = (void *)new std::string("Job 6");

  remove_job_from_already_killed_list(pwt);

  for(boost::ptr_vector<std::string>::iterator i = jobsKilled.begin();i != jobsKilled.end();i++)
    {
    fail_unless(strcmp(i->c_str(),"Job 6") != 0);
    }

  }
END_TEST

START_TEST(remove_job_from_node_test)
  {
  job pjob;

  strcpy(pjob.ji_qs.ji_jobid, "1.napali");
  job_usage_info *jui = (job_usage_info *)calloc(1, sizeof(job_usage_info));
  strcpy(jui->jobid, "1.napali");
  struct pbsnode *pnode = (struct pbsnode *)calloc(1, sizeof(struct pbsnode));

  for (int i = 0; i < 10; i++)
    pnode->nd_slots.add_execution_slot();

  pnode->nd_slots.reserve_execution_slots(6, jui->est);
  pnode->nd_job_usages.push_back(jui);

  fail_unless(pnode->nd_slots.get_number_free() == 4);

  remove_job_from_node(pnode, (const char *)pjob.ji_qs.ji_jobid);
  fail_unless(pnode->nd_slots.get_number_free() == 10);
  remove_job_from_node(pnode, (const char *)pjob.ji_qs.ji_jobid);
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
  struct pbsnode *pnode = (struct pbsnode *)calloc(1, sizeof(struct pbsnode));
  for (int i = 0; i < 9; i++)
    pnode->nd_slots.add_execution_slot();

  /* Job #1 */
  job_usage_info *jui = (job_usage_info *)calloc(1, sizeof(job_usage_info));
  strcpy(jui->jobid, "1.lei.ac");
  pnode->nd_slots.reserve_execution_slots(2, jui->est);
  pnode->nd_job_usages.push_back(jui);

  /* Job #2 */
  jui = (job_usage_info *)calloc(1, sizeof(job_usage_info));
  strcpy(jui->jobid, "2.lei.ac");
  pnode->nd_slots.reserve_execution_slots(4, jui->est);
  pnode->nd_job_usages.push_back(jui);
  
  jui = (job_usage_info *)calloc(1, sizeof(job_usage_info));
  strcpy(jui->jobid, "3.lei.ac");
  pnode->nd_slots.reserve_execution_slots(3, jui->est);
  pnode->nd_job_usages.push_back(jui);

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
  jui = (job_usage_info *)calloc(1, sizeof(job_usage_info));
  strcpy(jui->jobid, "4.noclean.ac");
  pnode->nd_slots.reserve_execution_slots(3, jui->est);
  pnode->nd_job_usages.push_back(jui);
  sync_node_jobs_with_moms(pnode, "");
  fail_unless(pnode->nd_slots.get_number_free() == 6);
  }
END_TEST



START_TEST(job_should_be_killed_test)
  {
  struct pbsnode pnode;
  struct jobinfo jinfo;

  memset(&pnode, 0, sizeof(pnode));
  memset(&jinfo, 0, sizeof(jinfo));

  pnode.nd_name = (char *)"tom";
  strcpy(jinfo.jobid, "1");

  fail_unless(job_should_be_killed((char *)"2", &pnode) == true, "non-existent job shouldn't be on node");
  fail_unless(job_should_be_killed((char *)"3", &pnode) == true, "non-existent job shouldn't be on node");
  fail_unless(job_should_be_killed((char *)"4", &pnode) == true, "non-existent job shouldn't be on node");
  fail_unless(job_should_be_killed((char *)"1", &pnode) == false, "false positive");
  fail_unless(job_should_be_killed((char *)"5", &pnode) == false, "false positive");
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

  fail_unless(node_in_exechostlist(node1, eh1) == TRUE, "blah1");
  fail_unless(node_in_exechostlist(node2, eh1) == TRUE, "blah2");
  fail_unless(node_in_exechostlist(node3, eh1) == FALSE, "blah3");
  fail_unless(node_in_exechostlist(node4, eh1) == FALSE, "blah4");
  fail_unless(node_in_exechostlist(node5, eh1) == FALSE, "blah5");
  
  fail_unless(node_in_exechostlist(node1, eh2) == FALSE, "blah6");
  fail_unless(node_in_exechostlist(node1, eh2) == FALSE, "blah6");
  fail_unless(node_in_exechostlist(node2, eh2) == FALSE, "blah7");
  fail_unless(node_in_exechostlist(node3, eh2) == TRUE, "blah8");
  fail_unless(node_in_exechostlist(node4, eh2) == TRUE, "blah9");
  fail_unless(node_in_exechostlist(node5, eh2) == TRUE, "blah10");
  }
END_TEST




START_TEST(check_for_node_type_test)
  {
  complete_spec_data all_reqs;
  single_spec_data   req;
  enum node_types    nt = ND_TYPE_CRAY;
  struct prop        p;

  memset(&all_reqs, 0, sizeof(all_reqs));
  memset(&req, 0, sizeof(req));
  memset(&p, 0, sizeof(p));

  all_reqs.num_reqs = 1;
  all_reqs.reqs = &req;

  fail_unless(check_for_node_type(&all_reqs, nt) == FALSE, "empty prop should always return false");
  nt = ND_TYPE_EXTERNAL;
  
  p.name = (char *)"bob";
  req.prop = &p;

  fail_unless(check_for_node_type(&all_reqs, nt) == TRUE, "didn't find the external node");
  nt = ND_TYPE_CRAY;
  fail_unless(check_for_node_type(&all_reqs, nt) == FALSE, "found a cray when only the external was requested");

  p.name = (char *)"cray";
  fail_unless(check_for_node_type(&all_reqs, nt) == TRUE, "found a cray when only the external was requested");
  nt = ND_TYPE_EXTERNAL;
  fail_unless(check_for_node_type(&all_reqs, nt) == FALSE, "found a cray when only the external was requested");

  }
END_TEST

START_TEST(check_node_order_test)
  {
  node_job_add_info *pBase = (node_job_add_info *)calloc(1,sizeof(node_job_add_info));
  struct pbsnode     node;
  single_spec_data   req;

  memset(&req,0,sizeof(single_spec_data));
  pBase->node_id = -1;

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_id = 0;
  fail_unless(save_node_for_adding(pBase,&node,&req,4,0,6) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_id = 1;
  fail_unless(save_node_for_adding(pBase,&node,&req,4,0,3) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_id = 2;
  fail_unless(save_node_for_adding(pBase,&node,&req,4,0,11) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_id = 3;
  fail_unless(save_node_for_adding(pBase,&node,&req,4,0,1) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_id = 4;
  fail_unless(save_node_for_adding(pBase,&node,&req,4,0,15) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_id = 5;
  fail_unless(save_node_for_adding(pBase,&node,&req,4,0,4) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_id = 6;
  fail_unless(save_node_for_adding(pBase,&node,&req,4,0,10) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_id = 7;
  fail_unless(save_node_for_adding(pBase,&node,&req,4,0,61) == PBSE_NONE);

  node_job_add_info *index = pBase;

  fail_unless(index->node_id == 4);
  index = index->next;
  fail_unless(index->node_id == 3);
  index = index->next;
  fail_unless(index->node_id == 1);
  index = index->next;
  fail_unless(index->node_id == 5);
  index = index->next;
  fail_unless(index->node_id == 0);
  index = index->next;
  fail_unless(index->node_id == 6);
  index = index->next;
  fail_unless(index->node_id == 2);
  index = index->next;
  fail_unless(index->node_id == 7);
  }
END_TEST

START_TEST(record_external_node_test)
  {
  job            pjob;
  struct pbsnode pnode1;
  struct pbsnode pnode2;
  struct pbsnode pnode3;
  char           buf[4096];

  memset(&pjob, 0, sizeof(pjob));
  memset(&pnode1, 0, sizeof(pnode1));
  memset(&pnode2, 0, sizeof(pnode2));
  memset(&pnode3, 0, sizeof(pnode3));

  pnode1.nd_name = (char *)"tom";
  pnode2.nd_name = (char *)"bob";
  pnode3.nd_name = (char *)"jim";

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




Suite *node_manager_suite(void)
  {
  Suite *s = suite_create("node_manager_suite methods");
  TCase *tc_core = tcase_create("get_next_exec_host_test");
  tcase_add_test(tc_core, get_next_exec_host_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("job_should_be_killed_test");
  tcase_add_test(tc_core, job_should_be_killed_test);
  tcase_add_test(tc_core, remove_job_from_already_killed_list_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("node_in_exechostlist_test");
  tcase_add_test(tc_core, node_in_exechostlist_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("check_for_node_type_test");
  tcase_add_test(tc_core, check_for_node_type_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("check_node_order_test");
  tcase_add_test(tc_core, check_node_order_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("sync_node_jobs_with_moms_test");
  tcase_add_test(tc_core, sync_node_jobs_with_moms_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("record_external_node_test");
  tcase_add_test(tc_core, record_external_node_test);
  tcase_add_test(tc_core, remove_job_from_node_test);
  tcase_add_test(tc_core, job_already_being_killed_test);
  tcase_add_test(tc_core, process_job_attribute_information_test);
  tcase_add_test(tc_core, process_as_node_list_test);
  tcase_add_test(tc_core, node_is_spec_acceptable_test);
  tcase_add_test(tc_core, populate_range_string_from_job_reservation_info_test);
  tcase_add_test(tc_core, translate_job_reservation_info_to_stirng_test);
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


