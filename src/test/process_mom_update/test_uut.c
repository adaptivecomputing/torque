#include "pbs_nodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "pbs_error.h"

int set_note_error(struct pbsnode *np, const char *str);
int restore_note(struct pbsnode *np);

START_TEST(test_set_note_error)
  {
  pbsnode pnode;
  fail_unless(set_note_error(&pnode, "message=ERROR - bob") == PBSE_NONE, "Failed to append");
  fail_unless(pnode.nd_note == "ERROR - bob");

  // Make sure the same error isn't appended twice - the note shouldn't be changed
  fail_unless(set_note_error(&pnode, "message=ERROR - bob") == PBSE_NONE);
  fail_unless(pnode.nd_note == "ERROR - bob");

  pnode.nd_note.clear();

  fail_unless(set_note_error(&pnode, "message=yay") == PBSE_NONE);
  fail_unless(pnode.nd_note == "yay");
  
  fail_unless(set_note_error(&pnode, "message=ERROR - the system is down") == PBSE_NONE);
  fail_unless(pnode.nd_note == "yay - ERROR - the system is down");
  }
END_TEST



START_TEST(test_two)
  {
  struct pbsnode *pnode = new pbsnode();
  fail_unless(restore_note(pnode) == PBSE_NONE);
  fail_unless(pnode->nd_note == "");

  fail_unless(set_note_error(pnode, "message=ERROR - bob") == PBSE_NONE);
  fail_unless(pnode->nd_note == "ERROR - bob");

  fail_unless(restore_note(pnode) == PBSE_NONE);
  fail_unless(pnode->nd_note == "");

  pnode->nd_note = "Yo Dawg, I heard you wanted a note";

  fail_unless(restore_note(pnode) == PBSE_NONE);
  fail_unless(pnode->nd_note == "Yo Dawg, I heard you wanted a note");
  fail_unless(set_note_error(pnode, "message=ERROR Everything's broken") == PBSE_NONE);
  fail_unless(pnode->nd_note == "Yo Dawg, I heard you wanted a note - ERROR Everything's broken");
  fail_unless(restore_note(pnode) == PBSE_NONE);
  fail_unless(pnode->nd_note == "Yo Dawg, I heard you wanted a note");
  }
END_TEST



Suite *process_mom_update_suite(void)
  {
  Suite *s = suite_create("process_mom_update test suite methods");
  TCase *tc_core = tcase_create("test_set_note_error");
  tcase_add_test(tc_core, test_set_note_error);
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
