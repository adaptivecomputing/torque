#include <boost/ptr_container/ptr_vector.hpp>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include "pbs_nodes.h"
#include "alps_constants.h"
#include "alps_functions.h"
#include <check.h>

int set_ncpus(struct pbsnode *,struct pbsnode *, int);
int set_ngpus(struct pbsnode *, int);
int set_state(struct pbsnode *, const char *);
void finish_gpu_status(unsigned int &i, std::vector<std::string> &status_info);
struct pbsnode *create_alps_subnode(struct pbsnode *parent, const char *node_id);
struct pbsnode *find_alpsnode_by_name(struct pbsnode *parent, const char *node_id);
struct pbsnode *determine_node_from_str(const char *str, struct pbsnode *parent, struct pbsnode *current);
int check_if_orphaned(void *str);
int process_reservation_id(struct pbsnode *pnode, const char *rsv_id_str);
int record_reservation(struct pbsnode *pnode, const char *rsv_id);

char buf[4096];

const char *alps_status[] = {"node=1", "CPROC=12", "state=UP", "reservation_id=12", "<cray_gpu_status>", "gpu_id=0", "clock_mhz=2600", "gpu_id=1", "clock_mhz=2600", "</cray_gpu_status>", NULL};
/*node=2\0CPROC=12\0state=UP\0<cray_gpu_status>\0gpu_id=0\0clock_mhz=2600\0gpu_id=1\0clock_mhz=2600\0</cray_gpu_status>\0node=3\0CPROC=12\0state=UP\0<cray_gpu_status>\0gpu_id=0\0clock_mhz=2600\0gpu_id=1\0clock_mhz=2600\0</cray_gpu_status>\0\0";*/

extern int mgr_count;
extern int removed_reservation;
extern int issued_request;
extern int state_updated;

START_TEST(record_reservation_test)
  {
  struct pbsnode pnode;

  fail_unless(record_reservation(&pnode, "1") != PBSE_NONE);

  job_usage_info jui(1);
  pnode.nd_job_usages.push_back(jui);
  fail_unless(record_reservation(&pnode, "1") == PBSE_NONE);
  }
END_TEST


START_TEST(set_ncpus_test)
  {
  pbsnode *pnode = new pbsnode();
  pbsnode *parent = new pbsnode();

  fail_unless(set_ncpus(pnode,parent, 2) == 0, "Couldn't set ncpus to 2");
  snprintf(buf, sizeof(buf), "ncpus should be 2 but is %d", pnode->nd_slots.get_total_execution_slots());
  fail_unless(pnode->nd_slots.get_total_execution_slots() == 2, buf);

  fail_unless(set_ncpus(pnode,parent, 4) == 0, "Couldn't set ncpus to 4");
  snprintf(buf, sizeof(buf), "ncpus should be 4 but is %d", pnode->nd_slots.get_total_execution_slots());
  fail_unless(pnode->nd_slots.get_total_execution_slots() == 4, buf);

  fail_unless(set_ncpus(pnode,parent, 8) == 0, "Couldn't set ncpus to 8");
  snprintf(buf, sizeof(buf), "ncpus should be 8 but is %d", pnode->nd_slots.get_total_execution_slots());
  fail_unless(pnode->nd_slots.get_total_execution_slots() == 8, buf);
  }
END_TEST




START_TEST(set_ngpus_test)
  {
  struct pbsnode pnode;

  fail_unless(set_ngpus(&pnode, 2) == 0, "Couldn't set ngpus to 2");
  snprintf(buf, sizeof(buf), "ngpus should be 2 but id %d", pnode.nd_ngpus);
  fail_unless(pnode.nd_ngpus == 2, buf);

  pnode.nd_ngpus = 0;
  fail_unless(set_ngpus(&pnode, 4) == 0, "Couldn't set ngpus to 4");
  snprintf(buf, sizeof(buf), "ngpus should be 4 but id %d", pnode.nd_ngpus);
  fail_unless(pnode.nd_ngpus == 4, buf);

  pnode.nd_ngpus = 0;
  fail_unless(set_ngpus(&pnode, 8) == 0, "Couldn't set ngpus to 8");
  snprintf(buf, sizeof(buf), "ngpus should be 8 but id %d", pnode.nd_ngpus);
  fail_unless(pnode.nd_ngpus == 8, buf);
  }
END_TEST





START_TEST(set_state_test)
  {
  struct pbsnode  pnode;
  const char    *up_str   = "state=UP";
  const char    *down_str = "state=DOWN";

  pnode.nd_state = 0;

  set_state(&pnode, up_str);
  snprintf(buf, sizeof(buf), "Couldn't set state to up, state is %d", pnode.nd_state);
  fail_unless(pnode.nd_state == INUSE_FREE, buf);
  set_state(&pnode, down_str);
  fail_unless((pnode.nd_state & INUSE_DOWN) != 0, "Couldn't set the state to down");

  set_state(&pnode, up_str);
  fail_unless(pnode.nd_state == INUSE_FREE, "Couldn't set the state to up 2");
  set_state(&pnode, down_str);
  fail_unless((pnode.nd_state & INUSE_DOWN) != 0, "Couldn't set the state to down 2");
  }
