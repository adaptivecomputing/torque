#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>

#include "complete_req.hpp"
#include "attribute.h"
#include "pbs_error.h"

START_TEST(test_free_complete_req)
  {
  pbs_attribute  pattr;
  complete_req  *cr = new complete_req();

  memset(&pattr, 0, sizeof(pattr));

  pattr.at_flags = ATR_VFLAG_SET;
  pattr.at_val.at_ptr = cr;

  free_complete_req(&pattr);
  fail_unless(pattr.at_val.at_ptr == NULL);
  fail_unless(pattr.at_flags == 0);

  // make sure we don't segfault if we call this twice
  free_complete_req(&pattr);
  }
END_TEST



START_TEST(test_decode_encode_complete_req)
  {
  pbs_attribute  pattr;
  const char    *val = "   req[0]\ntask count: 6\nlprocs: all\n thread usage policy: use threads\nplacement type: place numa\n   req[1]\ntask count: 8\nlprocs: 4\n thread usage policy: use cores\nplacement type: place socket";
  const char    *val2 = "   req[0]\ntask count: 2\nlprocs: all\n thread usage policy: use threads\nplacement type: place numa\n";

  memset(&pattr, 0, sizeof(pattr));

  // null should fail
  fail_unless(decode_complete_req(&pattr, NULL, NULL, NULL, 1) == PBSE_BAD_PARAMETER);
  // not a toString() output should fail
  fail_unless(decode_complete_req(&pattr, NULL, NULL, "bob", 1) == PBSE_BAD_PARAMETER);

  // should still be NULL
  fail_unless(pattr.at_val.at_ptr == NULL);

  // these should work
  fail_unless(decode_complete_req(&pattr, NULL, NULL, val, 1) == PBSE_NONE);
  fail_unless((pattr.at_flags & ATR_VFLAG_SET) != 0);

  complete_req *cr = (complete_req *)pattr.at_val.at_ptr;
  fail_unless(cr != NULL);

  std::string debug;
  cr->toString(debug);

  fail_unless(cr->req_count() == 2, "count: %d\n%s", cr->req_count(), debug.c_str());
  fail_unless(decode_complete_req(&pattr, NULL, NULL, val2, 1) == PBSE_NONE);

  cr = (complete_req *)pattr.at_val.at_ptr;
  fail_unless(cr != NULL);

  // make sure we correctly overwrote the value of the pointer
  fail_unless(cr->req_count() == 1);

  fail_unless(encode_complete_req(NULL, NULL, NULL, NULL, 0, 0) == PBSE_NONE);
  fail_unless(encode_complete_req(&pattr, NULL, NULL, NULL, 0, 0) == PBSE_NONE);

  extern std::string encoded;
  std::string out;
  cr->toString(out);
  fail_unless(out == encoded);
  pattr.at_val.at_ptr = NULL;
  encoded.clear();
  fail_unless(encode_complete_req(&pattr, NULL, NULL, NULL, 0, 0) == PBSE_NONE);
  fail_unless(encoded.size() == 0);
  }
END_TEST


START_TEST(test_set_complete_req)
  {
  pbs_attribute  pattr;
  pbs_attribute  pattr2;
  const char    *val = "   req[0]\ntask count: 6\nlprocs: all\n thread usage policy: use threads\nplacement type: place numa\n   req[1]\ntask count: 8\nlprocs: 4\n thread usage policy: use cores\nplacement type: place socket";
  const char    *val2 = "   req[0]\ntask count: 2\nlprocs: all\n thread usage policy: use threads\nplacement type: place numa\n";

  memset(&pattr, 0, sizeof(pattr));
  memset(&pattr2, 0, sizeof(pattr2));
  fail_unless(decode_complete_req(&pattr, NULL, NULL, val, 1) == PBSE_NONE);

  // DECR is unsupported
  fail_unless(set_complete_req(&pattr2, &pattr, DECR) == PBSE_INTERNAL);

  // INCR should work for an empty destination attribute
  fail_unless(set_complete_req(&pattr2, &pattr, INCR) == PBSE_NONE);

  // INCR is only allowed if the destination attribute has no value
  fail_unless(set_complete_req(&pattr2, &pattr, INCR) == PBSE_INTERNAL);
  
  complete_req *cr2 = (complete_req *)pattr2.at_val.at_ptr;
  complete_req *cr1 = (complete_req *)pattr.at_val.at_ptr;

  fail_unless(cr2->req_count() == cr1->req_count());
  fail_unless(decode_complete_req(&pattr2, NULL, NULL, val2, 1) == PBSE_NONE);

  // make sure they're different now
  cr2 = (complete_req *)pattr2.at_val.at_ptr;
  fail_unless(cr2->req_count() != cr1->req_count());
  fail_unless(set_complete_req(&pattr2, &pattr, SET) == PBSE_NONE);
  
  // they should be the same again
  cr2 = (complete_req *)pattr2.at_val.at_ptr;
  fail_unless(cr2->req_count() == cr1->req_count());

  fail_unless(set_complete_req(NULL, &pattr, SET) == PBSE_BAD_PARAMETER);
  fail_unless(set_complete_req(&pattr2, &pattr, UNSET) == PBSE_NONE);
  fail_unless(pattr2.at_val.at_ptr == NULL);
  fail_unless((pattr2.at_flags & ATR_VFLAG_SET) == 0);
  }
END_TEST



Suite *attr_fn_complete_req_suite(void)
  {
  Suite *s = suite_create("attr_fn_complete_req test suite methods");
  TCase *tc_core = tcase_create("test_free_complete_req");
  tcase_add_test(tc_core, test_free_complete_req);
  tcase_add_test(tc_core, test_set_complete_req);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_decode_encode_complete_req");
  tcase_add_test(tc_core, test_decode_encode_complete_req);
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
  sr = srunner_create(attr_fn_complete_req_suite());
  srunner_set_log(sr, "attr_fn_complete_req_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
