#include "license_pbs.h" /* See here for the software license */
#include "svr_mail.h"
#include "test_svr_mail.h"
#include "pbs_error.h"
#include "pbs_job.h"
#include "svr_format_job.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


const char owner[] = "xowner";
const char jobid[] = "123";
const char jobname[] = "testJob";
const char errpath[] = "xyz_host.ac:/home/echan/work/dev/torque/trunk/STDIN.e123";
const char outpath[] = "xyz_host.ac:/home/echan/work/dev/torque/trunk/STDIN.o123";
const char mailbuf[] = "Exit_status=271";
const char complete[] = "e";

void setup_job(job *pjob)
  {
  memset(pjob, 0, sizeof(job));
  strcpy(pjob->ji_qs.ji_jobid, jobid);
  pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str = (char *)owner;
  pjob->ji_wattr[JOB_ATR_jobname].at_val.at_str = (char *)jobname;
  pjob->ji_wattr[JOB_ATR_errpath].at_val.at_str = (char *)errpath;
  pjob->ji_wattr[JOB_ATR_errpath].at_val.at_str = (char *)outpath;
  pjob->ji_wattr[JOB_ATR_mailpnts].at_flags |= ATR_VFLAG_SET;
  pjob->ji_wattr[JOB_ATR_mailpnts].at_val.at_str = (char *)complete;
  }

START_TEST(test_email_containing_file_paths_when_complete)
  {
  job pjob;
  FILE *fp = NULL;
  bool errFile_found = false;
  bool outFile_found = false;
  char buf[512];

  setup_job(&pjob);
  svr_mailowner(&pjob, MAIL_END, MAIL_NORMAL, mailbuf);
  fp = fopen("/tmp/mail.out", "r");
  /* if fp is null, no email program was set hence no test */
  if (fp)
    {
    while (fgets(buf, sizeof(buf), fp) != NULL)
      {
      if (strstr(buf, "Error_Path: "))
        errFile_found = true;
      else if (strstr(buf, "Output_Path: "))
        outFile_found = true;
      }
    fail_unless(errFile_found, "No error file path was found in the mail");
    fail_unless(outFile_found, "No output file path was found in the mail");
    fclose(fp);
    }
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *svr_mail_suite(void)
  {
  Suite *s = suite_create("svr_mail_suite methods");
  TCase *tc_core = tcase_create("test_email_containing_file_paths_when_complete");
  tcase_add_test(tc_core, test_email_containing_file_paths_when_complete);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
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
  sr = srunner_create(svr_mail_suite());
  srunner_set_log(sr, "svr_mail_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
