#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "test_u_memmgr.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"


#define calloc_fail 1
#define malloc_fail 2

int malloc_count;
int calloc_count;
int free_count;
int tcase;

void *malloc_track(size_t __size)
  {
  void *ptr = NULL;
  if (tcase != malloc_fail)
    ptr = malloc(__size);
  malloc_count++;
  return ptr;
  }

void *calloc_track(size_t __nmemb, size_t __size)
  {
  void *ptr = NULL;
  if (tcase != calloc_fail)
    ptr = calloc(__nmemb, __size);
  calloc_count++;
  return ptr;
  }

void free_track(void *__ptr)
  {
  free(__ptr);
  free_count++;
  }

void init_globals()
  {
  malloc_count = 0;
  calloc_count = 0;
  free_count = 0;
  tcase = 0;
  }

START_TEST(test_memmgr_init_destroy_one)
  {
  int rc = 0;
  memmgr *mm = NULL;
  init_globals();
  rc = memmgr_init(&mm, 16);
  fail_unless(rc == PBSE_NONE, "memmgr_init failed. Code %d", rc);
  fail_unless(calloc_count == 1, "calloc should have been called once %d", calloc_count);
  fail_unless(malloc_count == 1, "malloc should have been called once %d", malloc_count);
  fail_if(mm == NULL, "No memory for mgr was allocated");
  fail_if(mm->the_mem == NULL, "No block memory was allocated");
  fail_unless(mm->alloc_size == 16, "Allocated size set wrong %d", mm->alloc_size);
  fail_unless(mm->remaining == 16, "Remaining size is set wrong %d", mm->remaining);
  fail_unless(mm->ref_count == 0, "ref count should be 0 %d", mm->ref_count);
  fail_unless(mm->current_pos == mm->the_mem, "the start of memory is NOT set correctly");
  fail_unless(mm->prev_mgr == NULL, "prev_mgr should be NULL");
  fail_unless(mm->prev_mgr == NULL, "next_mgr should be NULL");
  fail_unless(mm->current_mgr == mm, "current_mgr should be set to self");
  memmgr_destroy(&mm);
  fail_unless(free_count == 2, "free should have been called once %d", free_count);
  rc = memmgr_init(&mm, 0);
  fail_unless(mm->alloc_size == MEMMGR_DEFAULT_SIZE, "Allocated size set wrong %d", mm->alloc_size);
  fail_unless(mm->remaining == MEMMGR_DEFAULT_SIZE, "Remaining size is set wrong %d", mm->remaining);
  memmgr_destroy(&mm);
  }
END_TEST

START_TEST(test_memmgr_init_fail)
  {
  int rc = 0;
  memmgr *mm = NULL;
  init_globals();
  rc = memmgr_init(NULL, 16);
  fail_unless(rc == -3, "mgr null error should have been returned, but code was %d", rc);

  init_globals();
  tcase = malloc_fail;
  rc = memmgr_init(&mm, 16);
  fail_unless(rc == -1, "malloc should have failed, but code was %d", rc);
  fail_unless(free_count == 0, "free should have been called twice");

  init_globals();
  tcase = calloc_fail;
  rc = memmgr_init(&mm, 16);
  fail_unless(rc == -2, "calloc should have failed, but code was %d", rc);
  fail_unless(free_count == 1, "free should have been called twice");
  }
END_TEST

