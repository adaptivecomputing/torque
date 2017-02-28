#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "mail_throttler.hpp"
#include "test_svr_format_job.h"

void svr_format_job(FILE *fh, mail_info *mi, const char *fmt);

START_TEST(test_new_formats)
  {
  const char *path = "./new_formats.txt";
  const char *format = "My job is owned by %o in queue %q and it executed in working directory %w\n%R%u";
  const char *result = "My job is owned by dbeer@sel in queue batch and it executed in working \
directory /home/dbeer\nResources Requested Summary\n\twalltime=100\n\
\tnodes=2:ppn=8\nResources Used Summary\n\tcput=800\n\tvmem=8024mb\n\
\tmem=7000mb\n";
  mail_info mi;
  mi.owner = "dbeer@sel";
  mi.queue_name = "batch";
  mi.working_directory = "/home/dbeer";

  // These vectors have to be the correct size, but the actual information is put in through
  // the scaffolding
  std::vector<resource> requested;
  std::vector<resource> used;
  resource r;
  for (int i = 0; i < 2; i++)
    requested.push_back(r);
  for (int i = 0; i < 3; i++)
    used.push_back(r);
  
  mi.resources_requested = requested;
  mi.resources_used = used;
                        
  FILE *test_file = fopen(path, "w+");

  if (test_file != NULL)
    {
    char buf[4096];
    svr_format_job(test_file, &mi, format);
    fclose(test_file);

    test_file = fopen(path, "r");

    if (test_file != NULL)
      {
      char *ptr = buf;
      memset(ptr, 0, sizeof(buf));
      if (fread(ptr, 1, sizeof(buf) - 1, test_file) > 0)
        {
        fail_unless((!strcmp(buf, result)), "Expected:\n'%s'\nGot:\n'%s'", result, buf);
        }
      }

    unlink(path);
    }

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *svr_format_job_suite(void)
  {
  Suite *s = suite_create("svr_format_job_suite methods");
  TCase *tc_core = tcase_create("test_new_formats");
  tcase_add_test(tc_core, test_new_formats);
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
  sr = srunner_create(svr_format_job_suite());
  srunner_set_log(sr, "svr_format_job_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
