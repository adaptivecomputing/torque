#include "machine.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include <hwloc.h>


START_TEST(test_displayAsString)
  {
  PCI_Device p;
  p.setName("k80");
  p.set_type(GPU);
  p.setId(0);
  std::stringstream out;

  p.displayAsString(out);
  fail_unless(out.str() == "      GPU 0 k80\n", out.str().c_str());
  }
END_TEST


START_TEST(test_basics)
  {
  PCI_Device p;
  p.setId(1);

  fail_unless(p.get_id() == 1);
  fail_unless(p.get_type() == -1);
  fail_unless(p.is_busy() == false);
  p.set_state(true);
  fail_unless(p.is_busy() == true);
  p.set_type(GPU);
  fail_unless(p.get_type() == GPU);
  }
END_TEST


Suite *numa_pci_device_suite(void)
  {
  Suite *s = suite_create("numa_pci_device test suite methods");
  TCase *tc_core = tcase_create("test_displayAsString");
  tcase_add_test(tc_core, test_displayAsString);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_basics");
  tcase_add_test(tc_core, test_basics);
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
  sr = srunner_create(numa_pci_device_suite());
  srunner_set_log(sr, "numa_pci_device_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
