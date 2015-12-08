#include "license_pbs.h" /* See here for the software license */

#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "issue_request.h"
#include "test_uut.h"
#include "pbs_error.h"
#include "attribute.h"

bool return_addr;
bool local_connect;
bool net_rc_retry;
bool connect_error;

extern std::string rq_id_str;
void queue_a_retry_task(batch_request *preq, void (*replyfunc)(struct work_task *));
int send_request_to_remote_server(int conn, batch_request *request, bool close_handle);
int issue_Drequest(int conn, batch_request *request, bool close_handle);


START_TEST(queue_a_retry_task_test)
  {
  batch_request *preq = (batch_request *)calloc(1, sizeof(batch_request));
  preq->rq_id = strdup("tom");

  queue_a_retry_task(preq, NULL);
  // rq_id_str is a sensing variable set in set_task()
  // this tells us we made a deep copy because we acquired a new rq_id for the 
  // request that we are reissuing.
  fail_unless(rq_id_str != "tom");
  }
END_TEST


START_TEST(test_one)
  {
  job testJob;
  job *pTestJob;
  struct batch_request request;

  memset(&testJob,0,sizeof(job));
  memset(&request,0,sizeof(request));
  pTestJob = &testJob;

  local_connect = true;
  net_rc_retry = false; /* Keep this false for svr_connect except for one case with issue_to_svr */
  fail_unless(relay_to_mom(&pTestJob,&request,NULL) == PBSE_BADSTATE);

  decode_str(&testJob.ji_wattr[JOB_ATR_exec_host],NULL,NULL,"Sherrie",0);

  fail_unless(relay_to_mom(&pTestJob,&request,NULL) == PBSE_NONE);

  }
END_TEST

START_TEST(test_send_request_to_remote_server)
  {
  int rc;
  batch_request *preq;

  preq = alloc_br(PBS_BATCH_TrackJob);
  fail_unless(preq != NULL);
  preq->rq_ind.rq_track.rq_hopcount = 1;
  strcpy(preq->rq_ind.rq_track.rq_jid, "1234");
  strcpy(preq->rq_ind.rq_track.rq_location, "hosta");
  preq->rq_ind.rq_track.rq_state[0] = 'Q';
  preq->rq_fromsvr = 1;
  preq->rq_perm = ATR_DFLAG_MGRD | ATR_DFLAG_MGWR | ATR_DFLAG_SvWR;
  
  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);

  free(preq);

  preq = alloc_br(PBS_BATCH_DeleteJob);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_track.rq_jid, "1234");
  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);
  
  free(preq);
  preq = alloc_br(PBS_BATCH_CheckpointJob);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_hold.rq_orig.rq_objname, "some_object");
  rc = send_request_to_remote_server(10, preq, true);

  free(preq);
  preq = alloc_br(PBS_BATCH_GpuCtrl);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_gpuctrl.rq_momnode, "nodea");
  strcpy(preq->rq_ind.rq_gpuctrl.rq_gpuid, "0");
  preq->rq_ind.rq_gpuctrl.rq_gpumode = 1;
  preq->rq_ind.rq_gpuctrl.rq_reset_perm = 1;
  preq->rq_ind.rq_gpuctrl.rq_reset_vol = 1;

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);

  free(preq);
  preq = alloc_br(PBS_BATCH_MessJob);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_message.rq_jid, "1234");
  preq->rq_ind.rq_message.rq_file = 1;
  preq->rq_ind.rq_message.rq_text = NULL;

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);


  free(preq);
  preq = alloc_br(PBS_BATCH_Rerun);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_rerun, "1234");

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);

  free(preq);
  preq = alloc_br(PBS_BATCH_RegistDep);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_rerun, "1234");

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);

  free(preq);
  preq = alloc_br(PBS_BATCH_AsySignalJob);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_signal.rq_jid, "1234");
  strcpy(preq->rq_ind.rq_signal.rq_signame, "SIGKILL");
  preq->rq_extra = NULL;

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);

  free(preq);
  preq = alloc_br(PBS_BATCH_SignalJob);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_signal.rq_jid, "1234");
  strcpy(preq->rq_ind.rq_signal.rq_signame, "SIGKILL");
  preq->rq_extra = NULL;

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);

  free(preq);
  preq = alloc_br(PBS_BATCH_StatusJob);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_status.rq_id, "1234");

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);

  free(preq);
  preq = alloc_br(PBS_BATCH_ReturnFiles);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_status.rq_id, "1234");

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);

  free(preq);
  preq = alloc_br(PBS_BATCH_CopyFiles);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_status.rq_id, "1234");

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);

  free(preq);
  preq = alloc_br(PBS_BATCH_DelFiles);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_status.rq_id, "1234");

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);

  free(preq);
  preq = alloc_br(PBS_BATCH_DeleteReservation);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_status.rq_id, "1234");

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc == PBSE_NONE);

  free(preq);
  preq = alloc_br(PBS_BATCH_GAP038);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_status.rq_id, "1234");

  rc = send_request_to_remote_server(10, preq, true);
  fail_unless(rc != PBSE_NONE);




  }
