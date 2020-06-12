#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <poll.h>

#include "qsub_functions.h"
#include "test_qsub_functions.h"
#include "complete_req.hpp"
#include "pbs_error.h"

void process_opt_L(const char *str);
void validate_basic_resourcing(job_info *ji);
bool is_resource_request_valid(job_info *ji, std::string &err_msg);
void add_new_request_if_present(job_info *ji);
bool retry_submit_error(int error);
int  process_opt_d(job_info *ji, const char *cmd_arg, int data_type, job_data *tmp_job_info);
int  process_opt_j(job_info *ji, const char *cmd_arg, int data_type);
int  process_opt_k(job_info *ji, const char *cmd_arg, int data_type);
int  process_opt_K(job_info *ji, const char *cmd_arg, int data_type);
int  process_opt_m(job_info *ji, const char *cmd_arg, int data_type);
int  process_opt_p(job_info *ji, const char *cmd_arg, int data_type);
int  wait_for_read_ready(int, int);
bool is_memory_request_valid(job_info *ji, std::string &err_msg);
int  validate_pbs_o_workdir(const job_info *);
unsigned int get_port_in_range();

extern complete_req cr;
extern bool         mem_fail;
extern bool         submission_string_fail;
extern bool         added_req;
extern bool         find_nodes;
extern bool         find_mpp;
extern bool         find_size;
extern bool         validate_path;
extern bool         init_work_dir;
extern std::string  added_value;
extern std::string  added_name;

extern int global_poll_rc;
extern short global_poll_revents;
extern int global_poll_errno;

extern int interactive_port_min;
extern int interactive_port_max;

bool are_we_forking()

  {
  char *forking = getenv("CK_FORK");

  if ((forking != NULL) &&
      (!strcasecmp(forking, "no")))
    return(false);
  
  return(true);
  }


START_TEST(test_get_port_in_range)
  {
  interactive_port_min = 30000;
  interactive_port_max = 30100;

  for (int i = 0; i <= 100; i++)
    {
    unsigned int port = get_port_in_range();
    fail_unless(port >= interactive_port_min);
    fail_unless(port <= interactive_port_max);
    }
  }
END_TEST


START_TEST(test_is_memory_request_valid)
  {
#ifdef PENABLE_LINUX_CGROUPS
  job_info    ji;
  std::string err;

  mem_fail = false;
  fail_unless(is_memory_request_valid(&ji, err) == true);
  mem_fail = true;
  fail_unless(is_memory_request_valid(&ji, err) == false);
#endif

  }
END_TEST


START_TEST(test_process_opt_K)
  {
  job_info ji;

  // Must be a positive number
  fail_unless(process_opt_K(&ji, NULL, 1) != PBSE_NONE);
  fail_unless(process_opt_K(&ji, "-1", 1) != PBSE_NONE);
  fail_unless(process_opt_K(&ji, "abc", 1) != PBSE_NONE);
  fail_unless(process_opt_K(&ji, "0", 1) != PBSE_NONE);
  fail_unless(process_opt_K(&ji, "30", 1) == PBSE_NONE);
  fail_unless(process_opt_K(&ji, "3", 1) == PBSE_NONE);
  fail_unless(process_opt_K(&ji, "15", 1) == PBSE_NONE);
  }
END_TEST


START_TEST(test_process_opt_p)
  {
  job_info ji;

  fail_unless(process_opt_p(&ji, NULL, 1) != PBSE_NONE);
  fail_unless(process_opt_p(&ji, "-a", 1) != PBSE_NONE);
  fail_unless(process_opt_p(&ji, "-1025", 1) != PBSE_NONE);
  fail_unless(process_opt_p(&ji, "1024", 1) != PBSE_NONE);
  fail_unless(process_opt_p(&ji, "-", 1) != PBSE_NONE);
  fail_unless(process_opt_p(&ji, "  5", 1) == PBSE_NONE);
  fail_unless(added_value == "5");
  fail_unless(process_opt_p(&ji, "+10", 1) == PBSE_NONE);
  fail_unless(added_value == "+10");
  fail_unless(process_opt_p(&ji, "1", 1) == PBSE_NONE);
  fail_unless(added_value == "1");
  fail_unless(process_opt_p(&ji, "1000", 1) == PBSE_NONE);
  fail_unless(added_value == "1000");
  fail_unless(process_opt_p(&ji, "-1000", 1) == PBSE_NONE);
  fail_unless(added_value == "-1000");
  }
