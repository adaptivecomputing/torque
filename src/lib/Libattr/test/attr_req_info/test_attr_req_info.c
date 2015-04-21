#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "attr_req_info.hpp"
#include "pbs_error.h"

extern int called_log_event;


START_TEST(test_constructor)
  {
  attr_req_info ari;
  int val = 111;
  unsigned int uval = 222;
  int ret;

  /* test min values */
  ret = ari.get_signed_min_limit_value("lprocs", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_unsigned_min_limit_value("memory", uval); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_unsigned_min_limit_value("swap", uval); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_unsigned_min_limit_value("disk", uval); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_min_limit_value("nodes", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_min_limit_value("sockets", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_min_limit_value("numa_chips", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_min_limit_value("cores", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_min_limit_value("threads", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  /* test max values */
  ret = ari.get_signed_max_limit_value("lprocs", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_unsigned_max_limit_value("memory", uval); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_unsigned_max_limit_value("swap", uval); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_unsigned_max_limit_value("disk", uval); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_max_limit_value("nodes", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_max_limit_value("sockets", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_max_limit_value("numa_chips", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_max_limit_value("cores", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_max_limit_value("threads", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);

  /* test default values */
  ret = ari.get_signed_default_limit_value("lprocs", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_unsigned_default_limit_value("memory", uval); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_unsigned_default_limit_value("swap", uval); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_unsigned_default_limit_value("disk", uval); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_default_limit_value("nodes", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_default_limit_value("sockets", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_default_limit_value("numa_chips", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_default_limit_value("cores", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  ret = ari.get_signed_default_limit_value("threads", val); 
  fail_unless(ret == PBSE_NONE);
  fail_unless(val == 0);
  
  }
END_TEST

START_TEST(test_set_limit_values)
  {
  attr_req_info ari;
  int ret;
  int val;
  unsigned int uval;

  ret = ari.set_min_limit_value("lprocs", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_min_limit_value("lprocs", val);
  fail_unless(val == 2);

  ret = ari.set_min_limit_value("memory", "4000Kb");
  fail_unless(ret == 0);
  ret = ari.get_unsigned_min_limit_value("memory", uval);
  fail_unless(uval == 4000);

  ret = ari.set_min_limit_value("swap", "4000Kb");
  fail_unless(ret == 0);
  ret = ari.get_unsigned_min_limit_value("swap", uval);
  fail_unless(uval == 4000);

  ret = ari.set_min_limit_value("disk", "4000Kb");
  fail_unless(ret == 0);
  ret = ari.get_unsigned_min_limit_value("disk", uval);
  fail_unless(uval == 4000);

  ret = ari.set_min_limit_value("nodes", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_min_limit_value("nodes", val);
  fail_unless(val == 2);

  ret = ari.set_min_limit_value("sockets", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_min_limit_value("sockets", val);
  fail_unless(val == 2);

  ret = ari.set_min_limit_value("numa_chips", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_min_limit_value("numa_chips", val);
  fail_unless(val == 2);

  ret = ari.set_min_limit_value("cores", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_min_limit_value("cores", val);
  fail_unless(val == 2);

  ret = ari.set_min_limit_value("threads", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_min_limit_value("threads", val);
  fail_unless(val == 2);

  /* Test max values */
  ret = ari.set_max_limit_value("lprocs", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_max_limit_value("lprocs", val);
  fail_unless(val == 2);

  ret = ari.set_max_limit_value("memory", "4000Kb");
  fail_unless(ret == 0);
  ret = ari.get_unsigned_max_limit_value("memory", uval);
  fail_unless(uval == 4000);

  ret = ari.set_max_limit_value("swap", "4000Kb");
  fail_unless(ret == 0);
  ret = ari.get_unsigned_max_limit_value("swap", uval);
  fail_unless(uval == 4000);

  ret = ari.set_max_limit_value("disk", "4000Kb");
  fail_unless(ret == 0);
  ret = ari.get_unsigned_max_limit_value("disk", uval);
  fail_unless(uval == 4000);

  ret = ari.set_max_limit_value("nodes", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_max_limit_value("nodes", val);
  fail_unless(val == 2);

  ret = ari.set_max_limit_value("sockets", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_max_limit_value("sockets", val);
  fail_unless(val == 2);

  ret = ari.set_max_limit_value("numa_chips", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_max_limit_value("numa_chips", val);
  fail_unless(val == 2);

  ret = ari.set_max_limit_value("cores", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_max_limit_value("cores", val);
  fail_unless(val == 2);

  ret = ari.set_max_limit_value("threads", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_max_limit_value("threads", val);
  fail_unless(val == 2);

  /* Test default values */
  ret = ari.set_default_limit_value("lprocs", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_default_limit_value("lprocs", val);
  fail_unless(val == 2);

  ret = ari.set_default_limit_value("memory", "4000Kb");
  fail_unless(ret == 0);
  ret = ari.get_unsigned_default_limit_value("memory", uval);
  fail_unless(uval == 4000);

  ret = ari.set_default_limit_value("swap", "4000Kb");
  fail_unless(ret == 0);
  ret = ari.get_unsigned_default_limit_value("swap", uval);
  fail_unless(uval == 4000);

  ret = ari.set_default_limit_value("disk", "4000Kb");
  fail_unless(ret == 0);
  ret = ari.get_unsigned_default_limit_value("disk", uval);
  fail_unless(uval == 4000);

  ret = ari.set_default_limit_value("nodes", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_default_limit_value("nodes", val);
  fail_unless(val == 2);

  ret = ari.set_default_limit_value("sockets", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_default_limit_value("sockets", val);
  fail_unless(val == 2);

  ret = ari.set_default_limit_value("numa_chips", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_default_limit_value("numa_chips", val);
  fail_unless(val == 2);

  ret = ari.set_default_limit_value("cores", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_default_limit_value("cores", val);
  fail_unless(val == 2);

  ret = ari.set_default_limit_value("threads", "2");
  fail_unless(ret == 0);
  ret = ari.get_signed_default_limit_value("threads", val);
  fail_unless(val == 2);

  std::vector<std::string> names;
  std::vector<std::string> values;

  ret = ari.get_min_values(names, values);
  fail_unless(ret==PBSE_NONE);

  ret = ari.check_min_values(names, values);
  fail_unless(ret==PBSE_NONE);

  names.clear();
  values.clear();

  ret = ari.get_max_values(names, values);
  fail_unless(ret==PBSE_NONE);

  ret = ari.check_max_values(names, values);
  fail_unless(ret==PBSE_NONE);

  ret = ari.get_default_values(names, values);
  fail_unless(ret==PBSE_NONE);

  names.clear();
  values.clear();

  std::string rescn;
  std::string rescval;

  rescn = "lprocs";
  rescval = "4";
  names.push_back(rescn);
  values.push_back(rescval);
  ret = ari.check_max_values(names, values);
  fail_unless(ret == PBSE_EXLIMIT);

  names.clear();
  values.clear();
  rescn = "lprocs";
  rescval = "1";
  names.push_back(rescn);
  values.push_back(rescval);
  ret = ari.check_min_values(names, values);
  fail_unless(ret == PBSE_EXLIMIT);


  }
END_TEST


START_TEST(test_check_limit_values)
  {
  std::vector<std::string> names, values;
  attr_req_info ari;
  int ret;

  /* set the min limits first */
  ret = ari.set_min_limit_value("lprocs", "2");
  fail_unless(ret == 0);

  ret = ari.set_min_limit_value("memory", "4000Kb");
  fail_unless(ret == 0);

  ret = ari.set_min_limit_value("swap", "4000Kb");
  fail_unless(ret == 0);

  ret = ari.set_min_limit_value("disk", "4000Kb");
  fail_unless(ret == 0);

  ret = ari.set_min_limit_value("nodes", "2");
  fail_unless(ret == 0);

  ret = ari.set_min_limit_value("sockets", "2");
  fail_unless(ret == 0);

  ret = ari.set_min_limit_value("numa_chips", "2");
  fail_unless(ret == 0);

  names.push_back("lprocs");
  values.push_back("1");
  ret = ari.check_min_values(names, values);
  fail_unless(ret== PBSE_EXLIMIT);

  names.clear();
  values.clear();

  names.push_back("lprocs");
  values.push_back("3");
  ret = ari.check_min_values(names, values);
  fail_unless(ret== PBSE_NONE);


  ret = ari.set_min_limit_value("cores", "2");
  fail_unless(ret == 0);

  ret = ari.set_min_limit_value("threads", "2");
  fail_unless(ret == 0);

  /* set max limits */
  ret = ari.set_max_limit_value("lprocs", "3");
  fail_unless(ret == 0);

  ret = ari.set_max_limit_value("memory", "6000Kb");
  fail_unless(ret == 0);

  ret = ari.set_max_limit_value("swap", "8000Kb");
  fail_unless(ret == 0);

  ret = ari.set_max_limit_value("disk", "10000Kb");
  fail_unless(ret == 0);

  ret = ari.set_max_limit_value("nodes", "4");
  fail_unless(ret == 0);

  ret = ari.set_max_limit_value("sockets", "4");
  fail_unless(ret == 0);

  ret = ari.set_max_limit_value("numa_chips", "4");
  fail_unless(ret == 0);

  ret = ari.set_max_limit_value("cores", "8");
  fail_unless(ret == 0);

  ret = ari.set_max_limit_value("threads", "16");
  fail_unless(ret == 0);


 }
END_TEST


Suite *complete_req_suite(void)
  {
  Suite *s = suite_create("attr_req_info test suite methods");
  TCase *tc_core = tcase_create("test_constructor");
  tcase_add_test(tc_core, test_constructor);
  suite_add_tcase(s, tc_core);
  

  tc_core = tcase_create("test_set_limit_values");
  tcase_add_test(tc_core, test_set_limit_values);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_check_limit_values");
  tcase_add_test(tc_core, test_check_limit_values);
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
  sr = srunner_create(complete_req_suite());
  srunner_set_log(sr, "complete_req_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
