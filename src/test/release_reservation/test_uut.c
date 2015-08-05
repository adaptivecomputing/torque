#include <check.h>
#include <stdio.h>
#include "pbs_error.h"

#include "alps_constants.h"

char *apbasil_protocol = (char *)"1.0";
char *release_path = (char *)"../test_scripts/release_reservation.sh test_input.txt";
char *input_file = (char *)"test_input.txt";

int destroy_alps_reservation(char *, char *, char *, int);
void create_input(char *, char *);
int parse_release_output(char *output);

START_TEST(destroy_alps_reservation_test)
  {
  char *rsv1 = (char *)"12";
  char *rsv2 = (char *)"34";
  char *inp1 = (char *)"s\nf";
  char *inp2 = (char *)"s\ns";
  int   rc;

  fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);
  create_input(input_file, inp1);
  fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);
  rc = destroy_alps_reservation(rsv1, release_path, apbasil_protocol, 1);
  fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);
  fail_unless(rc == 0, "couldn't release rsv 1");
  fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);
  
  create_input(input_file, inp2);
  rc = destroy_alps_reservation(rsv2, release_path, apbasil_protocol, 1);
  fail_unless(rc != 0, "returned success but couldn't release rsv 2");
  }

END_TEST

START_TEST(test_parse_release_output)
  {
  int   rc;
  char  output[128];
  
  strcpy(output, "<nid1>12</nid1>");
  rc = parse_release_output(output);
  fail_unless(rc!=PBSE_NONE, "parse_release_output failed for failure case");

  }
END_TEST


START_TEST(test_two)
  {
  }
END_TEST

Suite *node_func_suite(void)
  {
  Suite *s = suite_create("alps helper suite methods");
  TCase *tc_core = tcase_create("destroy_alps_reservation_test");
  tcase_add_test(tc_core, destroy_alps_reservation_test);
  tcase_set_timeout(tc_core, 30);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_parse_release_output");
  tcase_add_test(tc_core, test_parse_release_output);
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
  sr = srunner_create(node_func_suite());
  srunner_set_log(sr, "node_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }

void create_input(char *filename, char* contents)
  {
  FILE *file;
  file = fopen(filename, "w");
  fprintf(file, "%s", contents);
  fclose(file);
  }
