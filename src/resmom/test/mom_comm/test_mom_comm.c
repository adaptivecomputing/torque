#include "license_pbs.h" /* See here for the software license */
#include "mom_comm.h"
#include "test_mom_comm.h"
#include <stdlib.h>
#include <stdio.h>

#include "dis.h"
#include "pbs_error.h"
#include "pbs_nodes.h"

received_node *get_received_node_entry(char *str);

START_TEST(test_read_status_strings_null_chan_doesnt_crash)
  {
  fail_unless(DIS_INVALID == read_status_strings(NULL, 1));
  }
END_TEST

extern int disrst_count;

START_TEST(test_read_status_strings_loop)
  {
  struct tcp_chan chan;
  disrst_count = 5;
  read_status_strings(&chan, 1);
  }
END_TEST

START_TEST(test_get_received_node_entry)
  {
  fail_unless(get_received_node_entry("pickle") != NULL);
  }
END_TEST

Suite *mom_comm_suite(void)
  {
  Suite *s = suite_create("mom_comm_suite methods");
  TCase * tc = tcase_create("mom_comm");

  tcase_add_test(tc, test_read_status_strings_null_chan_doesnt_crash);
  tcase_add_test(tc, test_read_status_strings_loop);
  tcase_add_test(tc, test_get_received_node_entry);

  suite_add_tcase(s, tc);
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
  sr = srunner_create(mom_comm_suite());
  srunner_set_log(sr, "mom_comm_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
