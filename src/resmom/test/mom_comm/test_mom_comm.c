#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include "pbs_nodes.h"
#include "mom_comm.h"
#include <set>
#include <stdlib.h>
#include <stdio.h>

#include "dis.h"
#include "pbs_error.h"
#include "pbs_job.h"
#include "test_mom_comm.h"
#include "resmon.h"
#include "mom_server.h"

extern int disrsi_return_index;
extern int disrst_return_index;
extern int disrsi_array[];
extern char *disrst_array[];
extern int log_event_counter;
extern bool ms_val;
extern mom_server mom_servers[PBS_MAXSERVER];
extern int ServerStatUpdateInterval;
extern time_t LastServerUpdateTime;
extern time_t time_now;
extern bool ForceServerUpdate;

#define IM_DONE                     0
#define IM_FAILURE                 -1


#define IM_DONE                     0
#define IM_FAILURE                 -1


int process_end_job_error_reply(job *pjob, hnodent *np, struct sockaddr_in *pSockAddr, int errcode);
void create_contact_list(job &pjob, std::set<int> &sister_list, struct sockaddr_in *contacting_address);
int handle_im_poll_job_response(struct tcp_chan *chan, job &pjob, int nodeidx, hnodent *np);
received_node *get_received_node_entry(char *str);
bool is_nodeid_on_this_host(job *pjob, tm_node_id nodeid);


START_TEST(is_nodeid_on_this_host_test)
  {
  job pjob;

  pjob.ji_vnods = (vnodent *)calloc(2, sizeof(vnodent));
  pjob.ji_vnods[0].vn_host = (hnodent *)0x0011;
  pjob.ji_vnods[1].vn_host = (hnodent *)0x0022;
  pjob.ji_nodeid = 0;

  fail_unless(is_nodeid_on_this_host(&pjob, 0) == true);
  fail_unless(is_nodeid_on_this_host(&pjob, 1) == false);

  pjob.ji_nodeid = 1;
  fail_unless(is_nodeid_on_this_host(&pjob, 0) == false);
  fail_unless(is_nodeid_on_this_host(&pjob, 1) == true);
  }
END_TEST


START_TEST(test_process_end_job_error_reply)
  {
  job                *pjob = (job *)calloc(1, sizeof(job));
  hnodent             np;
  struct sockaddr_in  psock;

  pjob->ji_hosts = (hnodent *)calloc(4, sizeof(hnodent));
  pjob->ji_hosts[0].hn_sister = SISTER_OKAY;
  pjob->ji_hosts[1].hn_sister = SISTER_OKAY;
  pjob->ji_hosts[2].hn_sister = SISTER_OKAY;
  pjob->ji_hosts[3].hn_sister = SISTER_OKAY;

  fail_unless(process_end_job_error_reply(pjob, &np, &psock, PBSE_UNKJOBID) == -1);

  pjob->ji_numnodes = 4;
  pjob->ji_qs.ji_svrflags = JOB_SVFLG_HERE;
  log_event_counter = 0;
  fail_unless(process_end_job_error_reply(pjob, &np, &psock, PBSE_UNKJOBID) == 0);
  fail_unless(log_event_counter == 1);
  }
END_TEST



START_TEST(handle_im_poll_job_response_test)
  {
  job             *pjob = (job *)calloc(1, sizeof(job));
  struct tcp_chan *chan = (struct tcp_chan *)calloc(1, sizeof(struct tcp_chan));
  hnodent         *np   = (hnodent *)calloc(1, sizeof(hnodent));
  pjob->ji_resources = (noderes *)calloc(10, sizeof(noderes));

  disrsi_return_index = 0;
  np->hn_node = 4;

  fail_unless(handle_im_poll_job_response(chan, *pjob, 4, np) == -1);
  pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HERE;

  fail_unless(handle_im_poll_job_response(chan, *pjob, 4, np) == 0);
  fail_unless(pjob->ji_resources[3].nr_mem < pjob->ji_resources[3].nr_vmem);
  }
END_TEST

START_TEST(handle_im_obit_task_response_test)
  {
  job             *pjob = (job *)calloc(1, sizeof(job));
  struct tcp_chan *chan = (struct tcp_chan *)calloc(1, sizeof(struct tcp_chan));
  task             *ptask = (task *)calloc(1,sizeof(task));
  pjob->ji_tasks.ll_next = &ptask->ti_jobtask;
  pjob->ji_tasks.ll_prior = &ptask->ti_jobtask;
  ptask->ti_jobtask.ll_struct = ptask;
  ptask->ti_jobtask.ll_next = &pjob->ji_tasks;
  ptask->ti_jobtask.ll_prior = &pjob->ji_tasks;
  ptask->ti_qs.ti_task = TM_INIT;
  ptask->ti_chan = chan;

  disrsi_return_index = 500;
  fail_unless(handle_im_obit_task_response(chan,pjob,TM_NULL_TASK,42) == IM_FAILURE);

  disrsi_return_index = 0;
  fail_unless(handle_im_obit_task_response(chan,pjob,TM_NULL_TASK,42) == IM_DONE);

  disrsi_return_index = 0;
  fail_unless(handle_im_obit_task_response(chan,pjob,TM_INIT,42) == IM_FAILURE);

  chan->readbuf.tdis_bufsize = 5;
  chan->writebuf.tdis_bufsize = 5;

  disrsi_return_index = 0;
  fail_unless(handle_im_obit_task_response(chan,pjob,TM_INIT,42) == IM_DONE);
  }