START_TEST(test_memmgr_calloc_shuffle_shuffle_extend)
  {
  int rc = 0;
  char *ptr = NULL;
  char *data_ptr1 = NULL;
  char *data_ptr2 = NULL;
  char *data_ptr3 = NULL;
  int size = 0;
  memmgr *mm = NULL;
  init_globals();
  ptr = (char *)memmgr_calloc(NULL, 1, 5);
  fail_unless(ptr == NULL, "Invalid memmgr should generate NULL ptr response");
  ptr = (char *)memmgr_calloc(&mm, 1, 5);
  fail_unless(ptr == NULL, "Invalid memmgr should generate NULL ptr response");
  ptr = (char *)memmgr_calloc(&mm, 0, 5);
  fail_unless(ptr == NULL, "zero memory request should generate NULL ptr response");
  
  rc = memmgr_init(&mm, 48);
  data_ptr1 = (char *)memmgr_calloc(&mm, 1, 5);
  strcpy(data_ptr1, "11111");
  data_ptr2 = (char *)memmgr_calloc(&mm, 1, 6);
  strcpy(data_ptr2, "222222");
  data_ptr3 = (char *)memmgr_calloc(&mm, 1, 7);
  strcpy(data_ptr3, "3333333");
  fail_unless(mm->ref_count == 3, "ref_count should be three but is:%d", mm->ref_count);
  fail_unless(mm->remaining == 15, "remaining should be 15 but is:%d", mm->remaining);

  ptr = (char *)mm->the_mem;
  memcpy(&size, ptr, sizeof(int));
  fail_unless(size == 5, "allocated size should have been 5 but was %d", size);
  ptr = (char *)mm->the_mem + sizeof(int);
  fail_unless(strncmp(ptr, "11111\0", 6) == 0, "value should be [11111]but is:[%s]", ptr);
  ptr += 6;

  memcpy(&size, ptr, sizeof(int));
  fail_unless(size == 6, "allocated size should have been 6 but was %d", size);
  ptr += sizeof(int);
  fail_unless(strncmp(ptr, "222222\0", 7) == 0, "value should be [222222]but is:[%s]", ptr);
  ptr += 7;

  memcpy(&size, ptr, sizeof(int));
  fail_unless(size == 7, "allocated size should have been 7 but was %d", size);
  ptr += sizeof(int);
  fail_unless(strncmp(ptr, "3333333\0", 8) == 0, "value should be [3333333]but is:[%s]", ptr);

  data_ptr1 = (char *)memmgr_calloc(&mm, 1, MEMMGR_DEFAULT_SIZE+1);
  memset(data_ptr1, 5, MEMMGR_DEFAULT_SIZE+1);
  fail_unless(mm->remaining == 0, "The first block should be the MEMMGR_DEFAULT_SIZE+1 which is maxed out, but is: %d", mm->remaining);
  fail_unless(mm->alloc_size == MEMMGR_DEFAULT_SIZE+6, "The first block should be MEMMGR_DEFAULT_SIZE+6 in size, but: %d != %d", MEMMGR_DEFAULT_SIZE+6, mm->alloc_size);
  fail_unless(mm->next_mgr->current_mgr == NULL, "As a shuffle was called, the next_mgr->current_mgr should be NULL");
  fail_unless(mm->prev_mgr == NULL, "There should be no prev_mgr");
  fail_unless(mm->next_mgr != NULL, "There should be a next_mgr");
  fail_unless(mm->current_mgr == mm->next_mgr, "The mgr for next alloc should benext_mgr");
  fail_unless(mm->current_mgr->remaining == 15, "This should still be the same amount as shuffle was called and this mgr was unaffected");
  fail_unless(mm->next_mgr->current_mgr == NULL, "Only the root mgr should have current_mgr populated, not the next_mgr");
  fail_unless(mm->next_mgr->prev_mgr == mm, "The next_mgr->prev_mgr value should be mm");
  ptr = (char *)mm->the_mem;
  memcpy(&size, ptr, sizeof(int));
  fail_unless(size == MEMMGR_DEFAULT_SIZE+1, "The size indicated in the block should be MEMMGR_DEFAULT_SIZE+1, but is:%d", size);
  fail_unless(ptr+sizeof(int) == data_ptr1, "The block allocated should be offset 4 from the start of the array but %x != %x", ptr+sizeof(int), data_ptr1);

  data_ptr1 = (char *)memmgr_calloc(&mm, 2, MEMMGR_DEFAULT_SIZE);
  memset(data_ptr1, 6, MEMMGR_DEFAULT_SIZE*2);
  fail_unless(mm->alloc_size == MEMMGR_DEFAULT_SIZE+6, "The first block should be MEMMGR_DEFAULT_SIZE+6 in size, but: %d != %d", MEMMGR_DEFAULT_SIZE+6, mm->alloc_size);
  fail_unless(mm->next_mgr->alloc_size == MEMMGR_DEFAULT_SIZE*2+5, "The second block should be MEMMGR_DEFAULT_SIZE*2+5 in size, but: %d != %d", MEMMGR_DEFAULT_SIZE*2+5, mm->next_mgr->alloc_size);
  fail_unless(mm->current_mgr->alloc_size == 48, "The current block should be 48 in size, but: %d != %d", mm->current_mgr->alloc_size);
  fail_unless(mm->current_mgr == mm->next_mgr->next_mgr, "mgr current_mgr link is not accurate");
  fail_unless(mm->next_mgr == mm->next_mgr->next_mgr->prev_mgr, "mgr link list is not accurate");

  data_ptr1 = (char *)memmgr_calloc(&mm, 1, 24);
  memset(data_ptr1, 7, 24);
  ptr = (char *)mm->current_mgr->the_mem;
  memcpy(&size, ptr, sizeof(int));
  fail_unless(size == 24, "The size indicated in the block should be 24+1, but is:%d", size);
  fail_unless(mm->current_mgr->prev_mgr == mm->next_mgr->next_mgr, "Error in link list after extend");

  }
