#include <string>
#include <vector>
#include "license_pbs.h" /* See here for the software license */
#include "node_manager.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "server.h" /* server */

const char *exec_hosts = "napali/0+napali/1+napali/2+napali/50+napali/4+l11/0+l11/1+l11/2+l11/3";
char  buf[4096];
const char *napali = "napali";
const char *l11 =    "l11";
struct server server;

int   remove_job_from_node(struct pbsnode *pnode, const char *jobid);
int   node_in_exechostlist(char *, char *);
char *get_next_exec_host(char **);
int   job_should_be_killed(char *, struct pbsnode *);
int   check_for_node_type(complete_spec_data *, enum node_types);
int   record_external_node(job *, struct pbsnode *);
void *record_reported_time(void *vp);
int save_node_for_adding(node_job_add_info *naji,struct pbsnode *pnode,single_spec_data *req,char *first_node_name,int is_external_node,int req_rank);
void remove_job_from_already_killed_list(struct work_task *pwt);
extern job_reservation_info *place_subnodes_in_hostlist(job *pjob, struct pbsnode *pnode, node_job_add_info *naji, char *ProcBMStr);


extern std::vector<std::string> jobsKilled;

START_TEST(remove_job_from_already_killed_list_test)
  {
  struct work_task *pwt;

  pwt = (struct work_task *)calloc(1,sizeof(struct work_task));
  pwt->wt_mutex = (pthread_mutex_t *)calloc(1,sizeof(pthread_mutex_t));
  pwt->wt_parm1 = (void *)new std::string("Job 5");
  std::string s("Job 1");
  jobsKilled.push_back(s);
  s = "Job 2";
  jobsKilled.push_back(s);
  s = "Job 3";
  jobsKilled.push_back(s);
  s = "Job 4";
  jobsKilled.push_back(s);
  s = "Job 5";
  jobsKilled.push_back(s);
  s = "Job 6";
  jobsKilled.push_back(s);

  remove_job_from_already_killed_list(pwt);

  for(std::vector<std::string>::iterator i = jobsKilled.begin();i != jobsKilled.end();i++)
    {
    fail_unless(strcmp((*i).c_str(),"Job 5") != 0);
    }

  pwt = (struct work_task *)calloc(1,sizeof(struct work_task));
  pwt->wt_mutex = (pthread_mutex_t *)calloc(1,sizeof(pthread_mutex_t));
  pwt->wt_parm1 = (void *)new std::string("Job 6");

  remove_job_from_already_killed_list(pwt);

  for(std::vector<std::string>::iterator i = jobsKilled.begin();i != jobsKilled.end();i++)
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

START_TEST(record_reported_time_test)
  {
  job *pjob;

  record_reported_time(strdup("1:tom"));
  pjob = svr_find_job(strdup("1"), TRUE);

  fail_unless(time(NULL) - pjob->ji_last_reported_time < 10);
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
  fail_unless(check_for_node_type(&all_reqs, nt) == FALSE, "empty prop should always return false");

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
  struct pbsnode    node;
  single_spec_data   req;

  memset(&req,0,sizeof(single_spec_data));

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_name = (char *)"first";
  fail_unless(save_node_for_adding(pBase,&node,&req,(char *)"Mother Superior",0,6) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_name = (char *)"second";
  fail_unless(save_node_for_adding(pBase,&node,&req,(char *)"Mother Superior",0,3) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_name = (char *)"third";
  fail_unless(save_node_for_adding(pBase,&node,&req,(char *)"Mother Superior",0,11) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_name = (char *)"fourth";
  fail_unless(save_node_for_adding(pBase,&node,&req,(char *)"Mother Superior",0,1) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_name = (char *)"Mother Superior";
  fail_unless(save_node_for_adding(pBase,&node,&req,(char *)"Mother Superior",0,15) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_name = (char *)"fifth";
  fail_unless(save_node_for_adding(pBase,&node,&req,(char *)"Mother Superior",0,4) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_name = (char *)"sixth";
  fail_unless(save_node_for_adding(pBase,&node,&req,(char *)"Mother Superior",0,10) == PBSE_NONE);

  memset(&node,0,sizeof(struct pbsnode));
  node.nd_name = (char *)"seventh";
  fail_unless(save_node_for_adding(pBase,&node,&req,(char *)"Mother Superior",0,61) == PBSE_NONE);

  node_job_add_info *index = pBase;

  fail_unless(strcmp(index->node_name,"Mother Superior") == 0);
  index = index->next;
  fail_unless(strcmp(index->node_name,"fourth") == 0);
  index = index->next;
  fail_unless(strcmp(index->node_name,"second") == 0);
  index = index->next;
  fail_unless(strcmp(index->node_name,"fifth") == 0);
  index = index->next;
  fail_unless(strcmp(index->node_name,"first") == 0);
  index = index->next;
  fail_unless(strcmp(index->node_name,"sixth") == 0);
  index = index->next;
  fail_unless(strcmp(index->node_name,"third") == 0);
  index = index->next;
  fail_unless(strcmp(index->node_name,"seventh") == 0);
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

START_TEST(place_subnodes_in_hostlist_job_exclusive_test)
  {
  job pjob;
  memset(&pjob, 0, sizeof(job));
  strcpy(pjob.ji_qs.ji_jobid, "1.lei");

  struct pbsnode *pnode = (struct pbsnode *)calloc(1, sizeof(struct pbsnode));
  for (int i = 0; i < 9; i++)
    pnode->nd_slots.add_execution_slot();

  job_usage_info *jui = (job_usage_info *)calloc(1, sizeof(job_usage_info));
  strcpy(jui->jobid, "1.lei.ac");

  pnode->nd_slots.reserve_execution_slots(1, jui->est);
  pnode->nd_job_usages.push_back(jui);

  fail_unless(pnode->nd_state == 0, "Node state has garbage");

  node_job_add_info *naji = (node_job_add_info *)calloc(1,sizeof(node_job_add_info));
  strcpy(naji->node_name, "lei");
  naji->req_rank = 1;

  char buf[10];
  buf[0] = '\0';

  /* set job_exclusive_on_use true */
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_flags=ATR_VFLAG_SET;
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_val.at_long = 1;

  job_reservation_info *jri = place_subnodes_in_hostlist(&pjob, pnode, naji, buf);

  fail_unless((jri != NULL), "Call to place_subnodes_in_hostlit failed");
  fail_unless(pnode->nd_state == INUSE_JOB, "Call to place_subnodes_in_hostlit was not set to job exclusive state");

  /* turn job_exclusive_on_use off and reset the node state */
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_flags=ATR_VFLAG_SET;
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_val.at_long = 0;
  pnode->nd_state = 0;

  jri = place_subnodes_in_hostlist(&pjob, pnode, naji, buf);
  fail_unless((jri != NULL), "2nd call to place_subnodes_in_hostlit failed");
  fail_unless(pnode->nd_state != INUSE_JOB, "2nd call to place_subnodes_in_hostlit was not set to job exclusive state");

  /* test case when the attribute SVR_ATR_JobExclusiveOnUse was never set */
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_flags=0;
  server.sv_attr[SRV_ATR_JobExclusiveOnUse].at_val.at_long = 0;
  pnode->nd_state = 0;

  jri = place_subnodes_in_hostlist(&pjob, pnode, naji, buf);
  fail_unless((jri != NULL), "3rd call to place_subnodes_in_hostlit failed");
  fail_unless(pnode->nd_state != INUSE_JOB, "3rd call to place_subnodes_in_hostlit was not set to job exclusive state");
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

  tc_core = tcase_create("place_subnodes_in_hostlist_job_exclusive_test");
  tcase_add_test(tc_core, place_subnodes_in_hostlist_job_exclusive_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("record_external_node_test");
  tcase_add_test(tc_core, record_external_node_test);
  tcase_add_test(tc_core, record_reported_time_test);
  tcase_add_test(tc_core, remove_job_from_node_test);
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