END_TEST


START_TEST(create_contact_list_test)
  {
  job pjob;
  std::set<int> contact_list;
  struct sockaddr_in addr;

  memset(&pjob, 0, sizeof(pjob));
  memset(&addr, 0, sizeof(addr));

  pjob.ji_qs.ji_svrflags |= JOB_SVFLG_INTERMEDIATE_MOM;
  addr.sin_addr.s_addr = htonl(100);
  pjob.ji_numnodes = 3;
  pjob.ji_sisters = (hnodent *)calloc(3, sizeof(hnodent));
  pjob.ji_sisters[1].sock_addr.sin_addr.s_addr = htonl(100);
  pjob.ji_sisters[2].sock_addr.sin_addr.s_addr = htonl(101);

  create_contact_list(pjob, contact_list, &addr);
  fail_unless(contact_list.size() == 1);
  contact_list.clear();

  create_contact_list(pjob, contact_list, NULL);
  fail_unless(contact_list.size() == 2);
  contact_list.clear();

  pjob.ji_qs.ji_svrflags &= ~JOB_SVFLG_INTERMEDIATE_MOM;
  pjob.ji_hosts = pjob.ji_sisters;
  pjob.ji_sisters = NULL;

  create_contact_list(pjob, contact_list, &addr);
  fail_unless(contact_list.size() == 1);
  contact_list.clear();

  create_contact_list(pjob, contact_list, NULL);
  fail_unless(contact_list.size() == 2);

  }
END_TEST

START_TEST(test_read_status_strings_null_chan_doesnt_crash)
  {
  fail_unless(DIS_INVALID == read_status_strings(NULL, 1));
  }
END_TEST


START_TEST(test_read_status_strings_loop)
  {
  struct tcp_chan chan;
  disrst_return_index = 0;
  disrsi_return_index = 0;
  disrsi_array[0] = DIS_SUCCESS;
  disrsi_array[1] = DIS_SUCCESS;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  read_status_strings(&chan, 1);
  }
END_TEST

START_TEST(test_get_received_node_entry)
  {
  fail_unless(get_received_node_entry(strdup("pickle")) != NULL);
  }
END_TEST

START_TEST(task_save_test)
  {
  int result = 0;
  struct task test_task;
  struct job test_job;
  const char *file_prefix = "prefix";

  memset(&test_task, 0, sizeof(test_task));
  memset(&test_job, 0, sizeof(test_job));

  result = task_save(NULL);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL input fail");

  result = task_save(&test_task);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL pointer to owning job fail");

  strncpy(test_job.ji_qs.ji_fileprefix,
          file_prefix,
          sizeof(test_job.ji_qs.ji_fileprefix) - 1);

  strcpy(test_task.ti_qs.ti_parentjobid, "jobid");
  
  extern job *mock_mom_find_job_return;
  mock_mom_find_job_return = NULL;
  result = task_save(&test_task);
  fail_unless(result == -1, "task_save fail");
  }
END_TEST