END_TEST

START_TEST(test_memmgr_free_remove)
  {
  int rc = 0;
  char *ptr = NULL;
  char *data_ptr1 = NULL;
  char *data_ptr2 = NULL;
  char *data_ptr3 = NULL;
  char *data_ptr4 = NULL;
  char *data_ptr5 = NULL;
  char *data_ptr6 = NULL;
  memmgr *mptr1 = NULL;
  memmgr *mptr2 = NULL;
  memmgr *mptr3 = NULL;
  memmgr *mptr4 = NULL;
  int size = 0;
  memmgr *mm = NULL;
  init_globals();
  rc = memmgr_init(&mm, 48);
  data_ptr1 = (char *)memmgr_calloc(&mm, 1, 5);
  strcpy(data_ptr1, "11111");
  data_ptr2 = (char *)memmgr_calloc(&mm, 1, 6);
  strcpy(data_ptr2, "222222");
  data_ptr3 = (char *)memmgr_calloc(&mm, 1, 7);
  strcpy(data_ptr3, "3333333");
  data_ptr4 = (char *)memmgr_calloc(&mm, 1, MEMMGR_DEFAULT_SIZE+1);
  memset(data_ptr4, 5, MEMMGR_DEFAULT_SIZE+1);
  data_ptr5 = (char *)memmgr_calloc(&mm, 2, MEMMGR_DEFAULT_SIZE);
  memset(data_ptr5, 6, MEMMGR_DEFAULT_SIZE*2);
  data_ptr6 = (char *)memmgr_calloc(&mm, 1, 24);
  memset(data_ptr6, 7, 24);
  mptr1 = mm;
  mptr2 = mm->next_mgr;
  mptr3 = mm->next_mgr->next_mgr;
  mptr4 = mm->next_mgr->next_mgr->next_mgr;

  memmgr_free(&mm, data_ptr1);
  fail_unless(mptr3->ref_count == 2, "ref_count after free should be 2 but is:%d", mm->ref_count);
  memcpy(&size, mptr3->the_mem, sizeof(int));
  fail_unless(size == 0, "size indicator should have been cleared but still exists (%d)", size);
  ptr = (char *)mptr3->the_mem+sizeof(int);
  fail_unless(memcmp(ptr, "\0\0\0\0\0", 5) == 0, "Memory should have been cleared after free but wasn't");
  memmgr_free(&mm, data_ptr5);
  fail_unless(mptr1->next_mgr == mptr3, "Linked list not updated correctly (removed mptr1->next_mgr) %x != %x", mptr1->next_mgr, mptr3);
  fail_unless(mptr3->prev_mgr == mptr1, "Linked list not updated correctly (removed mptr3->prev_mgr)");
  memmgr_free(&mm, data_ptr6);
  fail_unless(mm->current_mgr == mptr4, "current_mgr was updated (and shouldn't have been)");
  memmgr_free(&mm, data_ptr2);
  memmgr_free(&mm, data_ptr3);
  fail_unless(mm->current_mgr == mptr4, "current_mgr was not updated correctly");
  fail_unless(mm->prev_mgr == NULL, "prev_mgr of root was not update correctly");
  memmgr_free(&mm, data_ptr4);
  fail_unless(mm == mptr4, "Final node not set correctly");
  fail_unless(mm->next_mgr == NULL, "There should be no next node memmgr");
  fail_unless(mm->prev_mgr == NULL, "There should be no prev node memmgr");
  }
