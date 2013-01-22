#include "license_pbs.h" /* See here for the software license */
#include "svr_task.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

extern void check_nodes(struct work_task *ptask);


all_tasks task_list_timed;
all_tasks task_list_event;
task_recycler tr;

START_TEST(test_one)
  {
  initialize_all_tasks_array(&task_list_timed);
  initialize_all_tasks_array(&task_list_event);
  initialize_task_recycler();

  struct work_task *pWorkTask = set_task(WORK_Timed,357,check_nodes,NULL,0);
  fail_unless(pWorkTask != NULL);
  struct work_task *pWorkTask2 = set_task(WORK_Timed,356,check_nodes,NULL,0);
  fail_unless(pWorkTask2 != NULL);
  struct work_task *pWorkTask3 = set_task(WORK_Timed,358,check_nodes,NULL,0);
  fail_unless(pWorkTask3 != NULL);

  dispatch_task(pWorkTask);
  delete_task(pWorkTask);

  int iter = -1;
  struct work_task *pRecycled = next_task_from_recycler(&tr.tasks,&iter);
  fprintf(stderr,"%p %p\n",(void *)pWorkTask,(void *)pRecycled);
  fail_unless(pRecycled == pWorkTask);
  fail_unless(task_is_in_threadpool(pWorkTask2));



  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *svr_task_suite(void)
  {
  Suite *s = suite_create("svr_task_suite methods");
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
  sr = srunner_create(svr_task_suite());
  srunner_set_log(sr, "svr_task_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
