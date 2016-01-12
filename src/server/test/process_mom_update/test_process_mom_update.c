#include "pbs_nodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>

#include "pbs_error.h"

int set_note_error(struct pbsnode *, const char *);
int restore_note(struct pbsnode *);

START_TEST(test_set_note_error)
  {
  struct pbsnode node;
  char *s;

  memset((void *)&node, 0, sizeof(node));
  //  newline in string
  s = strdup("message=foo\n");

  // confirm return code
  fail_unless(set_note_error(&node, s) == PBSE_NONE);

  // make sure message comes back with first 8 chars stripped and no newline
  fail_unless(strcmp(node.nd_note, "foo") == 0);

  // clear things for new test
  memset((void *)&node, 0, sizeof(node));
  //  no newline
  s = strdup("message=bar");

  // confirm return code
  fail_unless(set_note_error(&node, s) == PBSE_NONE);

  // make sure message comes back with first 8 chars stripped
  fail_unless(strcmp(node.nd_note, "bar") == 0);
  }
END_TEST




START_TEST(test_restore_note)
  {
  struct pbsnode node;

  memset((void *)&node, 0, sizeof(node));
  node.nd_note = strdup("ERROR something bad happened");

  // confirm return code
  fail_unless(restore_note(&node) == PBSE_NONE);

  // note should be clear (from first instance of "ERROR" and beyond should be stripped)
  fail_unless(node.nd_note == NULL);

  // clear things for new test
  memset((void *)&node, 0, sizeof(node));
  node.nd_note = strdup("leading stuff here - ERROR some other bad stuff happened");

  // confirm return code
  fail_unless(restore_note(&node) == PBSE_NONE);

  // from first instance of "ERROR" and beyond should be stripped
  fail_unless(strcmp(node.nd_note, "leading stuff here") == 0);
  }
END_TEST




Suite *process_mom_update_suite(void)
  {
  Suite *s = suite_create("process_mom_update test suite methods");
  TCase *tc_core = tcase_create("test_set_note_error");
  tcase_add_test(tc_core, test_set_note_error);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_restore_note");
  tcase_add_test(tc_core, test_restore_note);
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
