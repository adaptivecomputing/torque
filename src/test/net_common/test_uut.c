#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include "test_net_common.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "pbs_error.h"
#include "net_cache.h"
#include "lib_net.h"

bool socket_success;
bool close_success;
bool connect_success;

int get_random_reserved_port();
int process_and_save_socket_error(int socket_errno);

START_TEST(test_get_random_reserved_port)
  {
  for (int i = 0; i < 2048; i++)
    fail_unless(get_random_reserved_port() < 1024);

  }
END_TEST

START_TEST(test_process_and_save_socket_error)
  {
  int rc = process_and_save_socket_error(ETIMEDOUT);

  fail_unless(rc == PERMANENT_SOCKET_FAIL);
  fail_unless(errno == ETIMEDOUT);

  fail_unless(process_and_save_socket_error(EINTR) == TRANSIENT_SOCKET_FAIL);
  fail_unless(errno == EINTR);

  fail_unless(process_and_save_socket_error(EAGAIN) == TRANSIENT_SOCKET_FAIL);
  fail_unless(errno == EAGAIN);

  fail_unless(process_and_save_socket_error(EHOSTUNREACH) == PERMANENT_SOCKET_FAIL);
  fail_unless(errno == EHOSTUNREACH);
  }
END_TEST

START_TEST(test_connect_to_trqauthd)
  {
  int rc;
  int sock;

  socket_success = true;
  close_success = true;
  connect_success = true;
   
  rc = connect_to_trqauthd(&sock);
  fail_unless(rc == PBSE_NONE, "connect_to_trqauthd failed", rc);

  socket_success = false;
  rc = connect_to_trqauthd(&sock);
  fail_unless(rc == PBSE_SOCKET_FAULT, "connect_to_trqauthd failed", rc);

  socket_success = true;
  connect_success = false;
  rc = connect_to_trqauthd(&sock);
  fail_unless(rc == PBSE_SOCKET_FAULT, "connect_to_trqauthd failed", rc);

  }
END_TEST

START_TEST(test_socket_get_unix)
  {
  int rc;

  socket_success = true;
  close_success = true;
  connect_success = true;

  rc = socket_get_unix();
  fail_unless(rc >= 0, "socket_get_unix failed", rc);

  socket_success = false;
  rc = socket_get_unix();
  fail_unless(rc < 0, "socket_get_unix failed", rc);

  }
END_TEST
    
START_TEST(test_socket_connect_unix)
  {
  int rc;
  int sock = 10;
  char *err_msg;
  const char *unix_sockname = "somename";

  socket_success = true;
  close_success = true;
  connect_success = true;

  rc = socket_connect_unix(sock, unix_sockname, &err_msg);
  fail_unless(rc == PBSE_NONE, "socket_connect_unix failed", rc);

  connect_success = false;
  rc = socket_connect_unix(sock, unix_sockname, &err_msg);
  fail_unless(rc == PBSE_DOMAIN_SOCKET_FAULT, "socket_connect_unix failed", rc);

  }
END_TEST

Suite *net_common_suite(void)
  {
  Suite *s = suite_create("net_common_suite methods");
  TCase *tc_core = tcase_create("test_get_random_reserved_port");
  tcase_add_test(tc_core, test_get_random_reserved_port);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_connect_to_trqauthd");
  tcase_add_test(tc_core, test_connect_to_trqauthd);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_process_and_save_socket_error");
  tcase_add_test(tc_core, test_process_and_save_socket_error);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_socket_get_unix");
  tcase_add_test(tc_core, test_socket_get_unix);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_socket_connect_unix");
  tcase_add_test(tc_core, test_socket_connect_unix);
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
  sr = srunner_create(net_common_suite());
  srunner_set_log(sr, "net_common_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
