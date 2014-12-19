#include "license_pbs.h" /* See here for the software license */
#include "svr_mail.h"
#include "test_svr_mail.h"
#include "pbs_error.h"
#include "pbs_job.h"
#include "svr_format_job.h"
#include "server.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

struct server server;

const char owner[] = "xowner";
const char jobid[] = "123";
const char jobname[] = "testJob";
const char errpath[] = "xyz_host.ac:/home/echan/work/dev/torque/trunk/STDIN.e123";
const char outpath[] = "xyz_host.ac:/home/echan/work/dev/torque/trunk/STDIN.o123";
const char mailbuf[] = "Exit_status=271";
const char msgbuf[]  = "Resources Used: One gallon of diesel; One bag Doritos; 2 liters Pepsi.";
const char complete[] = "e";

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
  rc = remove_old_mail("/tmp/mail.out");
  fail_unless((rc == 0), "unable to remove old mail output file");
  snprintf(correct_outfilepath, sizeof(correct_outfilepath), "Output_Path: %s", outpath);
  snprintf(correct_errfilepath, sizeof(correct_errfilepath), "Error_Path: %s", errpath);
  svr_mailowner(&pjob, MAIL_END, MAIL_NORMAL, mailbuf);
  sleep(1);
  fp = fopen("/tmp/mail.out", "r");
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
    remove_old_mail("/tmp/mail.out");
    }

  errFile_found = false;
  outFile_found = false;

  svr_mailowner_with_message(&pjob, MAIL_END, MAIL_NORMAL, mailbuf,msgbuf);
  sleep(1);
  fp = fopen("/tmp/mail.out", "r");
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
    remove_old_mail("/tmp/mail.out");
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

  rc = remove_old_mail("/tmp/mail.out");
  fail_unless((rc == 0), "unable to remove old mail output file");
  snprintf(correct_outfilepath, sizeof(correct_outfilepath), "Output_Path: %s", outpath);
  snprintf(correct_errfilepath, sizeof(correct_errfilepath), "Error_Path: %s", outpath);
  svr_mailowner(&pjob, MAIL_END, MAIL_NORMAL, mailbuf);
  sleep(1);
  fp = fopen("/tmp/mail.out", "r");
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
    remove_old_mail("/tmp/mail.out");
    }

  errFile_found = false;
  outFile_found = false;

  svr_mailowner_with_message(&pjob, MAIL_END, MAIL_NORMAL, mailbuf,msgbuf);
  sleep(1);
  fp = fopen("/tmp/mail.out", "r");
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
    remove_old_mail("/tmp/mail.out");
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

  rc = remove_old_mail("/tmp/mail.out");
  fail_unless((rc == 0), "unable to remove old mail output file");
  snprintf(correct_outfilepath, sizeof(correct_outfilepath), "Output_Path: %s", errpath);
  snprintf(correct_errfilepath, sizeof(correct_errfilepath), "Error_Path: %s", errpath);
  svr_mailowner(&pjob, MAIL_END, MAIL_NORMAL, mailbuf);
  sleep(1);
  fp = fopen("/tmp/mail.out", "r");
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
    remove_old_mail("/tmp/mail.out");
    }

  errFile_found = false;
  outFile_found = false;

  svr_mailowner_with_message(&pjob, MAIL_END, MAIL_NORMAL, mailbuf,msgbuf);
  sleep(1);
  fp = fopen("/tmp/mail.out", "r");
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
    remove_old_mail("/tmp/mail.out");
    }

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
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_with_eo_when_complete");
  tcase_add_test(tc_core, test_with_eo_when_complete);
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