END_TEST

START_TEST(test_memmgr_find_fail)
  {
  memmgr *mm = NULL;
  init_globals();
  mm = (memmgr *)memmgr_find(NULL, NULL);
  fail_unless(mm == NULL, "Invalid memmgr should generate NULL memmgr response");
  mm = (memmgr *)memmgr_find(&mm, NULL);
  fail_unless(mm == NULL, "Invalid memmgr should generate NULL memmgr response");
  memmgr_init(&mm, 48);
  mm = (memmgr *)memmgr_find(&mm, NULL);
  fail_unless(mm == NULL, "NULL ptr request should generate NULL memmgr response");
  memmgr_destroy(&mm);

  }
END_TEST

START_TEST(test_memmgr_realloc)
  {
  int rc = 0;
  char *ptr = NULL;
  char *data_ptr1 = NULL;
  char *data_ptr2 = NULL;
  char *data_ptr3 = NULL;
  char *data_ptr4 = NULL;
  char *data_ptr5 = NULL;
  char *data_ptr6 = NULL;
  memmgr *mm = NULL;
  init_globals();
  fail_unless(memmgr_realloc(NULL, NULL, 5) == NULL, "NULL memmgr should result in NULL ptr");
  fail_unless(memmgr_realloc(&mm, NULL, 5) == NULL, "NULL memmgr should result in NULL ptr");
  rc = memmgr_init(&mm, 48);
  fail_unless(memmgr_realloc(&mm, NULL, -1) == NULL, "negative size should result in NULL ptr");
  fail_unless(memmgr_realloc(&mm, NULL, 5) != NULL, "NULL ptr with valid size should result in valid ptr");
  memmgr_destroy(&mm);
  rc = memmgr_init(&mm, 48);
  data_ptr1 = (char *)memmgr_calloc(&mm, 1, 11);
  strcpy(data_ptr1, "11111111111");
  ptr = (char *)mm->current_pos;
  data_ptr2 = (char *)memmgr_realloc(&mm, data_ptr1, 4);
  fail_unless(data_ptr1 == data_ptr2, "The same ptr should have returned");
  fail_unless(strcmp(data_ptr1, "1111") == 0, "contents not correct for realloc to smaller '1111' != '%s'", data_ptr1);
  fail_unless(ptr - (char *)mm->current_pos == 7, "This should have adjusted the current_pos ptr");
  data_ptr2 = (char *)memmgr_realloc(&mm, data_ptr1, 10);
  fail_unless(data_ptr1 == data_ptr2, "The same ptr should have returned");
  fail_unless(memcmp(data_ptr1, "1111\0\0\0\0\0\0", 10) == 0, "Memory should have been cleared after free but wasn't");
  memset(data_ptr2, '2', 10);
  fail_unless(mm->the_mem + sizeof(int)+10+1 == (unsigned char *)mm->current_pos, "current_pos ptr is in the wrong place");
  data_ptr3 = (char *)memmgr_calloc(&mm, 1, 5);
  memset(data_ptr3, '3', 5);
  data_ptr4 = (char *)memmgr_realloc(&mm, data_ptr3, 4);
  fail_unless((unsigned char *)mm->current_pos - mm->the_mem == 25, "current_pos not in correct position");
  fail_unless(data_ptr3 == data_ptr4, "The same ptr should have returned");
  fail_unless(strcmp(data_ptr3, "3333") == 0, "contents not correct for realloc to smaller '3333' != '%s'", data_ptr3);
  data_ptr4 = (char *)memmgr_realloc(&mm, data_ptr3, 6);
  fail_unless(data_ptr3 != data_ptr4, "The ptr should have changed");
  fail_unless((unsigned char *)mm->current_pos - mm->the_mem == 36, "current_pos not in correct position");
  memset(data_ptr4, '4', 6);
  data_ptr5 = (char *)memmgr_realloc(&mm, data_ptr4, 20);
  fail_unless(mm->next_mgr != NULL, "There should have been an additional allocator created at this point");
  memset(data_ptr5, '5', 6);
  data_ptr6 = (char *)memmgr_realloc(&mm, data_ptr2, 50);
  fail_unless(data_ptr2 == data_ptr6, "The same ptr should have returned");
  fail_unless(mm->current_pos == NULL, "This should have set the current_pos to NULL in the mgr");
  fail_unless(mm->remaining == 0, "This should have set the remaining to 0 in the mgr");
  fail_unless(mm->alloc_size == 55, "This should have set the alloc_size to 55 in the mgr");

  memmgr_destroy(&mm);
  }
