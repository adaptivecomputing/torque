#include "license_pbs.h" /* See here for the software license */
#include "qmgr.h"
#include "test_qmgr.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"

int check_list(int type, const char *list);

void free_attrib(

  struct attropl **attribute)

  {
    struct attropl *p = *attribute; 
    if (p->name)
      free(p->name);

    if (p->value)
      free(p->value);

    if (p->resource)
      free(p->resource);
    
    free(p);
    *attribute = NULL;
  }


START_TEST(test_check_list)
  {
  // Server cannot be numeric
  fail_unless(check_list(MGR_OBJ_SERVER, "12") != 0);
  // Cray nodes can be numeric, so allow those
  fail_unless(check_list(MGR_OBJ_NODE, "12") == 0);
  // all alpha characters should work for both
  fail_unless(check_list(MGR_OBJ_NODE, "napali") == 0);
  fail_unless(check_list(MGR_OBJ_SERVER, "napali") == 0);
  }
END_TEST


START_TEST(test_res_max_nodes)
  {
  const char *copt = "s q one_nodeq resources_max.nodes = 1:ppn=32";
  int type, oper, errflg;
  char *name = NULL;
  struct attropl *attribs = NULL;

  type = oper = 0;
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(4 == errflg, "fail to detect resource_max.nodes incorrect syntax");

  name = NULL;
  if (attribs)
    {
    free_attrib(&attribs);
    }
  oper = type = 0;
  copt = "s q one_nodeq resources_max.nodes = 5";
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(0 == errflg, "fail to detect resource_max.nodes correct syntax");
  }
END_TEST

START_TEST(test_res_max_nodect)
  {
  const char *copt = "s q one_nodeq resources_max.nodect = 1:ppn=32";
  int type, oper, errflg; 
  char *name = NULL;
  struct attropl *attribs = NULL;

  type = oper = 0;
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(4 == errflg, "fail to detect resource_max.nodect incorrect syntax");

  name = NULL;
  if (attribs)
    {
    free_attrib(&attribs);
    }
  oper = type = 0;
  copt = "s q one_nodeq resources_max.nodect = 5";
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(0 == errflg, "fail to detect resouces_max.nodect correct syntax");
  }
END_TEST

START_TEST(test_res_max_procct)
  {
  const char *copt = "s q one_nodeq resources_max.procct = 1:ppn=32";
  int type, oper, errflg; 
  char *name = NULL;
  struct attropl *attribs = NULL;

  type = oper = 0;
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(4 == errflg, "fail to detect resource_max.procct incorrect syntax");

  name = NULL;
  if (attribs)
    {
    free_attrib(&attribs);
    }
  oper = type = 0;
  copt = "s q one_nodeq resources_max.procct = 5";
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(0 == errflg, "fail to detect resource_max.procct correct syntax");
  }
END_TEST

START_TEST(test_res_min_nodes)
  {
  const char *copt = "s q one_nodeq resources_min.nodes = 1:ppn=32";
  int type, oper, errflg; 
  char *name = NULL;
  struct attropl *attribs = NULL;

  type = oper = 0;
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(4 == errflg, "fail to detect resource_min.nodes incorrect syntax");

  name = NULL;
  if (attribs)
    {
    free_attrib(&attribs);
    }
  oper = type = 0;
  copt = "s q one_nodeq resources_min.nodes = 5";
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(0 == errflg, "fail to detect resource_min.nodes correct syntax");
  }
END_TEST

START_TEST(test_res_min_nodect)
  {
  const char *copt = "s q one_nodeq resources_min.nodect = 1:ppn=32";
  int type, oper, errflg; 
  char *name = NULL;
  struct attropl *attribs = NULL;

  type = oper = 0;
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(4 == errflg, "fail to detect resource_min.nodect incorrect syntax");

  name = NULL;
  if (attribs)
    {
    free_attrib(&attribs);
    }
  oper = type = 0;
  copt = "s q one_nodeq resources_min.nodes = 5";
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(0 == errflg, "fail to detect resource_min.nodect correct syntax");
  }
END_TEST

START_TEST(test_res_min_procct)
  {
  const char *copt = "s q one_nodeq resources_min.procct = 1:ppn=32";
  int type, oper, errflg; 
  char *name = NULL;
  struct attropl *attribs = NULL;

  type = oper = 0;
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(4 == errflg, "fail to detect resource_min.procct incorrect syntax");

  name = NULL;
  if (attribs)
    {
    free_attrib(&attribs);
    }
  oper = type = 0;
  copt = "s q one_nodeq resources_min.procct = 5";
  errflg = parse((char *)copt, &oper, &type, &name, &attribs);
  fail_unless(0 == errflg, "fail to detect resouce_min.procct correct syntax");
  }
END_TEST

Suite *qmgr_suite(void)
  {
  Suite *s = suite_create("qmgr_suite methods");
  TCase *tc_core = tcase_create("test_res_max_nodes");
  tcase_add_test(tc_core, test_res_max_nodes);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_res_max_nodect");
  tcase_add_test(tc_core, test_res_max_nodect);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_res_max_procct");
  tcase_add_test(tc_core, test_res_max_procct);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_res_min_nodes");
  tcase_add_test(tc_core, test_res_min_nodes);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_res_min_nodect");
  tcase_add_test(tc_core, test_res_min_nodect);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_res_min_procct");
  tcase_add_test(tc_core, test_res_min_procct);
  tcase_add_test(tc_core, test_check_list);
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
  sr = srunner_create(qmgr_suite());
  srunner_set_log(sr, "qmgr_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
