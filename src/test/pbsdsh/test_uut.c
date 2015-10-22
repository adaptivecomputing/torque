#include "license_pbs.h" /* See here for the software license */
#include "test_pbsdsh.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
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
int getvars_from_string(const char *, char **, int, int *, bool, char *, int);
int getallvars_from_env(const char **, char **, int, int *, char *, int);

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

START_TEST(test_getvars_from_string)
  {
  char *var_list_string = NULL;
  char **envp_out = NULL;
  int envp_out_index = 0;
  char errbuf[1024];
  int max_entries = 100;

  // expected failures

  fail_unless(getvars_from_string(var_list_string, envp_out, 0, &envp_out_index, false, NULL, 0) == 1);
  fail_unless((var_list_string = strdup("FIRSTNAME")) != NULL);
  fail_unless(getvars_from_string(var_list_string, envp_out, 0, &envp_out_index, false, NULL, 0) == 1);
  fail_unless((envp_out = (char **)calloc(max_entries, sizeof(char **))) != NULL);
  fail_unless(getvars_from_string(var_list_string, envp_out, max_entries - 1, NULL, false, NULL, 0) == 1);

  // test error string
  errbuf[0] = '\0';
  fail_unless(getvars_from_string(NULL, envp_out, max_entries - 1, &envp_out_index, false, errbuf, sizeof(errbuf)) == 1);
  fail_unless(strncmp("var_list_string is NULL", errbuf, 23) == 0);

  free(var_list_string);
  var_list_string = NULL;

  // check basic condition (no vars)
  envp_out_index = 0;
  fail_unless(getvars_from_string("", envp_out, max_entries - 1, &envp_out_index, false, NULL, 0) == 0);
  fail_unless(envp_out_index == 0);
  fail_unless(envp_out[0] == NULL);

  // put env var into environment
  fail_unless(putenv((char *)"FIRSTNAME=mike") == 0);
  // specify which to find
  fail_unless((var_list_string = strdup("FIRSTNAME")) != NULL);
  // get name=value into envp_out
  envp_out_index = 0;
  fail_unless(getvars_from_string(var_list_string, envp_out, max_entries - 1, &envp_out_index, false, NULL, 0) == 0);
  // check expected outcome
  fail_unless(envp_out_index == 1);
  fail_unless(strcmp("FIRSTNAME=mike", envp_out[0]) == 0);

  // clean up
  free(var_list_string);
  var_list_string = NULL;
  free(envp_out[0]);
  envp_out[0] = NULL;

  // put another env var into environment
  fail_unless(putenv((char *)"LASTNAME=jones") == 0);
  // specify which to find
  fail_unless((var_list_string = strdup("FIRSTNAME,LASTNAME")) != NULL);
  // get name=value into envp_out
  envp_out_index = 0;
  fail_unless(getvars_from_string(var_list_string, envp_out, max_entries - 1, &envp_out_index, false, NULL, 0) == 0);
  // check expected outcome
  fail_unless(envp_out_index == 2);
  fail_unless(strcmp("FIRSTNAME=mike", envp_out[0]) == 0);
  fail_unless(strcmp("LASTNAME=jones", envp_out[1]) == 0);

  // clean up
  free(var_list_string);
  var_list_string = NULL;
  free(envp_out[0]);
  envp_out[0] = NULL;
  free(envp_out[1]);
  envp_out[1] = NULL;

  // check for non-existent variable
  // specify which to find
  fail_unless((var_list_string = strdup("FOO")) != NULL);
  // get name=value into envp_out
  envp_out_index = 0;
  fail_unless(getvars_from_string(var_list_string, envp_out, max_entries - 1, &envp_out_index, false, NULL, 0) == 0);
  // check expected outcome
  fail_unless(envp_out_index == 1);
  fail_unless(strcmp(envp_out[0], "FOO=") == 0);
  fail_unless(envp_out[1] == NULL);
  free(var_list_string);

  // check out of space (2 things to copy but only 1 slot)
  errbuf[0] = '\0';
  var_list_string = strdup("LASTNAME,FIRSTNAME");
  envp_out_index = 0;
  fail_unless(getvars_from_string(var_list_string, envp_out, 1, &envp_out_index, false, errbuf, sizeof(errbuf)) == 1);
  // errbuf should have a value
  fail_unless(errbuf[0] != '\0');

  // clean up
  free(var_list_string);

  // check name=value
  errbuf[0] = '\0';
  var_list_string = strdup("LASTNAME,FRUIT=orange,FIRSTNAME,BLAM=");
  envp_out_index = 0;
  fail_unless(getvars_from_string(var_list_string, envp_out, max_entries - 1, &envp_out_index, false, errbuf, sizeof(errbuf)) == 0);
  fail_unless(envp_out_index == 4);
  fail_unless(strcmp(envp_out[0], "LASTNAME=jones") == 0);
  fail_unless(strcmp(envp_out[1], "FRUIT=orange") == 0);
  fail_unless(strcmp(envp_out[2], "FIRSTNAME=mike") == 0);
  fail_unless(strcmp(envp_out[3], "BLAM=") == 0);
  fail_unless(envp_out[4] == NULL);
  fail_unless(errbuf[0] == '\0');

  // check when env already set
  errbuf[0] = '\0';
  var_list_string = strdup("LASTNAME,VEGGIE=celery,FIRSTNAME");
  fail_unless(getvars_from_string(var_list_string, envp_out, max_entries - 1, &envp_out_index, true, errbuf, sizeof(errbuf)) == 0);
  fail_unless(envp_out_index == 5);
  fail_unless(strcmp(envp_out[0], "LASTNAME=jones") == 0);
  fail_unless(strcmp(envp_out[1], "FRUIT=orange") == 0);
  fail_unless(strcmp(envp_out[2], "FIRSTNAME=mike") == 0);
  fail_unless(strcmp(envp_out[3], "BLAM=") == 0);
  fail_unless(strcmp(envp_out[4], "VEGGIE=celery") == 0);
  fail_unless(envp_out[5] == NULL);
  fail_unless(errbuf[0] == '\0');
  }
