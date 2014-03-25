#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "mutex_mgr.hpp"
#include "pbs_job.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <check.h>
#include "pbs_error.h"

using namespace std;

START_TEST(initialize_mutex)
  {
  pthread_mutex_t test_mutex;
  int rc;

  rc = pthread_mutex_init(&test_mutex, NULL);
  fail_unless(rc == 0, "could not initialize mutex");

  mutex_mgr my_mutex = mutex_mgr(&test_mutex);
  fail_unless(my_mutex.is_valid() == true, "Failed to initialize and lock mutex");
  }
END_TEST

START_TEST(invalid_mutex)
  {
  pthread_mutex_t *test_mutex = NULL;
  int rc;

  mutex_mgr my_mutex(test_mutex);
  fail_unless(my_mutex.is_valid() == false, "Null mutex did not create error");

  rc = my_mutex.lock();
  fail_unless(rc == PBSE_INVALID_MUTEX, "Mutex should be invalid on this lock");
  rc = my_mutex.unlock();
  fail_unless(rc == PBSE_INVALID_MUTEX, "Mutex should be invalid on this unlock");
  }
END_TEST

START_TEST(unlock_mutex)
  {
  pthread_mutex_t test_mutex;
  int rc;

  rc = pthread_mutex_init(&test_mutex, NULL);
  fail_unless(rc == 0, "could not initialize mutex");

  mutex_mgr my_mutex = mutex_mgr(&test_mutex);
  fail_unless(my_mutex.is_valid() == true, "Failed to initialize and lock mutex");

  rc = my_mutex.unlock();
  fail_unless(rc == PBSE_NONE, "failed to unlock mutex");
  }
END_TEST

START_TEST(lock_mutex)
  {
  pthread_mutex_t test_mutex;
  int rc;

  rc = pthread_mutex_init(&test_mutex, NULL);
  fail_unless(rc == 0, "could not initialize mutex");

  mutex_mgr my_mutex = mutex_mgr(&test_mutex);
  fail_unless(my_mutex.is_valid() == true, "Failed to initialize and lock mutex");

  rc = my_mutex.unlock();
  fail_unless(rc == PBSE_NONE, "failed to unlock mutex");

  rc = my_mutex.lock();
  fail_unless(rc == PBSE_NONE, "failed to unlock mutex");
  }
END_TEST

START_TEST(mutex_already_locked)
  {
  pthread_mutex_t test_mutex;
  int rc;

  rc = pthread_mutex_init(&test_mutex, NULL);
  fail_unless(rc == 0, "could not initialize mutex");

  mutex_mgr my_mutex = mutex_mgr(&test_mutex);
  fail_unless(my_mutex.is_valid() == true, "Failed to initialize and lock mutex");

  rc = my_mutex.lock();
  fail_unless(rc == PBSE_MUTEX_ALREADY_LOCKED, "Failed to lock mutex");
  }
END_TEST

START_TEST(mutex_already_unlocked)
  {
  pthread_mutex_t test_mutex;
  int rc;

  rc = pthread_mutex_init(&test_mutex, NULL);
  fail_unless(rc == 0, "could not initialize mutex");

  mutex_mgr my_mutex = mutex_mgr(&test_mutex);
  fail_unless(my_mutex.is_valid() == true, "Failed to initialize and lock mutex");

  rc = my_mutex.unlock();
  fail_unless(rc == PBSE_NONE, "Failed to lock mutex");

  rc = my_mutex.unlock();
  fail_unless(rc == PBSE_MUTEX_ALREADY_UNLOCKED, "Failed to lock mutex");
  }
END_TEST

START_TEST(set_lock_exit)
  {
  pthread_mutex_t test_mutex;
  int rc;

  rc = pthread_mutex_init(&test_mutex, NULL);
  fail_unless(rc == 0, "could not initialize mutex");

  mutex_mgr my_mutex = mutex_mgr(&test_mutex);
  fail_unless(my_mutex.is_valid() == true, "Failed to initialize and lock mutex");

  my_mutex.set_unlock_on_exit(false);
  my_mutex.set_unlock_on_exit(true);
  }
END_TEST


Suite *u_mutex_mgr_suite(void)
  {
  Suite *s = suite_create("u_mutex_mgr methods");
  TCase *tc_core = tcase_create("initialize_mutex");
  tcase_set_timeout(tc_core,600);
  tcase_add_test(tc_core, initialize_mutex);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("invalid_mutex");
  tcase_add_test(tc_core,invalid_mutex);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("unlock_mutex");
  tcase_add_test(tc_core,unlock_mutex);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("lock_mutex");
  tcase_add_test(tc_core,lock_mutex);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("mutex_already_locked");
  tcase_add_test(tc_core,mutex_already_locked);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("mutex_already_unlocked");
  tcase_add_test(tc_core,mutex_already_unlocked);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("set_lock_exit");
  tcase_add_test(tc_core,set_lock_exit);
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
  sr = srunner_create(u_mutex_mgr_suite());
  srunner_set_log(sr, "u_mutext_mgr_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
