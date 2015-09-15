#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <vector>

#include "alps_constants.h"
#include "alps_functions.h"
#include <string>
#include <check.h>

typedef std::vector<host_req *> host_req_list;


char *hostname = (char *)"napali";
char *eh1 = (char *)"napali/0-1+l11/0-1";
char *eh2 = (char *)"napali/0-1+l11/0-2";
char *eh3 = (char *)"napali/0-1+l11/0-1+lihue/0-2+waimea/0-2";
char *mpp = (char *)"waimea,fred,lihue,napali,l11";
char  buf[4096];
char *uname = (char *)"dbeer";
char *jobids[] = {(char *)"0.napali", (char *)"1.napali"} ;
char *apbasil_path = (char *)"/usr/local/bin/apbasil";
char *apbasil_protocol = (char *)"1.0";
char *apbasil_protocol_13 = (char *)"1.3";
char *apbasil_protocol_14 = (char *)"1.4";
char *blank_cmd = (char *)"../test_scripts/blank_script.sh";

char *alps_rsv_outputs[] = {
    (char *)"<?xml version='1.0'?><BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='RESERVE'><Reserved reservation_id='777' admin_cookie='0' alloc_cookie='0'/></ResponseData></BasilResponse>",
    (char *)"<?xml version='1.0'?><BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='RESERVE'><Reserved reservation_id='123' admin_cookie='0' alloc_cookie='0'/></ResponseData></BasilResponse>",
    (char *)"<?xml version='1.0'?><BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='RESERVE'><Reserved reservation_id='456' admin_cookie='0' alloc_cookie='0'/></ResponseData></BasilResponse>",
    (char *)"<?xml version='1.0'?><BasilResponse protocol='1.0'> <ResponseData status='FAILURE' method='RESERVE'/></BasilResponse>",
    (char *)"tom"};

host_req_list *parse_exec_hosts(char *exec_hosts,const char *mppnodes);
void             get_reservation_command(host_req_list *, char *, char *, char *, char *, char *, int, int,int, std::string&, std::string&);
int              parse_reservation_output(char *, char **);
int              execute_reservation(const char *, char **);
int              confirm_reservation(char *, char *, long long, char *, char *,char *,int);
int              parse_confirmation_output(char *);
void             adjust_for_depth(unsigned int &width, unsigned int &nppn, int depth);


START_TEST(adjust_for_depth_test)
  {
  unsigned int width = 24;
  unsigned int nppn  = 24;

  adjust_for_depth(width, nppn, 24);
  fail_unless(width == 1);
  fail_unless(nppn  == 1);

  width = 96;
  nppn  = 24;
  adjust_for_depth(width, nppn, 12);
  fail_unless(width == 8);
  fail_unless(nppn  == 2);

  adjust_for_depth(width, nppn, 0);
  fail_unless(width == 8);
  fail_unless(nppn  == 2);
  }
END_TEST



START_TEST(host_req_tests)
  {
  host_req *hr;

  hr = get_host_req(hostname, 1);

  fail_unless(!strcmp(hostname, hr->hostname), "set incorrect hostname");
  fail_unless(hr->ppn == 1, "set incorrect ppn");

  free_host_req(hr);
  }

END_TEST