END_TEST

START_TEST(test_getallvars_from_env)
  {
  char **envp_in = NULL;
  char **envp_out = NULL;
  int envp_out_index = 0;
  char errbuf[1024];
  int max_entries = 100;

  // expected failures

  fail_unless(getallvars_from_env((const char **)envp_in, envp_out, 0, &envp_out_index, NULL, 0) == 1);
  fail_unless((envp_in = (char **)calloc(max_entries, sizeof(char **))) != NULL);
  fail_unless(getallvars_from_env((const char **)envp_in, envp_out, 0, &envp_out_index, NULL, 0) == 1);
  fail_unless((envp_out = (char **)calloc(max_entries, sizeof(char **))) != NULL);
  fail_unless(getallvars_from_env((const char **)envp_in, envp_out, max_entries - 1, NULL, NULL, 0) == 1);

  // test error string
  errbuf[0] = '\0';
  fail_unless(getallvars_from_env((const char **)envp_in, NULL, 0, &envp_out_index, errbuf, sizeof(errbuf)) == 1);
  fail_unless(strncmp("envp_out is NULL", errbuf, 16) == 0);

  // basic test
  envp_out_index = 0;
  fail_unless(getallvars_from_env((const char **)envp_in, envp_out, max_entries - 1, &envp_out_index, NULL, 0) == 0);
  fail_unless(envp_out_index == 0);

  // create some variables
  fail_unless((envp_in[0] = strdup("FIRSTNAME=mike")) != NULL);
  fail_unless((envp_in[1] = strdup("LASTNAME=jones")) != NULL);
  fail_unless((envp_in[2] = strdup("OTHERSTUFF=foo")) != NULL);
  envp_out_index = 0;
  fail_unless(getallvars_from_env((const char **)envp_in, envp_out, max_entries - 1, &envp_out_index, NULL, 0) == 0);
  fail_unless(envp_out_index == 3);
  fail_unless(strcmp("FIRSTNAME=mike", envp_out[0]) == 0);
  fail_unless(strcmp("LASTNAME=jones", envp_out[1]) == 0);
  fail_unless(strcmp("OTHERSTUFF=foo", envp_out[2]) == 0);

  // clean up
  free(envp_out[0]);
  envp_out[0] = NULL;
  free(envp_out[1]);
  envp_out[1] = NULL;
  free(envp_out[2]);
  envp_out[2] = NULL;

  // check out of space (3 things to copy but only 2 slots)
  errbuf[0] = '\0';
  envp_out_index = 0;
  fail_unless(getallvars_from_env((const char **)envp_in, envp_out, 2, &envp_out_index, errbuf, sizeof(errbuf)) == 1);
  // errbuf should have a value
  fail_unless(errbuf[0] != '\0');
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

  tc_core = tcase_create("test_getvars_from_string");
  tcase_add_test(tc_core, test_getvars_from_string);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_getallvars_from_env");
  tcase_add_test(tc_core, test_getallvars_from_env);
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
