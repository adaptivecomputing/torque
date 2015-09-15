#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "id_map.hpp"


START_TEST(test_constructor)
  {
  id_map im;

  // should be empty to start
  fail_unless(im.get_name(0) == NULL);
  fail_unless(im.get_id("napali") == -1);
  }
END_TEST




START_TEST(test_adding)
  {
  id_map im;
  int    id;

  const char *n1 = "napali";
  id = im.get_new_id(n1);
  fail_unless(im.get_id(n1) == id);
  fail_unless(!strcmp(n1, im.get_name(id)));

  const char *n2 = "waimea";
  id = im.get_new_id(n2);
  fail_unless(im.get_id(n2) == id);
  fail_unless(!strcmp(n2, im.get_name(id)));

  const char *n3 = "lihue";
  id = im.get_new_id(n3);
  fail_unless(im.get_id(n3) == id);
  fail_unless(!strcmp(n3, im.get_name(id)));

  const char *n4 = "wailua";
  id = im.get_new_id(n4);
  fail_unless(im.get_id(n4) == id);
  fail_unless(!strcmp(n4, im.get_name(id)));
  }
END_TEST




Suite *id_map_suite(void)
  {
  Suite *s = suite_create("id_map test suite methods");
  TCase *tc_core = tcase_create("test_constructor");
  tcase_add_test(tc_core, test_constructor);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_adding");
  tcase_add_test(tc_core, test_adding);
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
  sr = srunner_create(id_map_suite());
  srunner_set_log(sr, "id_map_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
