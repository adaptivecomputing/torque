#include "license_pbs.h" /* See here for the software license */
#include "mom_mach.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <fstream>

#include "sys_file.hpp"

#include "pbs_error.h"

class test_class : public sys_file
{
  private:
    std::string base_path;

  public:
    bool start_test(std::string path)
    {
      base_path = path;

      std::string number_path = path + "/nums";
      std::ofstream file(number_path.c_str(),std::ios::out|std::ios::trunc);
      if(!file.is_open())
      {
        return false;
      }
      file << "1 3 5 6 9";
      file.close();

      std::string text_path = path + "/text";
      std::ofstream tfile(text_path.c_str(),std::ios::out|std::ios::trunc);
      if(!tfile.is_open())
      {
        return false;
      }
      tfile << "These are some strings";
      tfile.close();
      return true;
    }

    void end_test()
    {
      std::string number_path = base_path + "/nums";
      std::string text_path = base_path + "/text";
      unlink(number_path.c_str());
      unlink(text_path.c_str());
    }

    bool test_single_number()
    {
      std::string num_path = base_path + "/num";

      if(!set_number_in_file(num_path,357))
      {
        return false;
      }
      unsigned long num = 0;
      if(!get_number_from_file(num_path,num))
      {
        return false;
      }
      if(num != 357)
      {
        return false;
      }
      unlink(num_path.c_str());
      return true;
    }
    
    bool test_single_str()
    {
      std::string str_path = base_path + "/str";

      if(!set_string_in_file(str_path,"a_string"))
      {
        return false;
      }
      std::string str;
      if(!get_string_from_file(str_path,str))
      {
        return false;
      }
      if(str != "a_string")
      {
        return false;
      }
      unlink(str_path.c_str());
      return true;
    }

    bool test_strings()
      {
      std::vector<std::string> strs;

      if(!get_strings_from_file(base_path + "/text",strs))
        {
        return false;
        }
      if(strs.size() != 4)
        {
        return false;
        }
      if(strs.at(2) != "some")
        {
        return false;
        }
      return true;
      }

    bool test_nums()
      {
      std::vector<unsigned long> nums;

      if(!get_numbers_from_file(base_path + "/nums",nums))
        {
        return false;
        }
      if(nums.size() != 5)
        {
        return false;
        }
      if(nums.at(2) != 5)
        {
        return false;
        }
      return true;
      }

};


START_TEST(test_one)
  {
    test_class testMe;

    testMe.start_test("/tmp");

    fail_unless(testMe.test_single_number() == true,"Single number test failed.");
    fail_unless(testMe.test_single_str() == true,"Single string test failed.");
    fail_unless(testMe.test_strings() == true,"Multiple string test failed.");
    fail_unless(testMe.test_nums() == true,"Multiple number test failed.");

    testMe.end_test();
  }
END_TEST

START_TEST(test_two)
  {
  }
END_TEST

Suite *sys_file_suite(void)
  {
  Suite *s = suite_create("sys_file_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
  suite_add_tcase(s, tc_core);

  return s;
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(sys_file_suite());
  srunner_set_log(sr, "sys_file_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
