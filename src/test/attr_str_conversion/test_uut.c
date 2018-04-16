#include <stdio.h>
#include <stdlib.h>

#include <string>
#include "utils.h"
#include <check.h>

#include "attribute.h"
#include "resource.h"
#include "server.h"

void appendEscapedXML(const char *xml, std::string &str);
int  size_to_str(struct size_value *szv, char *out, int space);
int  attr_to_str(std::string &output, attribute_def *at_def, pbs_attribute &attr, bool xml);
int resource_index_to_string(std::string &output, std::vector<resource> &resources, size_t index);


START_TEST(test_resource_index_to_string)
  {
  std::string  output;
  resource_def ad[4];
  resource     r[4];

  for (int i = 0; i < 4; i++)
    r[i].rs_defin = ad + i;

  ad[0].rs_name = "cput";
  ad[1].rs_name = "vmem";
  ad[2].rs_name = "mem";
  ad[3].rs_name = "stringy";

  r[0].rs_value.at_type = ATR_TYPE_LONG;
  r[0].rs_value.at_val.at_long = 200;
  r[1].rs_value.at_type = ATR_TYPE_SIZE;
  r[1].rs_value.at_val.at_size.atsv_num = 2048;
  r[1].rs_value.at_val.at_size.atsv_shift = 10;
  r[2].rs_value.at_type = ATR_TYPE_SIZE;
  r[2].rs_value.at_val.at_size.atsv_num = 3050;
  r[2].rs_value.at_val.at_size.atsv_shift = 10;
  r[3].rs_value.at_type = ATR_TYPE_STR;
  r[3].rs_value.at_val.at_str = strdup("bob");

  std::vector<resource> resources;
  for (int i = 0; i < 4; i++)
    resources.push_back(r[i]);

  fail_unless(resource_index_to_string(output, resources, 0) == PBSE_NONE);
  fail_unless(output == "cput=200");
  fail_unless(resource_index_to_string(output, resources, 1) == PBSE_NONE);
  fail_unless(output == "vmem=2048kb");
  fail_unless(resource_index_to_string(output, resources, 2) == PBSE_NONE);
  fail_unless(output == "mem=3050kb");
  fail_unless(resource_index_to_string(output, resources, 3) == PBSE_NONE);
  fail_unless(output == "stringy=bob");
  fail_unless(resource_index_to_string(output, resources, 4) != PBSE_NONE);
  }
END_TEST


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


START_TEST(test_str_to_attr)
  {
  pbs_attribute *pattr;
  attribute_def *pad;

  fail_unless((pattr = (pbs_attribute *)calloc(1, sizeof(pbs_attribute))) != NULL);
  fail_unless((pad = (attribute_def *)calloc(1, sizeof(attribute_def))) != NULL);

  pad->at_type = ATR_TYPE_BOOL;
  pad->at_name = strdup(ATTR_crayenabled);

  // t, 1 and y should all be true

  pattr->at_val.at_bool = false;
  fail_unless(str_to_attr(ATTR_crayenabled, (char *)"t", pattr, pad, 1) == PBSE_NONE);
  fail_unless(pattr->at_val.at_bool == true);

  pattr->at_val.at_bool = false;
  fail_unless(str_to_attr(ATTR_crayenabled, (char *)"T", pattr, pad, 1) == PBSE_NONE);
  fail_unless(pattr->at_val.at_bool == true);

  pattr->at_val.at_bool = false;
  fail_unless(str_to_attr(ATTR_crayenabled, (char *)"1", pattr, pad, 1) == PBSE_NONE);
  fail_unless(pattr->at_val.at_bool == true);

  pattr->at_val.at_bool = false;
  fail_unless(str_to_attr(ATTR_crayenabled, (char *)"y", pattr, pad, 1) == PBSE_NONE);
  fail_unless(pattr->at_val.at_bool == true);

  pattr->at_val.at_bool = false;
  fail_unless(str_to_attr(ATTR_crayenabled, (char *)"Y", pattr, pad, 1) == PBSE_NONE);
  fail_unless(pattr->at_val.at_bool == true);

  // everything else should be false

  pattr->at_val.at_bool = true;
  fail_unless(str_to_attr(ATTR_crayenabled, (char *)"f", pattr, pad, 1) == PBSE_NONE);
  fail_unless(pattr->at_val.at_bool == false);

  pattr->at_val.at_bool = true;
  fail_unless(str_to_attr(ATTR_crayenabled, (char *)"0", pattr, pad, 1) == PBSE_NONE);
  fail_unless(pattr->at_val.at_bool == false);

  pattr->at_val.at_bool = true;
  fail_unless(str_to_attr(ATTR_crayenabled, (char *)"n", pattr, pad, 1) == PBSE_NONE);
  fail_unless(pattr->at_val.at_bool == false);

  pattr->at_val.at_bool = true;
  fail_unless(str_to_attr(ATTR_crayenabled, (char *)"z", pattr, pad, 1) == PBSE_NONE);
  fail_unless(pattr->at_val.at_bool == false);
  }
END_TEST




Suite *attr_str_conversion_suite(void)
  {
  Suite *s = suite_create("attr_str_conversion test suite methods");
  TCase *tc_core = tcase_create("test_appendEscapedXML");
  tcase_add_test(tc_core, test_appendEscapedXML);
  tcase_add_test(tc_core, test_resource_index_to_string);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_size_to_str");
  tcase_add_test(tc_core, test_size_to_str);
  tcase_add_test(tc_core, test_attr_to_str);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_str_to_attr");
  tcase_add_test(tc_core, test_str_to_attr);
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

