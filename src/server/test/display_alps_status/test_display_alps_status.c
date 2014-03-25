#include "pbs_nodes.h"
#include <check.h>
#include "alps_constants.h"


#include "batch_request.h"
#include "list_link.h"
#include "alps_functions.h"


START_TEST(get_status_test)
  {
  tlist_head           pstat;
  struct pbsnode       pnode;
  int                  bad;
  struct batch_request preq;
  int                  count = 0;
  int                  rc = 0;

  pstat.ll_struct = &count;

  pnode.alps_subnodes = new all_nodes();
  for(int i = 0;i < 10;i++)
    {
    char id[10];
    struct pbsnode *pNd = (struct pbsnode *)calloc(1,sizeof(struct pbsnode));
    sprintf(id,"node_%d",i);
    pnode.alps_subnodes->lock();
    pnode.alps_subnodes->insert(pNd,id);
    pnode.alps_subnodes->unlock();
    }
  rc = get_alps_statuses(&pnode, &preq, &bad, &pstat);

  fail_unless(rc == 0, "Couldn't get the alps statuses?");
  fail_unless(count == 10, "The wrong count was returned");
  }
END_TEST




Suite *node_func_suite(void)
  {
  Suite *s = suite_create("alps helper suite methods");

  TCase *tc_core = tcase_create("get_status_test");
  tcase_add_test(tc_core, get_status_test);
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
  return number_failed;
  }

