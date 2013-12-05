#include "license_pbs.h" /* See here for the software license */
#include "mom_server_lib.h"
#include "test_mom_server.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

extern mom_hierarchy_t *mh;

extern void sort_paths();


START_TEST(test_sort_paths)
  {
  char before[500];
  char after[500];
  mh = initialize_mom_hierarchy();
  for(int paths = 0;paths < 5;paths++)
    {
    mom_levels *lvl = new  mom_levels();
    mh->paths->push_back(lvl);
    for(int levels = 0;levels < (15 - paths);levels++)
      {
      mom_nodes *nd = new mom_nodes();
      lvl->push_back(nd);
      }
    }
  before[0] = '\0';
  for(size_t i = 0;i < mh->paths->size();i++)
    {
    char num[10];
    sprintf(num,"%d ",(int)mh->paths->at(i)->size());
    strcat(before,num);
    }

  sort_paths();

  after[0] = '\0';
  for(size_t i = 0;i < mh->paths->size();i++)
    {
    char num[10];
    sprintf(num,"%d ",(int)mh->paths->at(i)->size());
    strcat(after,num);
    }
  fail_unless(strcmp(before,"15 14 13 12 11 ") == 0);
  fail_unless(strcmp(after,"11 12 13 14 15 ") == 0);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *mom_server_suite(void)
  {
  Suite *s = suite_create("mom_server_suite methods");
  TCase *tc_core = tcase_create("test_sort_paths");
  tcase_add_test(tc_core, test_sort_paths);
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
  sr = srunner_create(mom_server_suite());
  srunner_set_log(sr, "mom_server_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