END_TEST


START_TEST(test_process_opt_m)
  {
  job_info ji;

  fail_unless(process_opt_m(&ji, NULL, 1) != PBSE_NONE);
  fail_unless(process_opt_m(&ji, "bob", 1) != PBSE_NONE);
  fail_unless(process_opt_m(&ji, "a", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "b", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "e", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "ae", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "ab", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "ba", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "be", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "bea", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "bae", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "abe", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "aeb", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "eb", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "ea", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "eba", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "eab", 1) == PBSE_NONE);
  fail_unless(process_opt_m(&ji, "bobo", 1) != PBSE_NONE);
  }
END_TEST


START_TEST(test_process_opt_k)
  {
  job_info ji;

  fail_unless(process_opt_k(&ji, NULL, 1) != PBSE_NONE);
  fail_unless(process_opt_k(&ji, "bob", 1) != PBSE_NONE);
  fail_unless(process_opt_k(&ji, "o", 1) == PBSE_NONE);
  fail_unless(process_opt_k(&ji, "e", 1) == PBSE_NONE);
  fail_unless(process_opt_k(&ji, "oe", 1) == PBSE_NONE);
  fail_unless(process_opt_k(&ji, "eo", 1) == PBSE_NONE);
  fail_unless(process_opt_k(&ji, "n", 1) == PBSE_NONE);
  fail_unless(process_opt_k(&ji, "no", 1) != PBSE_NONE);
  }
END_TEST


START_TEST(test_process_opt_j)
  {
  job_info ji;

  fail_unless(process_opt_j(&ji, NULL, 1) != PBSE_NONE);
  fail_unless(process_opt_j(&ji, "bob", 1) != PBSE_NONE);
  fail_unless(process_opt_j(&ji, "n", 1) == PBSE_NONE);
  fail_unless(process_opt_j(&ji, "oe", 1) == PBSE_NONE);
  fail_unless(process_opt_j(&ji, "eo", 1) == PBSE_NONE);
  fail_unless(process_opt_j(&ji, "oeo", 1) != PBSE_NONE);
  }
END_TEST


START_TEST(test_retry_submit_error)
  {
  fail_unless(retry_submit_error(PBSE_NONE) == false);
  fail_unless(retry_submit_error(PBSE_NOATTR) == false);
  fail_unless(retry_submit_error(PBSE_ATTRRO) == false);
  fail_unless(retry_submit_error(PBSE_TIMEOUT));
  }
END_TEST


START_TEST(test_process_opt_d)
  {
  job_info    ji;
  job_data   *jd = NULL;

  // fail with bad arguments to the function
  fail_unless(process_opt_d(NULL, NULL, 1, NULL) != PBSE_NONE);
  fail_unless(process_opt_d(&ji, "/tmp/dbeer/work/", 1, jd) == PBSE_NONE);
  fail_unless(added_value == "/tmp/dbeer/work/", added_value.c_str());

  // current_directory/. will exist on every system
  process_opt_d(&ji, ".", 1, jd);
  char  cwd[1024];
  char *cwd_ptr;

  validate_path = true;

  cwd_ptr = getcwd(cwd, sizeof(cwd));
  std::string current(cwd_ptr);
  current += "/.";
  fail_unless(added_value == current);
  
  // fail with non-existent directory
  fail_unless(process_opt_d(&ji, "/tmp/superfakefakojet/", 1, jd) != PBSE_NONE);
  }
END_TEST


START_TEST(test_process_opt_L)
  {
  job_info    ji;
  const char *arg = "tasks=6";

  added_req = false;
  process_opt_L(arg);
  fail_unless(added_req == true);
  }
END_TEST

START_TEST(test_process_opt_L_fail1)
  {
  job_info    ji;
  // create a bad string failure
  process_opt_L("bob");
  }
END_TEST


START_TEST(test_process_opt_L_fail2)
  {
  job_info    ji;
  const char *arg = "tasks=6";

  submission_string_fail = true;
  process_opt_L(arg);
  }
END_TEST


START_TEST(test_add_new_request_if_present)
  {
  extern int req_val;
  extern bool stored_complete_req;
  job_info    ji;

  req_val = 3;
  stored_complete_req = false;
  add_new_request_if_present(&ji);
  fail_unless(stored_complete_req == true);
  }
