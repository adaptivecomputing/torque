#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "license_pbs.h" /* See here for the software license */
#include "queue_recov.h"
#include "test_queue_recov.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

extern char *path_queues;

START_TEST(test_one)
  {
  pbs_queue q;

  path_queues = (char *)"./";
  memset(&q,0,sizeof(q));
  strcpy(q.qu_qs.qu_name,"QueueSaveFile");
  fail_unless(que_save(&q) == 0);

  pbs_queue *pQ = que_recov_xml((char *)"QueueSaveFile");
  fail_unless(pQ != NULL);

  free(pQ);

  struct stat b;

  memset(&b,0,sizeof(b));
  fail_unless(stat("./QueueSaveFile",&b) == 0);

  unlink("./QueueSaveFile");

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *queue_recov_suite(void)
  {
  Suite *s = suite_create("queue_recov_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
  sr = srunner_create(queue_recov_suite());
  srunner_set_log(sr, "queue_recov_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