START_TEST(im_request_test)
  {
  struct tcp_chan test_chan;
  struct sockaddr_in sockaddr;

  memset(&test_chan, 0, sizeof(test_chan));
  memset(&sockaddr,0,sizeof(sockaddr));

  im_request(&test_chan, 0,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("other");
  disrst_array[1] = strdup("other");
  disrsi_array[0] = IM_KILL_JOB;
  disrsi_array[0] = 0;
  disrsi_array[1] = 0;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_ALL_OKAY;
  disrsi_array[0] = 0;
  disrsi_array[1] = 0;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_JOIN_JOB;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_KILL_JOB;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_SPAWN_TASK;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_GET_TASKS;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_SIGNAL_TASK;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_OBIT_TASK;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_POLL_JOB;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_GET_INFO;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_GET_RESC;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_ABORT_JOB;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_GET_TID;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_RADIX_ALL_OK;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_JOIN_JOB_RADIX;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_KILL_JOB_RADIX;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_MAX;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_ERROR;
  im_request(&test_chan, IM_PROTOCOL_VER,&sockaddr);
  }
END_TEST

START_TEST(im_join_job_as_sister_test)
  {
  int result = 0;
  char *test_job_id = strdup("not_jobid");
  char *test_cookie = strdup("cookie");
  struct tcp_chan test_chan;
  struct sockaddr_in test_sock_addr;

  memset(&test_chan, 0, sizeof(test_chan));
  memset(&test_sock_addr, 0, sizeof(test_sock_addr));

  result = im_join_job_as_sister(&test_chan,
                                 test_job_id,
                                 &test_sock_addr,
                                 test_cookie,
                                 0,
                                 0,
                                 0,
                                 0);
  fail_unless(result==0, "im_join_job_as_sister_failed", result);
  }
END_TEST

START_TEST(tm_spawn_request_test)
  {
  struct tcp_chan test_chan;
  struct job test_job;
  struct hnodent test_hnodent;
  char *test_cookie = strdup("cookie");
  int reply = 0;
  int ret = 0;
  int result = 0;

  memset(&test_chan, 0, sizeof(test_chan));
  memset(&test_job, 0, sizeof(test_job));
  memset(&test_hnodent, 0, sizeof(test_hnodent));

  test_job.ji_vnods = (vnodent *)calloc(3, sizeof(vnodent));

  result = tm_spawn_request(&test_chan,
                            &test_job,
                            0,
                            0,
                            (char *)test_cookie,
                            &reply,
                            &ret,
                            0,
                            &test_hnodent,
                            0);

  fail_unless(result == TM_DONE, "tm_spawn_request fail: %d", result);
  }
END_TEST

START_TEST(pbs_task_create_test)
  {
  job *pjob = (job *)calloc(1, sizeof(job));

  /* Check ranning into reserved task IDs */
  pjob->ji_taskid = TM_ADOPTED_TASKID_BASE + 1;
  fail_unless(pbs_task_create(pjob, TM_NULL_TASK) == NULL, "Reserved task");

  /* Success */
  pjob->ji_taskid = TM_NULL_TASK;
  fail_unless(pbs_task_create(pjob, TM_NULL_TASK) != NULL);
  }
END_TEST

/*
 * void send_update_soon(void) - force the next status update sending by updating the last status
 * update sent timestamps.
 *
 * Input:
 *    (G) int ServerStatUpdateInterval - status update interval in seconds. > 0, default = 45
 *    (G) time_t time_now - current timestamp.
 *    (G) time_t LastServerUpdateTime - global last update timestamp. >= 0
 * Output:
 *    (G) time_t LastServerUpdateTime - global last update timestamp
 *    (G) time_t mom_servers[*].MOMLastSendToServerTime - per server last update timestamp
 */
void set_mom_last_send_to_server_time(time_t target) {
  for (int sindex = 0; sindex < PBS_MAXSERVER; sindex++)
    {
    mom_servers[sindex].MOMLastSendToServerTime = target;
    }
}

void check_mom_last_send_to_server_time(time_t expected) {
  for (int sindex = 0; sindex < PBS_MAXSERVER; sindex++)
    {
    fail_unless(mom_servers[sindex].MOMLastSendToServerTime == expected);
    }
}

START_TEST(send_update_soon_test)
  {
  ForceServerUpdate = false;
  send_update_soon();
  fail_unless(ForceServerUpdate);

  ForceServerUpdate = true;
  send_update_soon();
  fail_unless(ForceServerUpdate);
  }
END_TEST

START_TEST(get_stat_update_interval_test)
  {
  int interval;

  ServerStatUpdateInterval = 45;

  ForceServerUpdate = false;
  interval = get_stat_update_interval();
  fail_unless(interval == 45);

  ForceServerUpdate = true;
  interval = get_stat_update_interval();
  fail_unless(interval == 15);
  }
END_TEST

Suite *mom_comm_suite(void)
  {
  Suite *s = suite_create("mom_comm_suite methods");
  TCase *tc_core = tcase_create("mom_comm");

  tc_core = tcase_create("test_read_status_strings_null_chan_doesnt_crash");
  tcase_add_test(tc_core, test_read_status_strings_null_chan_doesnt_crash);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_read_status_strings_loop");
  tcase_add_test(tc_core, test_read_status_strings_loop);
  tcase_add_test(tc_core, test_process_end_job_error_reply);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("handle_im_obit_task_response_test");
  tcase_add_test(tc_core, handle_im_obit_task_response_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_get_received_node_entry");
  tcase_add_test(tc_core, test_get_received_node_entry);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("task_save_test");
  tcase_add_test(tc_core, task_save_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("im_request_test");
  tcase_add_test(tc_core, im_request_test);
  tcase_add_test(tc_core, create_contact_list_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("im_join_job_as_sister_test");
  tcase_add_test(tc_core, im_join_job_as_sister_test);
  tcase_add_test(tc_core, handle_im_poll_job_response_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("tm_spawn_request_test");
  tcase_add_test(tc_core, tm_spawn_request_test);
  tcase_add_test(tc_core, is_nodeid_on_this_host_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("pbs_task_create_test");
  tcase_add_test(tc_core, pbs_task_create_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("send_update_soon_test");
  tcase_add_test(tc_core, send_update_soon_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("get_stat_update_interval_test");
  tcase_add_test(tc_core, get_stat_update_interval_test);
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
  sr = srunner_create(mom_comm_suite());
  srunner_set_log(sr, "mom_comm_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
