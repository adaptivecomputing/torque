#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "pbs_job.h"
#include "pbs_error.h"
#include "test_requests.h"

void string_replchar(const char*, char, char);
void determine_spooldir(std::string &spooldir, job *pjob);

extern char *TNoSpoolDirList[];
extern char *path_spool;
extern const char *wdir_ret;


START_TEST(test_determine_spooldir)
  {
  job          pjob;
  std::string  spooldir;
  const char  *val_ptr;

  memset(&pjob, 0, sizeof(job));
  pjob.ji_grpcache = (struct grpcache *)calloc(1, sizeof(struct grpcache));

  // If the spool directories don't match, we should get the default
  TNoSpoolDirList[0] = strdup("/home/bob/jobs/");
  wdir_ret = "PBS_O_WORKDIR=/home/dbeer/jobs/";
  val_ptr = strchr(wdir_ret, '=') + 1;

  determine_spooldir(spooldir, &pjob);
  fail_unless(spooldir == path_spool);
  
  TNoSpoolDirList[1] = strdup("/home/dbeer/jobs/");
  determine_spooldir(spooldir, &pjob);
  fail_unless(spooldir == val_ptr, "spool dir is %s, not %s", spooldir.c_str(), val_ptr);
  }
END_TEST


START_TEST(test_string_replchar)
  {
  char *mystring;

  mystring = (char *)calloc(1024, sizeof(char));
  fail_unless(NULL != mystring);

  // NULL should be handled without error
  string_replchar(NULL, 'a', 'b');

  // string empty
  string_replchar(mystring, 'a', 'b');
  fail_unless('\0' == *mystring);

  // simple replacement
  snprintf(mystring, 1024, "abcdefga");
  string_replchar(mystring, 'a', 'z');
  fail_unless(strcmp("zbcdefgz", mystring) == 0);

  // no replacement
  string_replchar(mystring, 'a', 'z');
  fail_unless(strcmp("zbcdefgz", mystring) == 0);
  }
END_TEST


Suite *requests_suite(void)
  {
  Suite *s = suite_create("requests_suite methods");
  TCase *tc_core = tcase_create("test_string_replchar");
  tcase_add_test(tc_core, test_string_replchar);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_determine_spooldir");
  tcase_add_test(tc_core, test_determine_spooldir);
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
  sr = srunner_create(requests_suite());
  srunner_set_log(sr, "requests_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
