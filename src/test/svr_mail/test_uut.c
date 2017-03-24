#include "license_pbs.h" /* See here for the software license */
#include "pbs_error.h"
#include "pbs_job.h"
#include "server.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "test_svr_mail.h"
#include "mail_throttler.hpp"
#include "work_task.h"
#include "server.h"



struct server server;

const char owner[] = "xowner";
const char jobid[] = "123";
const char jobname[] = "testJob";
const char errpath[] = "xyz_host.ac:/home/echan/work/dev/torque/trunk/STDIN.e123";
const char outpath[] = "xyz_host.ac:/home/echan/work/dev/torque/trunk/STDIN.o123";
const char mailbuf[] = "Exit_status=271";
const char msgbuf[]  = "Resources Used: One gallon of diesel; One bag Doritos; 2 liters Pepsi.";
const char complete[] = "e";
extern int called;


void send_email_batch(struct work_task *pwt);
int get_sendmail_args(char **, mail_info *, char **);
extern mail_throttler pending_emails;
extern bool empty_body;
extern bool get_svr_attr_str_return_svr_sm_attr;

void init_server()
  {
  server.sv_attr_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(server.sv_attr_mutex, NULL);
  }

void setup_job(job *pjob)
  {
  memset(pjob, 0, sizeof(job));
  strcpy(pjob->ji_qs.ji_jobid, jobid);
  pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str = (char *)owner;
  pjob->ji_wattr[JOB_ATR_jobname].at_val.at_str = (char *)jobname;
  pjob->ji_wattr[JOB_ATR_errpath].at_val.at_str = (char *)errpath;
  pjob->ji_wattr[JOB_ATR_outpath].at_val.at_str = (char *)outpath;
  pjob->ji_wattr[JOB_ATR_mailpnts].at_flags |= ATR_VFLAG_SET;
  pjob->ji_wattr[JOB_ATR_mailpnts].at_val.at_str = (char *)complete;
  
  }

int remove_old_mail(const char *filename)
  {
  if ((unlink(filename) != 0) && (errno != ENOENT))
    return -1;

  return 0;
  }

void read_file_into_string(

  const char  *filename,
  std::string &output)

  {
  FILE *fp = fopen(filename, "r");
  char  buf[1024];

  memset(&buf, 0, sizeof(buf));
  output.clear();

  if (fp == NULL)
    {
    sleep(1);
    fp = fopen(filename, "r");
    }
  
  if (fp != NULL)
    {
    while (fgets(buf, sizeof(buf) - 1, fp) != NULL)
      {
      output += buf;
      }
  
    fclose(fp);
    }
  }
  

START_TEST(test_send_email_batch)
  {
  mail_info    mi;
  char         buf[1024];
  const char  *addressee = "dbeer@adaptivecomputing.com";
  const char  *filename = "./output.dbeer@adaptivecomputing.com";
  std::string  output;
  work_task   *pwt = (work_task *)calloc(1, sizeof(work_task));
  pwt->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pwt->wt_parm1 = strdup(addressee);

  mi.mailto = addressee;

  for (int i = 0; i < 10; i++)
    {
    sprintf(buf, "%d.napali", i);
    mi.jobid = buf;
    pending_emails.add_email_entry(mi);
    }
  
  remove_old_mail(filename);

  // Make svr_format_job() do nothing
  empty_body = true;
  send_email_batch(pwt);
  read_file_into_string(filename, output);
  empty_body = false;

  // Check the file for the header "Job '<jobid>'" for each job
  for (int i = 0; i < 10; i++)
    {
    sprintf(buf, "Job '%d.napali'", i);
    fail_unless(output.find(buf) != std::string::npos, "output contains: '%s'", output.c_str());
    }

  fail_unless(output.find("Subject: Summary Email for 10 Torque Jobs") != std::string::npos);
  
  remove_old_mail(filename);
  }
END_TEST