START_TEST(parse_exec_hosts_test)
  {
  host_req_list *hrl = parse_exec_hosts(eh1,NULL);
  int              host_count = 0;
  host_req        *hr;

  for(host_req_list::iterator iter = hrl->begin();iter != hrl->end();iter++)
    {
    hr = *iter;
    snprintf(buf, sizeof(buf), "ppn should be %d but is %d", 2, hr->ppn);
    fail_unless(hr->ppn == 2, buf);
    host_count++;
    free(hr->hostname);
    }

  snprintf(buf, sizeof(buf), "Should be 2 hosts but there were %d", host_count);
  fail_unless(host_count == 2, buf);

  delete hrl;
  hrl = parse_exec_hosts(eh2,NULL);
  host_count = 0;

  for(host_req_list::iterator iter = hrl->begin();iter != hrl->end();iter++)
    {
    hr = *iter;
    if (host_count == 0)
      {
      snprintf(buf, sizeof(buf), "Hostname should be napali but is %s", hr->hostname);
      fail_unless(!strcmp(hr->hostname, "napali"), buf);

      snprintf(buf, sizeof(buf), "ppn should be 2 but is %d", hr->ppn);
      fail_unless(hr->ppn == 2, buf);
      }
    else
      {
      snprintf(buf, sizeof(buf), "Hostname should be l11 but is %s", hr->hostname);
      fail_unless(!strcmp(hr->hostname, "l11"), buf);

      snprintf(buf, sizeof(buf), "ppn should be 3 but is %d", hr->ppn);
      fail_unless(hr->ppn == 3, buf);
      }

    free(hr->hostname);
    host_count++;
    }

  snprintf(buf, sizeof(buf), "Should count 2 hosts but counted %d", host_count);
  fail_unless(host_count == 2, buf);

  delete hrl;
  hrl = parse_exec_hosts(eh3,NULL);
  host_count = 0;

  for(host_req_list::iterator iter = hrl->begin();iter != hrl->end();iter++)
    {
    hr = *iter;
    if (host_count < 2)
      {
      snprintf(buf, sizeof(buf), "ppn should be 2 but is %d", hr->ppn);
      fail_unless(hr->ppn == 2, buf);
      }
    else if (host_count >= 2)
      {
      snprintf(buf, sizeof(buf), "ppn should be 3 but is %d", hr->ppn);
      fail_unless(hr->ppn == 3, buf);
      }

    free(hr->hostname);
    host_count++;
    }

  snprintf(buf, sizeof(buf), "Should count 4 hosts but counted %d", host_count);
  fail_unless(host_count == 4, buf);

  delete hrl;

  hrl = parse_exec_hosts(eh3,mpp);
  hr = hrl->front();
  fail_unless(strcmp(hr->hostname,"waimea") == 0);
  hrl->erase(hrl->begin());
  hr = hrl->front();
  fail_unless(strcmp(hr->hostname,"lihue") == 0);
  hrl->erase(hrl->begin());
  hr = hrl->front();
  fail_unless(strcmp(hr->hostname,"napali") == 0);
  hrl->erase(hrl->begin());
  hr = hrl->front();
  fail_unless(strcmp(hr->hostname,"l11") == 0);
  hrl->erase(hrl->begin());
  fail_unless(hrl->size() == 0);
  delete hrl;
  }
END_TEST