END_TEST

Suite *u_memmgr_suite(void)
  {
  Suite *s = suite_create("u_memmgr_suite methods");
  TCase *tc_core = tcase_create("test_memmgr_init_destroy_one");
  tcase_add_test(tc_core, test_memmgr_init_destroy_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_memmgr_init_fail");
  tcase_add_test(tc_core, test_memmgr_init_fail);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_memmgr_calloc_shuffle_shuffle_extend");
  tcase_add_test(tc_core, test_memmgr_calloc_shuffle_shuffle_extend);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_memmgr_free_remove");
  tcase_add_test(tc_core, test_memmgr_free_remove);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_memmgr_find_fail");
  tcase_add_test(tc_core, test_memmgr_find_fail);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_memmgr_realloc");
  tcase_add_test(tc_core, test_memmgr_realloc);
  suite_add_tcase(s, tc_core);
  return s;
  }

void rundebug()
  {
  int rc = 0;
  char *data_ptr1 = NULL;
  char *data_ptr2 = NULL;
  char *data_ptr3 = NULL;
  char *data_ptr4 = NULL;
  char *data_ptr5 = NULL;
  char *data_ptr6 = NULL;
  memmgr *mm = NULL;
  init_globals();
  rc = memmgr_init(&mm, 48);
  data_ptr1 = (char *)memmgr_calloc(&mm, 1, 11);
  strcpy(data_ptr1, "11111111111");
  data_ptr2 = (char *)memmgr_realloc(&mm, data_ptr1, 4);
  data_ptr2 = (char *)memmgr_realloc(&mm, data_ptr1, 10);
  memset(data_ptr2, '2', 10);
  data_ptr3 = (char *)memmgr_calloc(&mm, 1, 5);
  memset(data_ptr3, '3', 5);
  data_ptr4 = (char *)memmgr_realloc(&mm, data_ptr3, 4);
  data_ptr4 = (char *)memmgr_realloc(&mm, data_ptr3, 6);
  memset(data_ptr4, '4', 6);
  data_ptr5 = (char *)memmgr_realloc(&mm, data_ptr4, 20);
  memset(data_ptr5, '5', 6);
  data_ptr6 = (char *)memmgr_realloc(&mm, data_ptr2, 50);
  memmgr_destroy(&mm);
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
/*  rundebug(); */
  sr = srunner_create(u_memmgr_suite());
  srunner_set_log(sr, "u_memmgr_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
