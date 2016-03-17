#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_PBSD_rdrpy.h"
#include <stdlib.h>
#include <stdio.h>

#include "dis.h"
#include "pbs_error.h"

void set_pbs_errno_from_dis_errcode(int *local_errno, int dis_errcode);

extern bool timeout;
extern bool setup_fail;
extern int  decode_rc;

START_TEST(test_PBSD_rdrpy)
  {
  int local_errno = 0;

  // Make sure invalid parameters fail
  fail_unless(PBSD_rdrpy(NULL, -1) == NULL);
  fail_unless(PBSD_rdrpy(NULL, PBS_NET_MAX_CONNECTIONS) == NULL);

  // Make sure we get NULL if DIS_tcp_setup fails
  setup_fail = true;
  fail_unless(PBSD_rdrpy(&local_errno, 1) == NULL);

  // Make sure the timeout message is properly propagated.
  setup_fail = false;
  decode_rc = -1;
  timeout = true;

  fail_unless(PBSD_rdrpy(&local_errno, 1) == NULL);
  fail_unless(!strcmp("We timed out!", connection[1].ch_errtxt), "message: %s",
    connection[1].ch_errtxt);
  fail_unless(connection[1].ch_errno == PBSE_TIMEOUT);

  timeout = false;
  decode_rc = DIS_OVERFLOW;
  fail_unless(PBSD_rdrpy(&local_errno, 1) == NULL);
  fail_unless(!strcmp(dis_emsg[DIS_OVERFLOW], connection[1].ch_errtxt), "message: %s",
    connection[1].ch_errtxt);
  fail_unless(connection[1].ch_errno == PBSE_PROTOCOL);

  decode_rc = PBSE_NONE;
  fail_unless(PBSD_rdrpy(&local_errno, 1) != NULL);
  fail_unless(connection[1].ch_errno == 0);
  fail_unless(connection[1].ch_errtxt == NULL);
  }
END_TEST

START_TEST(set_pbs_errno_from_dis_errcode_test)
  {
  int local_error = 0;

  for (int i = 1; i < DIS_INVALID; i++)
    {
    set_pbs_errno_from_dis_errcode(&local_error, i);

    switch (i)
      {
      case DIS_NOMALLOC:

        fail_unless(local_error == PBSE_MEM_MALLOC);
        break;

      case DIS_EOF:
        fail_unless(local_error == PBSE_EOF);
        break;
        
      default:
        fail_unless(local_error == PBSE_PROTOCOL);
        break;
      }
    }
  
  // Make sure PBSE_* is a straight copy
  for (int i = PBSE_FLOOR + 1; i < PBSE_CEILING; i++)
    {
    set_pbs_errno_from_dis_errcode(&local_error, i);
    fail_unless(local_error == i);
    }
  }
END_TEST

Suite *PBSD_rdrpy_suite(void)
  {
  Suite *s = suite_create("PBSD_rdrpy_suite methods");
  TCase *tc_core = tcase_create("test_PBSD_rdrpy");
  tcase_add_test(tc_core, test_PBSD_rdrpy);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("set_pbs_errno_from_dis_errcode_test");
  tcase_add_test(tc_core, set_pbs_errno_from_dis_errcode_test);
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
  sr = srunner_create(PBSD_rdrpy_suite());
  srunner_set_log(sr, "PBSD_rdrpy_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