END_TEST

START_TEST(test_validate_basic_resourcing)
  {
  extern int req_val;
  job_info   ji;

  // as long we don't exit here we're good, all failures exit
  req_val = 3;
  find_mpp = false;
  find_nodes = false;
  find_size = false;
  validate_basic_resourcing(&ji);

  req_val = 0;
  find_mpp = true;
  validate_basic_resourcing(&ji);

  find_mpp = false;
  find_nodes = true;
  validate_basic_resourcing(&ji);

  find_nodes = false;
  find_size = true;
  validate_basic_resourcing(&ji);

  find_size = false;
  // none should also work
  validate_basic_resourcing(&ji);

  }
END_TEST



extern std::vector<std::string> in_hash;

START_TEST(test_is_resource_request_valid)
  {
  job_info     ji;
  std::string  err_msg;
  const char  *resc_array[] = {"nodes", "mppwidth", "ncpus", "size"};

  // Empty shouldn't fail
  in_hash.clear();
  fail_unless(is_resource_request_valid(&ji, err_msg) == true);

  // Any one of these should work
  for (int i = 0; i < 4; i++)
    {
    in_hash.clear();
    in_hash.push_back(resc_array[i]);
    fail_unless(is_resource_request_valid(&ji, err_msg) == true);
    }

  // Any two of these should fail
  for (int i = 0; i < 4; i++)
    {
    in_hash.clear();
    in_hash.push_back(resc_array[i]);

    for (int j = 0; j < 4; j++)
      {
      if (j != i)
        {
        in_hash.push_back(resc_array[j]);
        fail_unless(is_resource_request_valid(&ji, err_msg) == false);
        in_hash.pop_back();
        }
      }
    }
  }
END_TEST


START_TEST(test_x11_get_proto_1)
  {
  char *param1 = NULL;
  char *resp = NULL;
  int debug = 1;
  resp = x11_get_proto(param1, debug);
  fail_unless(resp == NULL);
  }
END_TEST


START_TEST(test_isWindowsFormat)
  {
  char *tempfilename = tempnam("/tmp", "test");
  fail_unless((tempfilename != NULL), "Failed to create a temporary filename");
  FILE *fp = fopen(tempfilename, "w");
  fail_unless(fp != NULL, "Failed to create a file to test on Windows text format");

  //write in dos format
  fprintf(fp, "line 1, blah blah blah\r\n");
  fprintf(fp, "line 2, blah blah blah\r\n");
  fprintf(fp, "line 3, blah blah blah\r\n");
  int s = fclose(fp);
  fp = fopen(tempfilename, "r");
  fail_unless(fp != NULL, "Failed to open file %s for read", tempfilename);
  //s = isWindowsFormat(fp);
  //fail_unless(s==1, "Failed to detect Windows format text file");
  fclose(fp);

  fopen(tempfilename, "w");
  fail_unless(fp != NULL, "Failed to create a file to test on Windows text format");
  
  //write in unix format
  fprintf(fp, "line 1, blah blah blah\n");
  fprintf(fp, "line 2, blah blah blah\n");
  fprintf(fp, "line 3, blah blah blah\n");
  s = fclose(fp);

  fp = fopen(tempfilename, "r");
  fail_unless(fp != NULL, "Failed to open file %s for read", tempfilename);
  s = isWindowsFormat(fp);
  fail_unless(s==0, "Failed to detect UNIX format text file");
  fclose(fp);
  unlink(tempfilename);
  }
END_TEST

