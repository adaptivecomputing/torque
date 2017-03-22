#include <stdio.h>
#include <stdlib.h>

#include "pbs_error.h"

#include <pbs_config.h>
#include "pbs_nodes.h"
#include "machine.hpp"
#include <check.h>

int set_note_error(struct pbsnode *np, const char *str);
int restore_note(struct pbsnode *np);

#ifdef PENABLE_LINUX_CGROUPS
void update_layout_if_needed(pbsnode *pnode, const std::string &layout, bool force);

extern int event_logged;


START_TEST(test_update_layout_if_needed)
  {
  struct pbsnode *pnode = (struct pbsnode *)calloc(1, sizeof(pbsnode));
  const char *sans_threads = "\"node\":{\"socket\":{\"os_index\":0,\"numanode\":{\"os_index\":0,\"cores\":\"0-3\",\"threads\":\"\",\"mem\"=0},\"numanode\":{\"os_index\":1,\"cores\":\"4-7\",\"threads\":\"\",\"mem\"=1}},\"socket\":{\"os_index\":1,\"numanode\":{\"os_index\":2,\"cores\":\"8-11\",\"threads\":\"\",\"mem\"=2},\"numanode\":{\"os_index\":3,\"cores\":\"12-15\",\"threads\":\"\",\"mem\"=3}}}";

  const char *with_threads = "\"node\":{\"socket\":{\"os_index\":0,\"numanode\":{\"os_index\":0,\"cores\":\"0-3\",\"threads\":\"16-19\",\"mem\"=0},\"numanode\":{\"os_index\":1,\"cores\":\"4-7\",\"threads\":\"20-23\",\"mem\"=1}},\"socket\":{\"os_index\":1,\"numanode\":{\"os_index\":2,\"cores\":\"8-11\",\"threads\":\"24-27\",\"mem\"=2},\"numanode\":{\"os_index\":3,\"cores\":\"12-15\",\"threads\":\"28-31\",\"mem\"=3}}}";

  event_logged = 0;
  pnode->nd_layout = NULL;
  update_layout_if_needed(pnode, sans_threads, false);

  fail_unless(event_logged == 0);
  fail_unless(pnode->nd_layout != NULL);
  fail_unless(pnode->nd_layout->getTotalThreads() == 16);
    
  for (int i = 0; i < 16; i++)
    pnode->nd_slots.add_execution_slot();

  // Calling again without changing the number of slots should do nothing
  update_layout_if_needed(pnode, sans_threads, false);
  update_layout_if_needed(pnode, sans_threads, false);
  update_layout_if_needed(pnode, sans_threads, false);
  update_layout_if_needed(pnode, sans_threads, false);
  fail_unless(event_logged == 0);

  // Simulate that threads were turned on in this machine, therefore increasing the number of
  // execution slots
  for (int i = 0; i < 16; i++)
    pnode->nd_slots.add_execution_slot();

  update_layout_if_needed(pnode, with_threads, false);
  
  fail_unless(event_logged == 1);
  fail_unless(pnode->nd_layout->getTotalThreads() == 32);

  // Calling again without changing the number of slots should do nothing
  update_layout_if_needed(pnode, with_threads, false);
  update_layout_if_needed(pnode, with_threads, false);
  update_layout_if_needed(pnode, with_threads, false);
  update_layout_if_needed(pnode, with_threads, false);
  fail_unless(event_logged == 1);

  // Make sure that changing the number of gpus on the node triggers an event
  pnode->nd_ngpus = 1;
  update_layout_if_needed(pnode, with_threads, false);
  fail_unless(event_logged == 2, "event logged: %d", event_logged);
  }
END_TEST
#endif


START_TEST(test_set_note_error)
  {
  struct pbsnode *pnode = (struct pbsnode *)calloc(1, sizeof(pbsnode));
  fail_unless(set_note_error(pnode, "message=ERROR - bob") == PBSE_NONE, "Failed to append");
  fail_unless(!strcmp(pnode->nd_note, "ERROR - bob"));

  // Make sure the same error isn't appended twice - the note shouldn't be changed
  fail_unless(set_note_error(pnode, "message=ERROR - bob") == PBSE_NONE);
  fail_unless(!strcmp(pnode->nd_note, "ERROR - bob"));

  free(pnode->nd_note);
  pnode->nd_note = NULL;

  fail_unless(set_note_error(pnode, "message=yay") == PBSE_NONE);
  fail_unless(!strcmp(pnode->nd_note, "yay"));
  
  fail_unless(set_note_error(pnode, "message=ERROR - the system is down") == PBSE_NONE);
  fail_unless(!strcmp(pnode->nd_note, "yay - ERROR - the system is down"));

  // make sure newline stripped 
  fail_unless(set_note_error(pnode, "message=ERROR - the system is down\n") == PBSE_NONE);
  fail_unless(strcmp(pnode->nd_note, "yay - ERROR - the system is down") == 0);
  }
END_TEST



START_TEST(test_two)
  {
  struct pbsnode *pnode = (struct pbsnode *)calloc(1, sizeof(pbsnode));
  fail_unless(restore_note(pnode) == PBSE_NONE);
  fail_unless(pnode->nd_note == NULL);

  fail_unless(set_note_error(pnode, "message=ERROR - bob") == PBSE_NONE);
  fail_unless(!strcmp(pnode->nd_note, "ERROR - bob"));

  fail_unless(restore_note(pnode) == PBSE_NONE);
  fail_unless(pnode->nd_note == NULL);

  free(pnode->nd_note);
  pnode->nd_note = strdup("Yo Dawg, I heard you wanted a note");

  fail_unless(restore_note(pnode) == PBSE_NONE);
  fail_unless(!strcmp(pnode->nd_note, "Yo Dawg, I heard you wanted a note"));
  fail_unless(set_note_error(pnode, "message=ERROR Everything's broken") == PBSE_NONE);
  fail_unless(!strcmp(pnode->nd_note, "Yo Dawg, I heard you wanted a note - ERROR Everything's broken"));
  fail_unless(restore_note(pnode) == PBSE_NONE);
  fail_unless(!strcmp(pnode->nd_note, "Yo Dawg, I heard you wanted a note"));
  }
END_TEST



Suite *process_mom_update_suite(void)
  {
  Suite *s = suite_create("process_mom_update test suite methods");
  TCase *tc_core = tcase_create("test_set_note_error");
  tcase_add_test(tc_core, test_set_note_error);
#ifdef PENABLE_LINUX_CGROUPS
  tcase_add_test(tc_core, test_update_layout_if_needed);
#endif
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
  suite_add_tcase(s, tc_core);
  
  return(s);
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(process_mom_update_suite());
  srunner_set_log(sr, "process_mom_update_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