START_TEST(get_reservation_command_test)
  {
  host_req_list   *hrl = parse_exec_hosts(eh1,NULL);
  std::string      apbasil_command = "";
  const char     *reserve_param;
  const char     *reserve_param2;
  const char     *nppn;
  int              ppn;
  int              nppcu_value;
  cpu_frequency_value freq;
  freq.frequency_type = P1;
  freq.mhz = 0;
  std::string      frequency = get_frequency_request(&freq);

  get_reservation_command(hrl, uname, jobids[0], NULL, apbasil_protocol, NULL, 0, 0, 0, apbasil_command,frequency);

  snprintf(buf, sizeof(buf), "Username '%s' not found in command '%s'", uname, apbasil_command.c_str());
  fail_unless(strstr(apbasil_command.c_str(), uname) != NULL, buf);

  reserve_param = strstr(apbasil_command.c_str(), "ReserveParam ");
  fail_unless(reserve_param != NULL, "Couldn't find a ReserveParam element in the request");
  reserve_param2 = strstr(reserve_param + 1, "ReserveParam ");
  snprintf(buf, sizeof(buf), 
    "Found two ReserveParam elements when there should be only one '%s'", apbasil_command.c_str());
  fail_unless(reserve_param2 == NULL, buf);

  delete hrl;

  hrl = parse_exec_hosts(eh3,NULL);
  apbasil_command.clear();
  frequency = get_frequency_request(&freq);
  get_reservation_command(hrl, uname, jobids[1], apbasil_path, apbasil_protocol, NULL, 1, 0, 0,apbasil_command,frequency);

  reserve_param = strstr(apbasil_command.c_str(), "ReserveParam ");
  reserve_param2 = strstr(reserve_param + 1, "ReserveParam ");
  fail_unless(reserve_param != NULL, "Couldn't find the first ReserveParam element in the request");

  nppn = strstr(reserve_param, "nppn");
  fail_unless(nppn != NULL, "Couldn't find the nppn specification in the first reservation");
  nppn += strlen("nppn='");
  ppn = atoi(nppn);
  snprintf(buf, sizeof(buf), "nppn should be 3 but is %d", ppn);
  fail_unless(ppn == 3, buf);

  delete hrl;
  apbasil_command.clear();
  frequency = get_frequency_request(&freq);

  hrl = parse_exec_hosts(eh3,NULL);
  get_reservation_command(hrl, uname, jobids[1], apbasil_path, apbasil_protocol_13, NULL, 1, 1, 0, apbasil_command,frequency);

  reserve_param = strstr(apbasil_command.c_str(), "ReserveParam ");
  reserve_param2 = strstr(reserve_param + 1, "ReserveParam ");
  fail_unless(reserve_param != NULL, "Couldn't find the first ReserveParam element in the request");

  const char *nppcu = strstr(reserve_param, "nppcu");
  fail_unless(nppcu != NULL, "Couldn't find the nppcu specification in the next reservation");
  nppcu += strlen("nppcu='");
  nppcu_value = atoi(nppcu);
  snprintf(buf, sizeof(buf), "nppcu should be 1 but is %d", nppcu_value);
  fail_unless(nppcu_value == 1, buf);
  const char *freq_request = strstr(apbasil_command.c_str()," p-state=");
  fail_unless(freq_request == NULL,"Frequency added to request.");

  delete hrl;
  apbasil_command.clear();
  frequency = get_frequency_request(&freq);

  hrl = parse_exec_hosts(eh3,NULL);
  get_reservation_command(hrl, uname, jobids[1], apbasil_path, apbasil_protocol_14, NULL, 1, 1, 0, apbasil_command,frequency);

  freq_request = strstr(apbasil_command.c_str()," p-state='1'");
  fail_unless(freq_request != NULL,"Frequency not added to request.");

  delete hrl;
  apbasil_command.clear();
  freq.frequency_type = Performance;
  frequency = get_frequency_request(&freq);

  hrl = parse_exec_hosts(eh3,NULL);
  get_reservation_command(hrl, uname, jobids[1], apbasil_path, apbasil_protocol_14, NULL, 1, 1, 0, apbasil_command,frequency);

  freq_request = strstr(apbasil_command.c_str()," p-governor='performance'");
  fail_unless(freq_request != NULL,"Frequency not added to request.");

  delete hrl;
  apbasil_command.clear();
  freq.frequency_type = AbsoluteFrequency;
  freq.mhz = 2200;
  frequency = get_frequency_request(&freq);

  hrl = parse_exec_hosts(eh3,NULL);
  get_reservation_command(hrl, uname, jobids[1], apbasil_path, apbasil_protocol_14, NULL, 1, 1, 0, apbasil_command,frequency);

  freq_request = strstr(apbasil_command.c_str()," p-state='2200000'");
  fail_unless(freq_request != NULL,"Frequency not added to request.");



  }
END_TEST


/*
START_TEST(parse_reservation_output_test)
  {
  char *rsv_id = NULL;
  int   rc;

  rc = parse_reservation_output(alps_rsv_outputs[0], &rsv_id);
  fail_unless(rc == 0, "error with first reservation output");
  snprintf(buf, sizeof(buf), "Reservation id should be 777 but is %s", rsv_id);
  fail_unless(!strcmp(rsv_id, "777"), buf);

  rc = parse_reservation_output(alps_rsv_outputs[1], &rsv_id);
  fail_unless(rc == 0, "error with first reservation output");
  snprintf(buf, sizeof(buf), "Reservation id should be 123 but is %s", rsv_id);
  fail_unless(!strcmp(rsv_id, "123"), buf);

  rc = parse_reservation_output(alps_rsv_outputs[2], &rsv_id);
  fail_unless(rc == 0, "error with first reservation output");
  snprintf(buf, sizeof(buf), "Reservation id should be 456 but is %s", rsv_id);
  fail_unless(!strcmp(rsv_id, "456"), buf);

  rc = parse_reservation_output(alps_rsv_outputs[3], &rsv_id);
  snprintf(buf, sizeof(buf), "Error code should be -1 but is %d", rc);
  fail_unless(rc == -1, buf);

  rc = parse_reservation_output(alps_rsv_outputs[4], &rsv_id);
  snprintf(buf, sizeof(buf), "Error code should be -3 but is %d", rc);
  fail_unless(rc == -3, buf);
  }
END_TEST*/