START_TEST(test_make_argv)
  {
#define MAX_ARGV_LEN 16
  int argc;
  char *vect[MAX_ARGV_LEN + 1] = {};

  /* 0: "qsub"         1            2                3                     4    5    6   7 8 */
  char const * line = "simple_arg \"quoted ' arg\" \'s\"quoted \" arg\' \\\\ \\\" \\\' \\  end";
  make_argv(&argc, vect, line);
  fail_unless(argc == 9);
  fail_unless(strcmp(vect[0], "qsub") == 0);
  fail_unless(strcmp(vect[1], "simple_arg") == 0);
  fail_unless(strcmp(vect[2], "quoted ' arg") == 0);
  fail_unless(strcmp(vect[3], "s\"quoted \" arg") == 0);
  fail_unless(strcmp(vect[4], "\\") == 0);
  fail_unless(strcmp(vect[5], "\"") == 0);
  fail_unless(strcmp(vect[6], "\'") == 0);
  fail_unless(strcmp(vect[7], " ") == 0);
  fail_unless(strcmp(vect[8], "end") == 0);
  fail_unless(vect[9] == NULL);

  /* two args that are (escaped) spaces + test mem free/alloc no-fail test */
  line = "\\  \\\t";
  make_argv(&argc, vect, line);
  fail_unless(argc == 3);
  fail_unless(strcmp(vect[0], "qsub") == 0);
  fail_unless(strcmp(vect[1], " ") == 0);
  fail_unless(strcmp(vect[2], "\t") == 0);

  /* no arguments + mem free/alloc no-fail test */
  line = "      \t     ";
  make_argv(&argc, vect, line);
  fail_unless(argc == 1);
  fail_unless(strcmp(vect[0], "qsub") == 0);
  }
END_TEST

START_TEST(test_wait_for_read_ready)
  {
  int rc;
  int some_fd = 0; // arbitrary value
  int some_timeout_sec = 1; // arbitrary value > 0

  // emulate timeout
  global_poll_rc = 0;
  global_poll_revents = 0;
  rc = wait_for_read_ready(some_fd, some_timeout_sec);
  fail_unless(rc == 0);

  // emulate ready to read
  global_poll_rc = 1;
  global_poll_revents = POLLIN;
  rc = wait_for_read_ready(some_fd, some_timeout_sec);
  fail_unless(rc == 1);

  // emulate not ready to read
  global_poll_rc = 1;
  global_poll_revents = 0;
  rc = wait_for_read_ready(some_fd, some_timeout_sec);
  fail_unless(rc == 0);

  // emulate failure
  global_poll_rc = -1;
  global_poll_errno = EFAULT;
  rc = wait_for_read_ready(some_fd, some_timeout_sec);
  fail_unless(rc == -1);

  // emulate recoverable failure
  global_poll_rc = -1;
  global_poll_errno = EINTR;
  rc = wait_for_read_ready(some_fd, some_timeout_sec);
  fail_unless(rc == 0);
  }
END_TEST


START_TEST(test_validate_pbs_o_workdir)
  {
  job_info    ji;

  validate_path = true;

  init_work_dir = true;
  validate_pbs_o_workdir(&ji);
  fail_unless(strcmp(added_name.c_str(), "..") != 0);
  }
END_TEST


Suite *qsub_functions_suite(void)
  {
  Suite *s = suite_create("qsub_functions methods");
  TCase *tc_core = tcase_create("test x11_get_proto_1");
  tcase_add_test(tc_core, test_x11_get_proto_1);
  tcase_add_test(tc_core, test_process_opt_d);
  tcase_add_test(tc_core, test_process_opt_j);
  tcase_add_test(tc_core, test_process_opt_k);
  tcase_add_test(tc_core, test_process_opt_K);
  tcase_add_test(tc_core, test_process_opt_m);
  tcase_add_test(tc_core, test_process_opt_p);
  tcase_add_test(tc_core, test_retry_submit_error);
  tcase_add_test(tc_core, test_validate_pbs_o_workdir);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test isWindowsFormat");
  tcase_add_test(tc_core, test_isWindowsFormat);
  tcase_add_test(tc_core, test_process_opt_L);
  tcase_add_test(tc_core, test_add_new_request_if_present);
  tcase_add_test(tc_core, test_validate_basic_resourcing);
  suite_add_tcase(s, tc_core);
  
  if (are_we_forking() == true)
    {
    tc_core = tcase_create("test_failures");
    tcase_add_exit_test(tc_core, test_process_opt_L_fail1, 2);
    tcase_add_exit_test(tc_core, test_process_opt_L_fail2, 2);
    suite_add_tcase(s, tc_core);
    }

  tc_core = tcase_create("test_make_argv");
  tcase_add_test(tc_core, test_make_argv);
  tcase_add_test(tc_core, test_is_resource_request_valid);
  tcase_add_test(tc_core, test_get_port_in_range);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_wait_for_read_ready");
  tcase_add_test(tc_core, test_wait_for_read_ready);
  tcase_add_test(tc_core, test_is_memory_request_valid);
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

  sr = srunner_create(qsub_functions_suite());
  srunner_set_log(sr, "qsub_functions_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
