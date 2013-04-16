#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "utils.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_one)
  {
  char *in = (char *)"this <is> a \"string\" with a 'character' & so forth.";
  char shrt[5];
  char out[1024];
  char final[1024];


  fail_unless(escape_xml(NULL,NULL,sizeof(out)) == BUFFER_OVERFLOW);
  fail_unless(escape_xml(in,NULL,sizeof(out)) == BUFFER_OVERFLOW);
  fail_unless(escape_xml(in,shrt,sizeof(shrt)) == BUFFER_OVERFLOW);
  fail_unless(escape_xml(in,out,sizeof(out)) == PBSE_NONE);
  fail_unless(unescape_xml(out,shrt,sizeof(shrt)) == BUFFER_OVERFLOW);
  fail_unless(unescape_xml(out,final,sizeof(final)) == PBSE_NONE);
  fail_unless(strcmp(in,final) == 0);


  }
END_TEST

START_TEST(test_two)
  {
  char *sample = (char *)"  <parent><child>some text</child><kid>more text</kid></parent> ";
  char xml[1024];
  char *parent,*child,*end;

  xml[0] = '\0';
  fail_unless(get_parent_and_child(xml,&parent,&child,&end) == -1);
  strcpy(xml,"<unfinished ");
  fail_unless(get_parent_and_child(xml,&parent,&child,&end) == -1);
  strcpy(xml,"<unfinished><child>some text</child> ");
  fail_unless(get_parent_and_child(xml,&parent,&child,&end) == -1);
  strcpy(xml,sample);
  fail_unless(get_parent_and_child(xml,&parent,&child,&end) == 0);
  fail_unless(strcmp(parent,"parent")==0);
  fail_unless(strncmp(child,"<child>",7) == 0);

  strcpy(xml,sample);
  fail_unless(find_next_tag(NULL,&end) == NULL);
  fail_unless(find_next_tag(xml,&end) != NULL);


  }
END_TEST

Suite *u_xml_suite(void)
  {
  Suite *s = suite_create("u_xml_suite methods");
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
  sr = srunner_create(u_xml_suite());
  srunner_set_log(sr, "u_xml_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
