#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include <vector>

void translate_range_string_to_vector(const char *range_string, std::vector<int> &indices);


START_TEST(test_translate_range_string_to_vector)
  {
  std::vector<int> indices;

  translate_range_string_to_vector("1\n", indices);
  fail_unless(indices.size() == 1);
  fail_unless(indices[0] == 1);

  indices.clear();
  translate_range_string_to_vector("0-1\n", indices);
  fail_unless(indices.size() == 2);
  fail_unless(indices[0] == 0);
  fail_unless(indices[1] == 1);

  indices.clear();
  translate_range_string_to_vector("1-4", indices);
  fail_unless(indices.size() == 4);
  fail_unless(indices[0] == 1);
  fail_unless(indices[1] == 2);
  fail_unless(indices[2] == 3);
  fail_unless(indices[3] == 4);
  
  indices.clear();
  translate_range_string_to_vector("0-2,6-8", indices);
  fail_unless(indices.size() == 6);
  fail_unless(indices[0] == 0);
  fail_unless(indices[1] == 1);
  fail_unless(indices[2] == 2);
  fail_unless(indices[3] == 6);
  fail_unless(indices[4] == 7);
  fail_unless(indices[5] == 8);
  }
END_TEST




START_TEST(test_two)
  {
  }
END_TEST




Suite *u_misc_suite(void)
  {
  Suite *s = suite_create("u_misc test suite methods");
  TCase *tc_core = tcase_create("test_translate_range_string_to_vector");
  tcase_add_test(tc_core, test_translate_range_string_to_vector);
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
  sr = srunner_create(u_misc_suite());
  srunner_set_log(sr, "u_misc_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
