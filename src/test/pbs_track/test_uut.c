#include "license_pbs.h" /* See here for the software license */
#include "test_pbs_track.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include "tm.h"


#include "pbs_error.h"

int adopt_process(char* JobID, const std::string &tmpAdopteeID);
int parse_commandline_opts(int argc, char **argv, std::string &tmpAdopteeID, std::string &tmpJobID, int &DoBackground);
int fork_process(int argc, char **argv, int DoBackground, int &pid, char *JobID, char **Args);
int handle_adoption_results(int rc, int DoBackground, int this_pid, char *JobID, std::string tmpAdopteeID, char **Args);


void reset(std::string &tmpAdopteeID, int &DoBackground)
  {
  tmpAdopteeID.clear();

  tmpAdopteeID = std::string();
  DoBackground = 0;
  optind = 1;
  }

START_TEST(parse_commandline_opts)
  {
  std::string tmpAdopteeID;
  std::string tmpJobID;
  int         DoBackground = 0;
  int         rc;
  int         argc = 1;
  char       *argv[10];

  tmpJobID[0] = '\0';

  for(int i = 0; i < 10; i++)
      argv[i] = NULL;

  reset(tmpAdopteeID, DoBackground);
  rc = parse_commandline_opts(argc, argv, tmpAdopteeID, tmpJobID, DoBackground);
  fail_unless(rc == 2, "parse_commandline_opts failed with no options ", rc);

  argv[1] = (char *)malloc(10);
  strcpy(argv[1], "-j");
  argc = 2;
  reset(tmpAdopteeID, DoBackground);
  rc = parse_commandline_opts(argc, argv, tmpAdopteeID, tmpJobID, DoBackground);
  fail_unless(0 == PBSE_NONE, "parse_commandline_opts failed with only -j option  ", rc);

  strcpy(argv[1], "-a");
  argc = 2;
  reset(tmpAdopteeID, DoBackground);
  rc = parse_commandline_opts(argc, argv, tmpAdopteeID, tmpJobID, DoBackground);
  fail_unless(rc == 2, "parse_commandline_opts failed with only -a option  ", rc);

  argv[2] = (char *)malloc(10);
  strcpy(argv[1], "-j");
  strcpy(argv[2], "28");
  argc = 3;
  reset(tmpAdopteeID, DoBackground);
  rc = parse_commandline_opts(argc, argv, tmpAdopteeID, tmpJobID, DoBackground);
  fail_unless(rc != PBSE_NONE, "parse_commandline_opts failed with -j option and job ", rc);

  argv[3] = (char *)malloc(10);
  strcpy(argv[1], "-j");
  strcpy(argv[2], "28");
  strcpy(argv[3], "sleep");
  argc = 4;
  reset(tmpAdopteeID, DoBackground);
  rc = parse_commandline_opts(argc, argv, tmpAdopteeID, tmpJobID, DoBackground);
  fail_unless(rc == PBSE_NONE, "parse_commandline_opts failed with -j option and job ", rc);

  strcpy(argv[1], "-j");
  strcpy(argv[2], "28");
  strcpy(argv[3], "-b");
  argc = 4;
  reset(tmpAdopteeID, DoBackground);
  rc = parse_commandline_opts(argc, argv, tmpAdopteeID, tmpJobID, DoBackground);
  fail_unless(rc != PBSE_NONE, "parse_commandline_opts failed with -j and -b options and job ", rc);

  argv[4] = (char *)malloc(10);
  strcpy(argv[1], "-j");
  strcpy(argv[2], "28");
  strcpy(argv[3], "-b");
  strcpy(argv[4], "sleep");
  argc = 5;
  reset(tmpAdopteeID, DoBackground);
  rc = parse_commandline_opts(argc, argv, tmpAdopteeID, tmpJobID, DoBackground);
  fail_unless(rc == PBSE_NONE, "parse_commandline_opts failed with -j and -b options and job ", rc);

  strcpy(argv[1], "-j");
  strcpy(argv[2], "28");
  strcpy(argv[3], "-a");
  strcpy(argv[4], "aefaef");
  argc = 5;
  reset(tmpAdopteeID, DoBackground);
  rc = parse_commandline_opts(argc, argv, tmpAdopteeID, tmpJobID, DoBackground);
  fail_unless(rc == PBSE_NONE, "parse_commandline_opts failed with -j and -a options and job and pid %d", rc);

  argv[5] = (char *)malloc(10);
  strcpy(argv[1], "-j");
  strcpy(argv[2], "28");
  strcpy(argv[3], "-b");
  strcpy(argv[4], "-a");
  strcpy(argv[5], "aefaef");
  argc = 6;
  reset(tmpAdopteeID, DoBackground);
  rc = parse_commandline_opts(argc, argv, tmpAdopteeID, tmpJobID, DoBackground);
  fail_unless(rc == 2, "parse_commandline_opts failed with -j, -a and -b options and job and pid ", rc);

  strcpy(argv[1], "-j");
  strcpy(argv[2], "28");
  strcpy(argv[3], "-a");
  strcpy(argv[4], "aefaef");
  strcpy(argv[5], "-b");
  argc = 6;
  reset(tmpAdopteeID, DoBackground);
  rc = parse_commandline_opts(argc, argv, tmpAdopteeID, tmpJobID, DoBackground);
  fail_unless(rc == 2, "parse_commandline_opts failed with -j, -a and -b options and job and pid ", rc);
  }
END_TEST

START_TEST(handle_adoption_results)
  {
  int          rc;
  int          DoBackground;
  int          pid;
  char       * JobID;
  std::string  tmpAdopteeID;
  char        *Args[5];

  rc = TM_SUCCESS;
  DoBackground = 0;
  pid = 0;
  JobID = strdup("10.localhost");
  tmpAdopteeID = "1000";

  rc = handle_adoption_results(rc, DoBackground, pid, JobID, tmpAdopteeID, Args);
  fail_unless(rc == PBSE_NONE, "handle_adopt_results failed with TM_SUCCESS ", rc);

  rc = TM_ENOTFOUND;
  rc = handle_adoption_results(rc, DoBackground, pid, JobID, tmpAdopteeID, Args);
  fail_unless(rc == -1, "handle_adopt_results failed with TM_ENOTFOUND ", rc);

  rc = TM_ENOTCONNECTED;
  rc = handle_adoption_results(rc, DoBackground, pid, JobID, tmpAdopteeID, Args);
  fail_unless(rc == -1, "handle_adopt_results failed with TM_ENOTCONNECTED ", rc);

  rc = TM_EPERM;
  rc = handle_adoption_results(rc, DoBackground, pid, JobID, tmpAdopteeID, Args);
  fail_unless(rc == -1, "handle_adopt_results failed with TM_EPERM ", rc);

  rc = 99898;
  rc = handle_adoption_results(rc, DoBackground, pid, JobID, tmpAdopteeID, Args);
  fail_unless(rc == -1, "handle_adopt_results failed with an unrecognized error ", rc);

  rc = TM_SUCCESS;
  Args[0] = strdup("ThisIsNotARealCommand");
  tmpAdopteeID.clear();
  fprintf(stderr, "size %d", (int)tmpAdopteeID.size());
  rc = handle_adoption_results(rc, DoBackground, pid, JobID, tmpAdopteeID, Args);
  fail_unless(rc != PBSE_NONE, "handle_adopt_results failed to execute arguments ", rc);
  }
END_TEST

START_TEST(adopt_process)
  {
  int rc;

  rc = adopt_process(strdup("1"), "3544");
  fail_unless(rc == 1, "handle_adopt_results adopted with an invalid job ", rc);

  rc = adopt_process(strdup("1.localhost"), "100");
  fail_unless(rc == TM_EPERM, "handle_adopt_results adopted with wrong permissions ", rc);

  rc = adopt_process(strdup("1.localhost"), "100sdfsdf");
  fail_unless(rc == PBSE_RMBADPARAM, "handle_adopt_results adopted with invalid pid ", rc);

  rc = adopt_process(strdup("1.localhost"), "->(100");
  fail_unless(rc == PBSE_RMBADPARAM, "handle_adopt_results adopted with invalid pid ", rc);

  rc = adopt_process(strdup("1.localhost"), "10a\';DROP TABLE users;");
  fail_unless(rc == PBSE_RMBADPARAM, "handle_adopt_results adopted with invalid pid ", rc);

  rc = adopt_process(strdup("2.localhost"), "12345");
  fail_unless(rc == PBSE_NONE, "handle_adopt_results failed to adopt valid process ", rc);
  }
END_TEST

START_TEST(fork_process)
  {
  int          rc;
  int          DoBackground;
  int          pid;
  char         *JobID;
  std::string  tmpAdopteeID;
  char        *Args[10];
  char        *argv[3];

  rc = TM_SUCCESS;
  DoBackground = 0;
  pid = 1;
  JobID = strdup("1.localhost");
  optind = 1;
  argv[1] = strdup("echo");
  argv[2] = strdup("nonsense");

  rc = fork_process(3, argv, DoBackground, pid, JobID, Args);
  fail_unless(rc == PBSE_NONE, "handle_adopt_results failed with TM_SUCCESS ", rc);

  rc = TM_SUCCESS;
  DoBackground = 1;
  pid = 1;
  JobID = strdup("1.localhost");
  optind = 1;
  argv[1] = strdup("echo");
  argv[2] = strdup("nonsense");

  rc = fork_process(3, argv, DoBackground, pid, JobID, Args);
  if (pid == 0) {
    fail_unless(rc == PBSE_NONE, "handle_adopt_results failed with TM_SUCCESS ", rc);
    exit(0);
  }
  else {
    fail_unless(rc == -100, "handle_adopt_results failed with TM_SUCCESS ", rc);
  }


  }
END_TEST

Suite *pbs_track_suite(void)
  {
  Suite *s = suite_create("pbs_track_suite methods");
  TCase *tc_core = tcase_create("parse_commandline_opts");
  tcase_add_test(tc_core, parse_commandline_opts);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("handle_adopt_results");
  tcase_add_test(tc_core, handle_adoption_results);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("adopt_process");
  tcase_add_test(tc_core, adopt_process);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("fork_process");
  tcase_add_test(tc_core, fork_process);
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
  sr = srunner_create(pbs_track_suite());
  srunner_set_log(sr, "pbs_track_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
