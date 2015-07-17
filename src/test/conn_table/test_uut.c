#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include <stdlib.h>
#include <stdio.h>
#include <libpbs.h>
#include <server_limits.h>
#include "test_conn_table.h"


#include "pbs_error.h"

#define ENTRIES 3
struct connect_handle connection[PBS_NET_MAX_CONNECTIONS];

START_TEST(test_one)
  {
  int conn_pos;
  int rc;
  int i;

  for(i = 0; i < ENTRIES; i++)
    {
    connection[i].ch_mutex = NULL;
    connection[i].ch_inuse = FALSE;
    connection[i].ch_socket = i;
    }
  rc = get_connection_entry(&conn_pos);
  fail_unless(rc == PBSE_NONE, "get_connection_entry failed for normal case");
  fail_unless(conn_pos >= 0, "invalid connection entry returned");

  }
END_TEST

START_TEST(test_two)
  {
  int conn_pos;
  int rc;
  int i;

  for(i = 0; i < PBS_NET_MAX_CONNECTIONS; i++)
    {
    connection[i].ch_mutex = NULL;
    connection[i].ch_inuse = TRUE;
    connection[i].ch_socket = i;
    }

  rc = get_connection_entry(&conn_pos);
  fail_unless(rc == PBSE_NOCONNECTS, "get_connection_entry failed for normal case: %d", rc);
  fail_unless(conn_pos == -1, "invalid connection entry returned");

  }
END_TEST

START_TEST(test_three)
  {
  int conn_pos;
  int rc;
  int i;

  for(i = 0; i < ENTRIES; i++)
    {
    connection[i].ch_mutex = NULL;
    connection[i].ch_inuse = FALSE;
    connection[i].ch_socket = i;
    }

  rc = get_connection_entry(&conn_pos);
  fail_unless(rc == PBSE_NONE, "get_connection_entry failed for normal case");
  fail_unless(conn_pos >= 0, "invalid connection entry returned");
  connection[conn_pos].ch_inuse = TRUE;
  pthread_mutex_lock(connection[conn_pos].ch_mutex);
  rc = get_connection_entry(&conn_pos);
  fail_unless(rc == PBSE_NONE, "get_connection_entry failed for normal case");
  fail_unless(conn_pos >= 0, "invalid connection entry returned");

  }
END_TEST


Suite *conn_table_suite(void)
  {
  Suite *s = suite_create("conn_table_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_three");
  tcase_add_test(tc_core, test_three);
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
  sr = srunner_create(conn_table_suite());
  srunner_set_log(sr, "conn_table_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
