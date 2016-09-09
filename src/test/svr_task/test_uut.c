#include "license_pbs.h" /* See here for the software license */
#include "work_task.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "pbs_error.h"
#include "threadpool.h"

extern void  check_nodes(struct work_task *ptask);
void         insert_timed_task(work_task *wt);
work_task   *pop_timed_task(time_t  time_now);
bool         can_dispatch_task();
int          dispatch_timed_task(work_task *ptask);


extern all_tasks      task_list_event;
extern task_recycler  tr;
extern threadpool_t  *request_pool;
extern std::list<timed_task> *task_list_timed;

START_TEST(dispatch_timed_task_test)
  {
  work_task wt;

  memset(&wt, 0, sizeof(wt));
  wt.wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  wt.wt_event = 200;

  if (task_list_timed == NULL)
    task_list_timed = new std::list<timed_task>();

  if (request_pool == NULL)
    initialize_threadpool(&request_pool,10,50,50);
  request_pool->tp_max_threads = 50;
  request_pool->tp_nthreads = 50;
  request_pool->tp_idle_threads = 0;

  fail_unless(dispatch_timed_task(&wt) == PBSE_SERVER_BUSY);
  
  request_pool->tp_idle_threads = 6;
  fail_unless(dispatch_timed_task(&wt) == PBSE_NONE);
  fail_unless(wt.wt_being_recycled == TRUE);
  }
END_TEST

START_TEST(can_dispatch_task_test)
  {
  if (request_pool == NULL)
    initialize_threadpool(&request_pool,10,50,50);

  request_pool->tp_max_threads = 50;
  request_pool->tp_nthreads = 50;
  request_pool->tp_idle_threads = 0;

  fail_unless(can_dispatch_task() == false);

  request_pool->tp_idle_threads = 45;
  fail_unless(can_dispatch_task() == true);

  request_pool->tp_idle_threads = 6;
  fail_unless(can_dispatch_task() == true);
  }
END_TEST

START_TEST(manage_timed_task_test)
  {
  work_task  ptask1;
  work_task  ptask2;
  work_task  ptask3;
  work_task *wt;
  int rc;

  memset(&ptask1, 0, sizeof(ptask1));
  ptask1.wt_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(ptask1.wt_mutex, NULL);
  memset(&ptask2, 0, sizeof(ptask2));
  ptask2.wt_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(ptask2.wt_mutex, NULL);
  memset(&ptask3, 0, sizeof(ptask3));
  ptask3.wt_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(ptask3.wt_mutex, NULL);

  if (task_list_timed == NULL)
    task_list_timed = new std::list<timed_task>();

  ptask1.wt_event = 100;
  ptask2.wt_event = 200;
  ptask3.wt_event = 300;

  insert_timed_task(&ptask1);
  insert_timed_task(&ptask2);
  insert_timed_task(&ptask3);

  fail_unless(pop_timed_task(0) == NULL);
  fail_unless(pop_timed_task(99) == NULL);

  wt = pop_timed_task(150);
  fail_unless(wt != NULL);

  // we expect the mutex to be locked
  //   so trylock should fail
  rc = pthread_mutex_trylock(wt->wt_mutex);
  fail_unless(rc == EBUSY);

  fail_unless(wt->wt_event == 100);
  fail_unless(pop_timed_task(150) == NULL);

  wt = pop_timed_task(350);
  fail_unless(wt != NULL);
  fail_unless(wt->wt_event == 200);
  wt = pop_timed_task(350);
  fail_unless(wt != NULL);
  fail_unless(wt->wt_event == 300);
  wt = pop_timed_task(350);
  fail_unless(wt == NULL);
  }
END_TEST

START_TEST(test_one)
  {
  int rc;
  task_list_event.tasks.clear();
  initialize_task_recycler();

  if (task_list_timed == NULL)
    task_list_timed = new std::list<timed_task>();

  rc = initialize_threadpool(&request_pool, 5, 50, 60);
  fail_unless(rc == PBSE_NONE, "initalize_threadpool failed", rc);

  struct work_task *pWorkTask = set_task(WORK_Timed,357,check_nodes,NULL,0);
  fail_unless(pWorkTask != NULL);
  struct work_task *pWorkTask2 = set_task(WORK_Timed,356,check_nodes,NULL,0);
  fail_unless(pWorkTask2 != NULL);
  struct work_task *pWorkTask3 = set_task(WORK_Timed,358,check_nodes,NULL,0);
  fail_unless(pWorkTask3 != NULL);

  rc = dispatch_task(pWorkTask);
  fail_unless(rc == PBSE_NONE, "dispatch_task failed", rc);
  delete_task(pWorkTask);

  std::vector<work_task *>::iterator iter = tr.tasks.tasks.end();
  struct work_task *pRecycled = next_task_from_recycler(&tr.tasks,iter);
  fprintf(stderr,"%p %p\n",(void *)pWorkTask,(void *)pRecycled);
  fail_unless(pRecycled == pWorkTask);
  fail_unless(task_is_in_threadpool(pWorkTask2));
  }
END_TEST

Suite *svr_task_suite(void)
  {
  Suite *s = suite_create("svr_task_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("can_dispatch_task_test");
  tcase_add_test(tc_core, can_dispatch_task_test);
  tcase_add_test(tc_core, manage_timed_task_test);
  tcase_add_test(tc_core, dispatch_timed_task_test);
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