END_TEST




START_TEST(finish_gpu_status_test)
  {
  std::vector<std::string> status;
  unsigned int             i = 0;

  status.push_back("o");
  status.push_back("n");
  status.push_back("</cray_gpu_status>");
  status.push_back("tom");

  finish_gpu_status(i, status);
  snprintf(buf, sizeof(buf), "penultimate string isn't correct, should be '%s' but is '%s'",
    CRAY_GPU_STATUS_END, status[i].c_str());
  fail_unless(!strcmp(status[i].c_str(), CRAY_GPU_STATUS_END), buf);

  i++;
  snprintf(buf, sizeof(buf), "last string isn't correct, should be 'tom' but is '%s'",
    status[i].c_str());
  fail_unless(!strcmp(status[i].c_str(), "tom"), buf);

  }
END_TEST




START_TEST(find_alpsnode_test)
  {
  struct pbsnode  parent;
  const char     *node_id = (char *)"tom";
  struct pbsnode *alpsnode;

  parent.alps_subnodes = new all_nodes();

  alpsnode = find_alpsnode_by_name(&parent, node_id);
  fail_unless(alpsnode == NULL, "returned a non-NULL node?");

  }
END_TEST




START_TEST(determine_node_from_str_test)
  {
  struct pbsnode  parent;
  const char     *node_str1 = "node=tom";
  const char     *node_str2 = "node=george";
  struct pbsnode *new_node;

  parent.change_name("george");
  parent.alps_subnodes = new all_nodes();

  mgr_count = 0; // set so that create_alps_subnode doesn't fail
  new_node = determine_node_from_str(node_str1, &parent, &parent);
  fail_unless(new_node != NULL, "new node is NULL?");
  fail_unless(new_node->nd_lastupdate != 0, "update time not set");

  mgr_count = 0; // set so that create_alps_subnode doesn't fail
  new_node = determine_node_from_str(node_str2, &parent, &parent);
  fail_unless(new_node == &parent, "advanced current when current should've remained the same");

  }
END_TEST




START_TEST(check_orphaned_test)
  {
  const char *rsv_id = "napali:tom";
  removed_reservation = 0;
  issued_request = 0;
  state_updated = 0;

  fail_unless(check_if_orphaned(strdup(rsv_id)) == 0, "bad return code");
  fail_unless(issued_request == 1);
  fail_unless(state_updated == 1);
  }
END_TEST




START_TEST(create_alps_subnode_test)
  {
  struct pbsnode  parent;
  const char     *node_id = "tom";
  struct pbsnode *subnode;
  extern int      svr_clnodes;
  int             start_clnodes_value = svr_clnodes;;

  subnode = create_alps_subnode(&parent, node_id);
  fail_unless(subnode != NULL, "subnode was returned NULL?");
  fail_unless(subnode->parent == &parent, "parent set incorrectly");
  fail_unless(subnode->nd_ntype == NTYPE_CLUSTER, "node type incorrect");

  /* scaffolding makes it fail the second time */
  subnode = create_alps_subnode(&parent, node_id);
  fail_unless(subnode == NULL, "subnode isn't NULL when it should be");
  fail_unless(start_clnodes_value + 2 <= svr_clnodes);
  }
END_TEST



START_TEST(whole_test)
  {
  std::vector<std::string> ds;
  int                      rc;
  int                      i = 0;
  
  while (alps_status[i] != NULL)
    ds.push_back(alps_status[i++]);
 
  rc = process_alps_status((char *)"tom", ds);
  fail_unless(rc == 0, "didn't process alps status");
  }
END_TEST



START_TEST(process_reservation_id_test)
  {
  struct pbsnode pnode;

  pnode.change_name("napali");

  fail_unless(process_reservation_id(&pnode, "12") == 0, "couldn't process reservation");
  fail_unless(process_reservation_id(&pnode, "13") == 0, "couldn't process reservation");
  fail_unless(process_reservation_id(&pnode, "14") == 0, "couldn't process reservation");
  }
END_TEST



Suite *node_func_suite(void)
  {
  Suite *s = suite_create("alps helper suite methods");
  TCase *tc_core = tcase_create("set_ncpus_test");
  tcase_add_test(tc_core, set_ncpus_test);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("set_state_test");
  tcase_add_test(tc_core, set_state_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("finish_gpu_status_test");
  tcase_add_test(tc_core, finish_gpu_status_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("create_alps_subnode_test");
  tcase_add_test(tc_core, create_alps_subnode_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("find_alpsnode_test");
  tcase_add_test(tc_core, find_alpsnode_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("determine_node_from_str_test");
  tcase_add_test(tc_core, determine_node_from_str_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("check_orphaned_test");
  tcase_add_test(tc_core, check_orphaned_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("set_ngpus_test");
  tcase_add_test(tc_core, set_ngpus_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("whole_test");
  tcase_add_test(tc_core, whole_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("process_reservation_id_test");
  tcase_add_test(tc_core, process_reservation_id_test);
  tcase_add_test(tc_core, record_reservation_test);
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
  sr = srunner_create(node_func_suite());
  srunner_set_log(sr, "node_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }

