#include "license_pbs.h" /* See here for the software license */
#include "pbsdsh.h"
#include "test_pbsdsh.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>


#include "pbs_error.h"

int numnodes = 5;

int gethostnames_from_nodefile(char **, char *);
int build_listener(int *);
int fork_listener_handler(int, int);
int fork_read_handler(int, bool);
ssize_t read_blocking_socket(int, void*, ssize_t);

int stdoutfd;
int stdoutport = -1;

START_TEST(test_gethostnames_from_nodefile)
  {
  int count;
  char *allnodes;

  count = gethostnames_from_nodefile(&allnodes, strdup("pbs_nodefile"));
  fail_unless(count == numnodes);
  }
END_TEST

START_TEST(test_fork_listener_handler)
  {
  int pid;
  int rc;

  pid = fork_listener_handler(-1, -1);
  // check to make sure fork worked
  fail_unless(pid != -1);

  // kill the forked child
  rc = kill(pid, SIGKILL);
  fail_unless(rc == 0);
  }
END_TEST

START_TEST(test_fork_read_handler)
  {
  int pid;
  int rc;
  int status;

  pid = fork_read_handler(-1, TRUE);
  fail_unless(pid > 0);

  // kill the forked child
  rc = kill(pid, SIGKILL);
  fail_unless(rc == 0);

  // set up listening socket for stdout
  stdoutfd = build_listener(&stdoutport);
  fail_unless(stdoutport >= 0);

  // check to make sure fork worked
  pid = fork_read_handler(stdoutport, TRUE);
  fail_unless(pid != -1);

  waitpid(pid, &status, 0);
  fail_unless((WIFEXITED(status) == TRUE) && (WEXITSTATUS(status) == 0));

  // kill the forked child (should fail since child exited)
  rc = kill(pid, SIGKILL);
  fail_unless(rc == -1);
  }
END_TEST

Suite *pbsdsh_suite(void)
  {
  Suite *s = suite_create("pbsdsh_suite methods");
  TCase *tc_core = tcase_create("test_gethostnames_from_nodefile");
  tcase_add_test(tc_core, test_gethostnames_from_nodefile);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_fork_listener_handler");
  tcase_add_test(tc_core, test_fork_listener_handler);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_fork_read_handler");
  tcase_add_test(tc_core, test_fork_read_handler);
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
  sr = srunner_create(pbsdsh_suite());
  srunner_set_log(sr, "pbsdsh_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
