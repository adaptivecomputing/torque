#include "license_pbs.h" /* See here for the software license */
#include "attr_recov.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"


struct attribute_def test_def[] =
{
    {"A String",decode_str,encode_str,set_str,comp_str,free_str,NULL_FUNC,READ_WRITE,ATR_TYPE_STR,PARENT_TYPE_JOB},
    {"A Long",decode_l,encode_l,set_l,comp_l,free_null,NULL_FUNC,READ_WRITE,ATR_TYPE_LONG,PARENT_TYPE_JOB}
};

char saveBuff[1024];
int saveBuffRdPtr = 0;
int saveBuffEndPtr = 0;

START_TEST(test_one)
  {
  size_t spaceLeft = sizeof(saveBuff);
  pbs_attribute things[2];
  pbs_attribute recoveredThings[3];

  memset(things,0,sizeof(things));
  decode_str(&things[0],"A String","Hello","I am Thing One",0);
  decode_l(&things[1],"A Long","Hello","3577385",0);

  fail_unless(save_attr(test_def,things,2,0,saveBuff,&spaceLeft,sizeof(saveBuff)) == 0);
  saveBuffEndPtr = sizeof(saveBuff) - spaceLeft;

  memset(recoveredThings,0,sizeof(recoveredThings));

  fail_unless(recov_attr(0,NULL,test_def,recoveredThings,2,0,0) == 0);

  fail_unless(comp_str(things,recoveredThings) == 0);
  fail_unless(comp_l(things + 1,recoveredThings + 1) == 0);

  //Corrupt the file and make sure it errors properly
  saveBuffRdPtr = 0;
  saveBuff[0x45] = 6;
  memset(recoveredThings,0,sizeof(recoveredThings));

  fail_unless(recov_attr(0,NULL,test_def,recoveredThings,2,0,0) == -1);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *attr_recov_suite(void)
  {
  Suite *s = suite_create("attr_recov_suite methods");
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
  sr = srunner_create(attr_recov_suite());
  srunner_set_log(sr, "attr_recov_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