START_TEST(test_with_default_files_when_complete)
  {
  job pjob;
  FILE *fp = NULL;
  bool errFile_found = false;
  bool outFile_found = false;
  bool msg_found = false;
  char buf[512];
  char correct_outfilepath[512];
  char correct_errfilepath[512];
  int rc = 0;

  setup_job(&pjob);
  rc = remove_old_mail("./output.xowner");
  fail_unless((rc == 0), "unable to remove old mail output file");
  snprintf(correct_outfilepath, sizeof(correct_outfilepath), "Output_Path: %s", outpath);
  snprintf(correct_errfilepath, sizeof(correct_errfilepath), "Error_Path: %s", errpath);
  svr_mailowner(&pjob, MAIL_END, MAIL_NORMAL, mailbuf);
  sleep(1);
  fp = fopen("./output.xowner", "r");
  fail_unless((fp != NULL), "No output file was found");
  if (fp)
    {
    while (fgets(buf, sizeof(buf), fp) != NULL)
      {
      if (strstr(buf, correct_errfilepath))
        errFile_found = true;
      else if (strstr(buf, correct_outfilepath))
        outFile_found = true;
      }
    fail_unless(errFile_found, "No error file path was found in the mail");
    fail_unless(outFile_found, "No output file path was found in the mail");
    fclose(fp);
    remove_old_mail("./output.xowner");
    }

  errFile_found = false;
  outFile_found = false;

  svr_mailowner_with_message(&pjob, MAIL_END, MAIL_NORMAL, mailbuf,msgbuf);
  sleep(1);
  fp = fopen("./output.xowner", "r");
  fail_unless((fp != NULL), "No output file was found");
  if (fp)
    {
    while (fgets(buf, sizeof(buf), fp) != NULL)
      {
      if (strstr(buf, correct_errfilepath))
        errFile_found = true;
      else if (strstr(buf, correct_outfilepath))
        outFile_found = true;
      else if (strstr(buf,msgbuf))
        msg_found = true;
      }
    fail_unless(errFile_found, "No error file path was found in the mail");
    fail_unless(outFile_found, "No output file path was found in the mail");
    fail_unless(msg_found,"The additional message was not found in the mail.");
    fclose(fp);
    remove_old_mail("./output.xowner");
    }
  }
END_TEST

START_TEST(test_with_oe_when_complete)
  {
  job pjob;
  FILE *fp = NULL;
  bool errFile_found = false;
  bool outFile_found = false;
  bool msg_found = false;
  char buf[512];
  char correct_outfilepath[512];
  char correct_errfilepath[512];
  int rc = 0;
  const char attr_join_oe[] = "oe";

  setup_job(&pjob);
  pjob.ji_wattr[JOB_ATR_join].at_flags |= ATR_VFLAG_SET;
  pjob.ji_wattr[JOB_ATR_join].at_val.at_str = (char *)attr_join_oe;

  rc = remove_old_mail("./output.xowner");
  fail_unless((rc == 0), "unable to remove old mail output file");
  snprintf(correct_outfilepath, sizeof(correct_outfilepath), "Output_Path: %s", outpath);
  snprintf(correct_errfilepath, sizeof(correct_errfilepath), "Error_Path: %s", outpath);
  svr_mailowner(&pjob, MAIL_END, MAIL_NORMAL, mailbuf);
  sleep(1);
  fp = fopen("./output.xowner", "r");
  fail_unless((fp != NULL), "No output file was found");
  if (fp)
    {
    while (fgets(buf, sizeof(buf), fp) != NULL)
      {
      if (strstr(buf, correct_errfilepath))
        errFile_found = true;
      else if (strstr(buf, correct_outfilepath))
        outFile_found = true;
      }
    fail_unless(errFile_found, "No error file path was found in the mail");
    fail_unless(outFile_found, "No output file path was found in the mail");
    fclose(fp);
    remove_old_mail("./output.xowner");
    }

  errFile_found = false;
  outFile_found = false;

  svr_mailowner_with_message(&pjob, MAIL_END, MAIL_NORMAL, mailbuf,msgbuf);
  sleep(1);
  fp = fopen("./output.xowner", "r");
  fail_unless((fp != NULL), "No output file was found");
  if (fp)
    {
    while (fgets(buf, sizeof(buf), fp) != NULL)
      {
      if (strstr(buf, correct_errfilepath))
        errFile_found = true;
      else if (strstr(buf, correct_outfilepath))
        outFile_found = true;
      else if (strstr(buf,msgbuf))
        msg_found = true;
      }
    fail_unless(errFile_found, "No error file path was found in the mail");
    fail_unless(outFile_found, "No output file path was found in the mail");
    fail_unless(msg_found,"The additional message was not found in the mail.");
    fclose(fp);
    remove_old_mail("./output.xowner");
    }

  }
END_TEST