START_TEST(execute_reservation_test)
  {
  char *rsv_id;
  int   rc;
  int   rid = 30;
  char *cmd = (char *)"../test_scripts/execute_reservation.sh";
  char  cmdbuf[1024];

  snprintf(cmdbuf, sizeof(cmdbuf), "%s %d", cmd, rid);
  rc = execute_reservation(cmdbuf, &rsv_id);
  snprintf(buf, sizeof(buf), "Failed to execute the command '%s', rc=%d", cmdbuf, rc);
  fail_unless(rc == 0, buf);
  snprintf(buf, sizeof(buf), "Reservation id should be 30 but is '%s'", rsv_id);
  fail_unless(!strcmp(rsv_id, "30"), buf);

  rc = execute_reservation(blank_cmd, &rsv_id);
  fail_unless(rc == READING_PIPE_ERROR, "Somehow received bytes when we shouldn't have");

  rid = 919;
  snprintf(cmdbuf, sizeof(cmdbuf), "%s %d", cmd, rid);
  rc = execute_reservation(cmdbuf, &rsv_id);
  fail_unless(rc == 0, "Failed to execute the reservation");
  snprintf(buf, sizeof(buf), "Reservation id should be 919 but is '%s'", rsv_id);
  fail_unless(!strcmp(rsv_id, "919"), buf);
  }
END_TEST




START_TEST(confirm_reservation_test)
  {
  char      *rsv_id = (char *)"20";
  long long  pagg = 20;
  int        rc;
  char       cmdBuff[10240];

  /* this test only works if you're root */
  memset(cmdBuff,0,sizeof(cmdBuff));
  if (getuid() == 0)
    {
    rc = confirm_reservation(jobids[0], rsv_id, pagg, NULL, apbasil_protocol,cmdBuff,sizeof(cmdBuff));
    /*fail_unless(rc == 0, "Couldn't execute the reservation");*/
    snprintf(buf, sizeof(buf), "Reservation id should be 20 but was %s", rsv_id);
    fail_unless(!strcmp(rsv_id, "20"), buf);
    
    rc = confirm_reservation(jobids[1], rsv_id, pagg, blank_cmd, apbasil_protocol,cmdBuff,sizeof(cmdBuff));
    fail_unless(rc != 0, "Somehow parsed the blank command's output?");
    
    rc = parse_confirmation_output((char *)"tom");
    fail_unless(rc == ALPS_PARSING_ERROR, "We parsed non-xml?");
    }
  }
END_TEST



/*
START_TEST(create_alps_test)
  {
  char *rsv_id = strdup("120");
  char *rsv2 = strdup("121");
  int   rc = create_alps_reservation(eh3, uname, jobids[0], NULL, apbasil_protocol, 20, &rsv_id);

  fail_unless(rc == 0, "couldn't create reservation");

  rc = create_alps_reservation(eh2, uname, jobids[1], NULL, apbasil_protocol, 30, &rsv2);
  fail_unless(rc == 0, "couldn't create 2nd reservation");
  }
END_TEST*/




Suite *node_func_suite(void)
  {
  Suite *s = suite_create("alps helper suite methods");
  TCase *tc_core = tcase_create("host_req_tests");
  tcase_add_test(tc_core, host_req_tests);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("parse_exec_hosts_test");
  tcase_add_test(tc_core, parse_exec_hosts_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("get_reservation_command_test");
  tcase_add_test(tc_core, get_reservation_command_test);
  tcase_add_test(tc_core, adjust_for_depth_test);
  suite_add_tcase(s, tc_core);

  /*
  tc_core = tcase_create("parse_reservation_output_test");
  tcase_add_test(tc_core, parse_reservation_output_test);
  suite_add_tcase(s, tc_core);*/

  tc_core = tcase_create("execute_reservation_test");
  tcase_add_test(tc_core, execute_reservation_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("confirm_reservation_test");
  tcase_add_test(tc_core, confirm_reservation_test);
  suite_add_tcase(s, tc_core);

  /*
  tc_core = tcase_create("create_alps_test");
  tcase_add_test(tc_core, create_alps_test);
  suite_add_tcase(s, tc_core);*/
  
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
  sr = srunner_create(node_func_suite());
  srunner_set_log(sr, "node_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }

