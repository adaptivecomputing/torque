#include <stdio.h>
#include <stdlib.h>

#include <string>
#include "utils.h"
#include <check.h>

#include "attribute.h"
#include "server.h"

void appendEscapedXML(const char *xml, std::string &str);
int  size_to_str(struct size_value *szv, char *out, int space);
int  attr_to_str(std::string &output, attribute_def *at_def, pbs_attribute attr, bool xml);


START_TEST(test_attr_to_str)
  {
  std::string   output;
  attribute_def ad;
  pbs_attribute pattr;

  memset(&pattr, 0, sizeof(pattr));
  ad.at_type = ATR_TYPE_LONG;

  fail_unless(attr_to_str(output, &ad, pattr, false) == NO_ATTR_DATA);

  pattr.at_flags |= ATR_VFLAG_SET;
  pattr.at_val.at_long = 7;
  fail_unless(attr_to_str(output, &ad, pattr, false) == PBSE_NONE);
  fail_unless(output == "7");

  output.clear();
  ad.at_type = ATR_TYPE_CHAR;
  pattr.at_val.at_char = 'a';
  fail_unless(attr_to_str(output, &ad, pattr, false) == PBSE_NONE);
  fail_unless(output == "a", "is %s", output.c_str());

  output.clear();
  ad.at_type = ATR_TYPE_STR;
  pattr.at_val.at_str = strdup("bobby wood");
  fail_unless(attr_to_str(output, &ad, pattr, false) == PBSE_NONE);
  fail_unless(output == "bobby wood");

  }
END_TEST


START_TEST(test_appendEscapedXML)
  {
  std::string escaped;

  appendEscapedXML("command > log.txt 2>&1", escaped);
  fail_unless(escaped == "command &gt; log.txt 2&gt;&amp;1");

  escaped.clear();
  appendEscapedXML("do not alter this", escaped);
  fail_unless(escaped == "do not alter this");

  escaped.clear();
  appendEscapedXML("don't use \" for emphasis, it's < grammatically correct", escaped);
  fail_unless(escaped == "don&apos;t use &quot; for emphasis, it&apos;s &lt; grammatically correct");
  }
END_TEST



START_TEST(test_size_to_str)
  {
  struct size_value sv;
  char              buf[1024];

  sv.atsv_num = 2;
  sv.atsv_shift = 50;

  fail_unless(size_to_str(sv, buf, sizeof(buf)) == PBSE_NONE);
  fail_unless(!strcmp(buf, "2pb"));

  sv.atsv_num = 30;
  sv.atsv_shift = 40;
  fail_unless(size_to_str(sv, buf, sizeof(buf)) == PBSE_NONE);
  fail_unless(!strcmp(buf, "30tb"));

  sv.atsv_num = 4;
  sv.atsv_shift = 30;
  fail_unless(size_to_str(sv, buf, sizeof(buf)) == PBSE_NONE);
  fail_unless(!strcmp(buf, "4gb"));

  sv.atsv_num = 500;
  sv.atsv_shift = 20;
  fail_unless(size_to_str(sv, buf, sizeof(buf)) == PBSE_NONE);
  fail_unless(!strcmp(buf, "500mb"));

  sv.atsv_num = 1;
  sv.atsv_shift = 10;
  fail_unless(size_to_str(sv, buf, sizeof(buf)) == PBSE_NONE);
  fail_unless(!strcmp(buf, "1kb"));
  
  fail_unless(size_to_str(sv, buf, 2) != PBSE_NONE);
  }
END_TEST




Suite *attr_str_conversion_suite(void)
  {
  Suite *s = suite_create("attr_str_conversion test suite methods");
  TCase *tc_core = tcase_create("test_appendEscapedXML");
  tcase_add_test(tc_core, test_appendEscapedXML);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_size_to_str");
  tcase_add_test(tc_core, test_size_to_str);
  tcase_add_test(tc_core, test_attr_to_str);
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
  sr = srunner_create(attr_str_conversion_suite());
  srunner_set_log(sr, "attr_str_conversion_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }

