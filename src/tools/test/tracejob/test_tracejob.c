#include "license_pbs.h" /* See here for the software license */
#include "tracejob.h"
#include "test_tracejob.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <zlib.h>
#include "pbs_error.h"


// test with uncompressed log
START_TEST(test_20161003)
  {
      struct tm *tm_ptr;
      time_t t;
      int file_count;
      char *filenames[MAX_LOG_FILES_PER_DAY];
      const char *prefix_path = ".";
      const char *jobid = "625";
      gzFile fp;
      struct log_array log = {};
      unsigned int i;

      // create tm struct
      t = time(NULL);
      tm_ptr = localtime(&t);
      ck_assert(tm_ptr != NULL);

      // set to 2016-10-03
      tm_ptr->tm_year = 2016 - 1900;
      tm_ptr->tm_mon = 10 - 1;
      tm_ptr->tm_mday = 3;

      file_count = log_path((char *)prefix_path, 1, tm_ptr, filenames);

      ck_assert_int_eq(file_count, 1);
      ck_assert_str_eq(filenames[0], "./server_logs/20161003");

      fp = gzopen(filenames[0], "r");
      ck_assert_msg(fp != NULL, "Failed to open ./server_logs/20161003 with gzopen");

      ck_assert_int_eq(parse_log(&fp, (char *)jobid, 1, &log), 0);

      gzclose(fp);
      free(filenames[0]);

      // check first and one to last line
      ck_assert_str_eq(log.log_lines[0].msg, "enqueuing into default, state 1 hop 1");
      ck_assert_str_eq(log.log_lines[0].type, "Job");
      ck_assert_str_eq(log.log_lines[0].date, "10/03/2016 13:43:35.194");
      ck_assert_str_eq(log.log_lines[0].obj, "PBS_Server.6728");

      i = log.ll_cur_amm-2;
      ck_assert_str_eq(log.log_lines[i].msg, "Exit_status=143 resources_used.cput=1 resources_used.energy_used=0 resources_used.mem=51996kb resources_used.vmem=38060kb resources_used.walltime=00:00:42");
      ck_assert_str_eq(log.log_lines[i].type, "Job");
      ck_assert_str_eq(log.log_lines[i].date, "10/03/2016 13:44:35.174");
      ck_assert_str_eq(log.log_lines[i].obj, "PBS_Server.6172");

      for (i = 0;i < log.ll_cur_amm;i++)
          free_log_entry(&log.log_lines[i]);
      free(log.log_lines);
  }
END_TEST

// test with gzip compressed log
START_TEST(test_20160928)
  {
      struct tm *tm_ptr;
      time_t t;
      int file_count;
      char *filenames[MAX_LOG_FILES_PER_DAY];
      const char *prefix_path = ".";
      const char *jobid = "624[4]";
      gzFile fp;
      struct log_array log = {};
      unsigned int i;

      // create tm struct
      t = time(NULL);
      tm_ptr = localtime(&t);
      ck_assert(tm_ptr != NULL);

      // set to 2016-09-28
      tm_ptr->tm_year = 2016 - 1900;
      tm_ptr->tm_mon = 9 - 1;
      tm_ptr->tm_mday = 28;

      file_count = log_path((char *)prefix_path, 1, tm_ptr, filenames);

      ck_assert_int_eq(file_count, 2);
      ck_assert_str_eq(filenames[0], "./server_logs/20160928");
      ck_assert_str_eq(filenames[1], "./server_logs/20160928.gz");

      fp = gzopen(filenames[1], "r");
      ck_assert_msg(fp != NULL, "Failed to open ./server_logs/20160928.gz with gzopen");

      ck_assert_int_eq(parse_log(&fp, (char *)jobid, 1, &log), 0);

      gzclose(fp);
      free(filenames[0]);
      free(filenames[1]);

      // check first and one to last line
      ck_assert_str_eq(log.log_lines[0].msg, "enqueuing into short, state 2 hop 1");
      ck_assert_str_eq(log.log_lines[0].type, "Job");
      ck_assert_str_eq(log.log_lines[0].date, "09/28/2016 10:11:21.389");
      ck_assert_str_eq(log.log_lines[0].obj, "PBS_Server.6173");

      i = log.ll_cur_amm-2;
      ck_assert_str_eq(log.log_lines[i].msg, "Exit_status=1 resources_used.cput=1 resources_used.energy_used=0 resources_used.mem=68752kb resources_used.vmem=38204kb resources_used.walltime=00:02:02");
      ck_assert_str_eq(log.log_lines[i].type, "Job");
      ck_assert_str_eq(log.log_lines[i].date, "09/28/2016 10:13:43.451");
      ck_assert_str_eq(log.log_lines[i].obj, "PBS_Server.6173");

      for (i = 0;i < log.ll_cur_amm;i++)
          free_log_entry(&log.log_lines[i]);
      free(log.log_lines);
  }
END_TEST

Suite *tracejob_suite(void)
  {
  Suite *s = suite_create("tracejob_suite methods");
  TCase *tc_core = tcase_create("test_20161003");
  tcase_add_test(tc_core, test_20161003);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_20160928");
  tcase_add_test(tc_core, test_20160928);
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
  sr = srunner_create(tracejob_suite());
  srunner_set_log(sr, "tracejob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