END_TEST

START_TEST(test_handle_local_request)
  {
  int rc;
  struct batch_request *preq;

  preq = alloc_br(PBS_BATCH_StatusJob);
  fail_unless(preq != NULL);

  rc = handle_local_request(10, preq);
  fail_unless(rc == PBSE_NONE);
  free(preq);

  }
END_TEST

START_TEST(test_issue_Drequest)
  {
  int rc;
  struct batch_request *preq;

  preq = alloc_br(PBS_BATCH_StatusJob);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_status.rq_id, "1234");

  rc = issue_Drequest(10, preq, true);
  fail_unless(rc == PBSE_NONE);


  }
END_TEST


START_TEST(test_issue_to_svr)
  {
  int rc;
  char server_name[PBS_MAXHOSTNAME];
  struct batch_request *preq;

  preq = alloc_br(PBS_BATCH_StatusJob);
  fail_unless(preq != NULL);
  strcpy(preq->rq_ind.rq_status.rq_id, "1234");
  strcpy(server_name, "hosta");

  return_addr = true;
  local_connect = true;
  rc = issue_to_svr(server_name, &preq, NULL);
  fail_unless(rc==PBSE_NONE);
  // local connect should cause preq to be set to NULL
  fail_unless(preq == NULL);

  return_addr = false;
  preq = alloc_br(PBS_BATCH_StatusJob);
  rc = issue_to_svr(server_name, &preq, NULL);
  fail_unless(rc==PBSE_NONE);

  return_addr = true;
  local_connect = false;
  net_rc_retry = false;
  rc = issue_to_svr(server_name, &preq, NULL);
  fail_unless(rc==PBSE_NONE);

  net_rc_retry = true;
  rc = issue_to_svr(server_name, &preq, NULL);
  fail_unless(rc==PBSE_NONE);

  local_connect = false;
  net_rc_retry = false;
  connect_error = true;
  rc = issue_to_svr(server_name, &preq, NULL);
  fail_unless(rc==PBSE_INTERNAL);

  }
END_TEST

START_TEST(test_reissue_to_svr);
  {
  struct batch_request *preq;
  struct work_task *pwt;

  /* Test for NULL work task */
  pwt = NULL;
  reissue_to_svr(pwt);

  /* Schedule a task. Success */
  preq = alloc_br(PBS_BATCH_TrackJob);
  fail_unless(preq != NULL);
  preq->rq_ind.rq_track.rq_hopcount = 1;
  strcpy(preq->rq_ind.rq_track.rq_jid, "1234");
  strcpy(preq->rq_ind.rq_track.rq_location, "hosta");
  preq->rq_ind.rq_track.rq_state[0] = 'Q';
  preq->rq_fromsvr = 1;
  strcpy(preq->rq_host, "hosta");
  preq->rq_perm = ATR_DFLAG_MGRD | ATR_DFLAG_MGWR | ATR_DFLAG_SvWR;
  get_batch_request_id(preq);

  pwt = set_task(WORK_Timed, (time(NULL) + PBS_NET_RETRY_TIME), reissue_to_svr, preq->rq_id, TRUE);
  fail_unless(pwt != NULL);
  reissue_to_svr(pwt);

 
  }
END_TEST

Suite *issue_request_suite(void)
  {
  Suite *s = suite_create("issue_request_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_send_request_to_remote_server");
  tcase_add_test(tc_core, test_send_request_to_remote_server);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_handle_local_request");
  tcase_add_test(tc_core, test_handle_local_request);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_issue_Drequest");
  tcase_add_test(tc_core, test_issue_Drequest);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_issue_to_svr");
  tcase_add_test(tc_core, test_issue_to_svr);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_reissue_to_svr");
  tcase_add_test(tc_core, test_reissue_to_svr);
  tcase_add_test(tc_core, queue_a_retry_task_test);
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
  sr = srunner_create(issue_request_suite());
  srunner_set_log(sr, "issue_request_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