START_TEST(test_with_eo_when_complete)
  {
  job pjob;
  FILE *fp = NULL;
  bool errFile_found = false;
  bool outFile_found = false;
  bool msg_found = false;
  char buf[512];
  char correct_outfilepath[512];
  char correct_errfilepath[512];
  int rc = 0;
  const char attr_join_oe[] = "eo";

  setup_job(&pjob);
  pjob.ji_wattr[JOB_ATR_join].at_flags |= ATR_VFLAG_SET;
  pjob.ji_wattr[JOB_ATR_join].at_val.at_str = (char *)attr_join_oe;

  rc = remove_old_mail("./output.xowner");
  fail_unless((rc == 0), "unable to remove old mail output file");
  snprintf(correct_outfilepath, sizeof(correct_outfilepath), "Output_Path: %s", errpath);
  snprintf(correct_errfilepath, sizeof(correct_errfilepath), "Error_Path: %s", errpath);
  svr_mailowner(&pjob, MAIL_END, MAIL_NORMAL, mailbuf);
  sleep(1);
  fp = fopen("./output.xowner", "r");
  fail_unless((fp != NULL), "No output file was found");
  if (fp)
    {
    while (fgets(buf, sizeof(buf), fp) != NULL)
      {
      if (strstr(buf, correct_errfilepath))
        errFile_found = true;
      else if (strstr(buf, correct_outfilepath))
        outFile_found = true;
      }
    fail_unless(errFile_found, "No error file path was found in the mail");
    fail_unless(outFile_found, "No output file path was found in the mail", buf);
    fclose(fp);
    remove_old_mail("./output.xowner");
    }

  errFile_found = false;
  outFile_found = false;

  svr_mailowner_with_message(&pjob, MAIL_END, MAIL_NORMAL, mailbuf,msgbuf);
  sleep(1);
  fp = fopen("./output.xowner", "r");
  fail_unless((fp != NULL), "No output file was found");
  if (fp)
    {
    while (fgets(buf, sizeof(buf), fp) != NULL)
      {
      if (strstr(buf, correct_errfilepath))
        errFile_found = true;
      else if (strstr(buf, correct_outfilepath))
        outFile_found = true;
      else if (strstr(buf,msgbuf))
        msg_found = true;
      }
    fail_unless(errFile_found, "No error file path was found in the mail");
    fail_unless(outFile_found, "No output file path was found in the mail");
    fail_unless(msg_found,"The additional message was not found in the mail.");
    fclose(fp);
    remove_old_mail("./output.xowner");
    }

  }
END_TEST

START_TEST(mail_point_p)
  {	
  called = 0;
  job pjob;	
   
  char p[]= "p";
  pjob.ji_wattr[JOB_ATR_mailpnts].at_val.at_str = p;	  
  svr_mailowner(&pjob, 1, 1, p);
  fail_unless((called == 0),"one");

  
  char a[]= "a";
  setup_job(&pjob);
  pjob.ji_wattr[JOB_ATR_mailpnts].at_val.at_str = a;	  
  svr_mailowner(&pjob, 1, 1, p);
  fail_unless((called == 1),"two");
  }
END_TEST

START_TEST(test_get_sendmail_args)
  {
  char *sendmail_args[100];
  mail_info mi;
  char *p;

  get_sendmail_args(sendmail_args, &mi, (char **)&p);

  // expect value compiled in
  fail_unless(strcmp(sendmail_args[0], UT_SENDMAIL_CMD) == 0);

  // now set the server attribute
  server.sv_attr[SRV_ATR_SendmailPath].at_val.at_str = (char *)"./foo";

  // expect to match server attribute (not one compiled in)
  get_svr_attr_str_return_svr_sm_attr = true;
  get_sendmail_args(sendmail_args, &mi, (char **)&p);
  fail_unless(strcmp(sendmail_args[0], "./foo") == 0);
  }
END_TEST

Suite *svr_mail_suite(void)
  {
  Suite *s = suite_create("svr_mail_suite methods");
  TCase *tc_core = tcase_create("test_with_default_files_when_complete");
  tcase_add_test(tc_core, test_with_default_files_when_complete);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_with_oe_when_complete");
  tcase_add_test(tc_core, test_with_oe_when_complete);
  tcase_add_test(tc_core, test_send_email_batch);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_with_eo_when_complete");
  tcase_add_test(tc_core, test_with_eo_when_complete);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("mail_point_p");
  tcase_add_test(tc_core, mail_point_p);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_get_sendmail_args");
  tcase_add_test(tc_core, test_get_sendmail_args);
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
  init_server();
  rundebug();
  sr = srunner_create(svr_mail_suite());
  srunner_set_log(sr, "svr_mail_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
