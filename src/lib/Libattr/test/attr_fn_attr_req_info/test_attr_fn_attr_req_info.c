#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>

#include "attr_req_info.hpp"
#include "attribute.h"
#include "pbs_error.h"

int overwrite_attr_req_info(pbs_attribute *, pbs_attribute *);

START_TEST(test_free_attr_req_info)
  {
  pbs_attribute  pattr;
  attr_req_info  *cr = new attr_req_info();

  memset(&pattr, 0, sizeof(pattr));

  pattr.at_flags = ATR_VFLAG_SET;
  pattr.at_val.at_ptr = cr;

  free_attr_req_info(&pattr);
  fail_unless(pattr.at_val.at_ptr == NULL);
  fail_unless(pattr.at_flags == 0);

  }
END_TEST



START_TEST(test_decode_encode_attr_req_info)
  {
  pbs_attribute  pattr_max;
  pbs_attribute  pattr_min;
  pbs_attribute  pattr_default;
  tlist_head     phead;

  const char    *resc_names[] = {"lprocs", "memory", "swap"};
  const char    *values[] = { "5", "1000000kb", "200000kb"};

  memset(&pattr_max, 0, sizeof(pattr_max));
  memset(&pattr_min, 0, sizeof(pattr_min));
  memset(&pattr_default, 0, sizeof(pattr_default));


  // null should fail
  fail_unless(decode_attr_req_info(NULL, NULL, NULL, NULL, 1) == PBSE_BAD_PARAMETER);
  fail_unless(decode_attr_req_info(&pattr_max, NULL, NULL, NULL, 1) == PBSE_BAD_PARAMETER);
  // null resource name should fail
  fail_unless(decode_attr_req_info(&pattr_max, NULL, NULL, "bob", 1) == PBSE_BAD_PARAMETER);
  // random words should fail
  fail_unless(decode_attr_req_info(&pattr_max, NULL, "hippo", "bob", 1) == PBSE_BAD_PARAMETER);
  fail_unless(decode_attr_req_info(&pattr_max, "fred", NULL, "bob", 1) == PBSE_BAD_PARAMETER);
  fail_unless(decode_attr_req_info(&pattr_max, "fred", "bill", "bob", 1) == PBSE_BAD_PARAMETER);

  // should still be NULL
  fail_unless(pattr_max.at_val.at_ptr == NULL);

  // these should work
  fail_unless(decode_attr_req_info(&pattr_max, "req_information_max", resc_names[0], values[0], 1) == PBSE_NONE);
  fail_unless((pattr_max.at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pattr_max.at_val.at_ptr != 0);

  attr_req_info *cr = (attr_req_info *)pattr_max.at_val.at_ptr;
  fail_unless(cr != NULL);

  fail_unless(decode_attr_req_info(&pattr_max, "req_information_max", resc_names[1], values[1], 1) == PBSE_NONE);
  fail_unless((pattr_max.at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pattr_max.at_val.at_ptr != 0);

  fail_unless(decode_attr_req_info(&pattr_max, "req_information_max", resc_names[2], values[2], 1) == PBSE_NONE);
  fail_unless((pattr_max.at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pattr_max.at_val.at_ptr != 0);

  /* Thest the minimum decode */
  fail_unless(decode_attr_req_info(&pattr_min, "req_information_min", "lprocs", "2", 1) == PBSE_NONE);
  fail_unless((pattr_min.at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pattr_min.at_val.at_ptr != 0);
  /* Thest the default  decode */
  fail_unless(decode_attr_req_info(&pattr_default, "req_information_default", "lprocs", "2", 1) == PBSE_NONE);
  fail_unless((pattr_default.at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pattr_default.at_val.at_ptr != 0);



  /* We can pass a null pbs_attribute into ecnode_attr_req_info and return PBSE_NONE */
  fail_unless(encode_attr_req_info(NULL, NULL, NULL, NULL, 0, 0) == PBSE_NONE);
  /* name, rescn, and val need to be set in order for this to work */
  fail_unless(encode_attr_req_info(&pattr_max, NULL, NULL, NULL, 0, 0) == PBSE_BAD_PARAMETER);
  fail_unless(encode_attr_req_info(&pattr_max, &phead, "george", "lprocs", ATR_ENCODE_CLIENT, ATR_DFLAG_RDACC) == PBSE_BAD_PARAMETER);

  fail_unless(encode_attr_req_info(&pattr_max, &phead, "req_information_max", "lprocs", ATR_ENCODE_CLIENT, ATR_DFLAG_RDACC) == PBSE_NONE);
  fail_unless(encode_attr_req_info(&pattr_min, &phead, "req_information_min", "lprocs", ATR_ENCODE_CLIENT, ATR_DFLAG_RDACC) == PBSE_NONE);
  fail_unless(encode_attr_req_info(&pattr_default, &phead, "req_information_default", "lprocs", ATR_ENCODE_CLIENT, ATR_DFLAG_RDACC) == PBSE_NONE);
  }
END_TEST


START_TEST(test_set_attr_req_info)
  {
  pbs_attribute  pattr_max;
  pbs_attribute  pattr_min;
  pbs_attribute  pattr_default;
  pbs_attribute  new_pattr;

  const char    *resc_names[] = {"lprocs", "memory", "swap"};
  const char    *values[] = { "5", "1000000kb", "200000kb"};

  memset(&pattr_max, 0, sizeof(pattr_max));
  memset(&pattr_min, 0, sizeof(pattr_min));
  memset(&pattr_default, 0, sizeof(pattr_default));
  memset(&new_pattr, 0, sizeof(new_pattr));
  if (new_pattr.at_val.at_ptr == 0)
    {;}
  //
  // these should work
  fail_unless(decode_attr_req_info(&pattr_max, "req_information_max", resc_names[0], values[0], 1) == PBSE_NONE);
  fail_unless((pattr_max.at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pattr_max.at_val.at_ptr != 0);

  attr_req_info *cr = (attr_req_info *)pattr_max.at_val.at_ptr;
  fail_unless(cr != NULL);

  fail_unless(decode_attr_req_info(&pattr_max, "req_information_max", resc_names[1], values[1], 1) == PBSE_NONE);
  fail_unless((pattr_max.at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pattr_max.at_val.at_ptr != 0);

  fail_unless(decode_attr_req_info(&pattr_max, "req_information_max", resc_names[2], values[2], 1) == PBSE_NONE);
  fail_unless((pattr_max.at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pattr_max.at_val.at_ptr != 0);

  /* Thest the minimum decode */
  fail_unless(decode_attr_req_info(&pattr_min, "req_information_min", "lprocs", "2", 1) == PBSE_NONE);
  fail_unless((pattr_min.at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pattr_min.at_val.at_ptr != 0);
  /* Thest the default  decode */
  fail_unless(decode_attr_req_info(&pattr_default, "req_information_default", "lprocs", "2", 1) == PBSE_NONE);
  fail_unless((pattr_default.at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pattr_default.at_val.at_ptr != 0);

  /* Test some error cases */
  fail_unless(set_attr_req_info(NULL, &new_pattr, SET) == PBSE_BAD_PARAMETER);
  fail_unless(set_attr_req_info(&pattr_max, NULL, SET) == PBSE_BAD_PARAMETER);

  fail_unless(set_attr_req_info(&pattr_max, &new_pattr, INCR) == PBSE_INTERNAL);

  /* test the success case */
  fail_unless(set_attr_req_info(&new_pattr, &pattr_max, SET) == PBSE_NONE);
  fail_unless(new_pattr.at_val.at_ptr != NULL);

  int value, orig_value;
  attr_req_info *ari = (attr_req_info *)new_pattr.at_val.at_ptr;
  fail_unless(ari->get_signed_max_limit_value(resc_names[0], value) == PBSE_NONE);
  orig_value = atoi(values[0]);
  fail_unless(orig_value == value);

  fail_unless(set_attr_req_info(&new_pattr, NULL, UNSET) == PBSE_NONE);
  fail_unless(new_pattr.at_val.at_ptr == NULL);

  fail_unless(set_attr_req_info(&new_pattr, &pattr_min, SET) == PBSE_NONE);
  fail_unless(new_pattr.at_val.at_ptr != NULL);

  fail_unless(set_attr_req_info(&new_pattr, NULL, UNSET) == PBSE_NONE);
  fail_unless(new_pattr.at_val.at_ptr == NULL);

  fail_unless(set_attr_req_info(&new_pattr, &pattr_default, SET) == PBSE_NONE);
  fail_unless(new_pattr.at_val.at_ptr != NULL);

  fail_unless(set_attr_req_info(&new_pattr, NULL, UNSET) == PBSE_NONE);
  fail_unless(new_pattr.at_val.at_ptr == NULL);

  /* Failure case. attr flag set but at_ptr NULL */
  memset(&pattr_max, 0, sizeof(pattr_max));
  pattr_max.at_flags = ATR_VFLAG_SET;
  fail_unless(set_attr_req_info(&pattr_max, &new_pattr, SET) == PBSE_NONE);
  fail_unless(pattr_max.at_flags == 0);

  memset(&pattr_max, 0, sizeof(pattr_max));
  memset(&new_pattr, 0, sizeof(pattr_max));
  new_pattr.at_flags = ATR_VFLAG_SET;
  fail_unless(set_attr_req_info(&pattr_max, &new_pattr, SET) == PBSE_NONE);
  fail_unless(new_pattr.at_flags == 0);
  
 }
END_TEST

START_TEST(test_overwrite_attr_req_info)
  {
  pbs_attribute  old_attr;
  pbs_attribute  new_attr;

  memset(&old_attr, 0, sizeof(old_attr));
  memset(&new_attr, 0, sizeof(new_attr));

  fail_unless(decode_attr_req_info(&new_attr, "req_information_max", "lprocs", "4",  1) == PBSE_NONE);
  overwrite_attr_req_info(&old_attr, &new_attr);
  fail_unless(old_attr.at_flags != 0);
  fail_unless(old_attr.at_val.at_ptr != NULL);
  /* do it again */
  overwrite_attr_req_info(&old_attr, &new_attr);
  fail_unless(old_attr.at_flags != 0);
  fail_unless(old_attr.at_val.at_ptr != NULL);

  }
END_TEST



Suite *attr_fn_attr_req_info(void)
  {
  Suite *s = suite_create("attr_fn_attr_req_info test suite methods");
  TCase *tc_core = tcase_create("test_free_attr_req_info");
  tcase_add_test(tc_core, test_free_attr_req_info);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_decode_encode_attr_req_info");
  tcase_add_test(tc_core, test_decode_encode_attr_req_info);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_overwrite_attr_req_info");
  tcase_add_test(tc_core, test_overwrite_attr_req_info);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_set_attr_req_info");
  tcase_add_test(tc_core, test_set_attr_req_info);
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
  sr = srunner_create(attr_fn_attr_req_info());
  srunner_set_log(sr, "attr_fn_attr_req_info.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
