#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "dynamic_string.h"
#include "alps_constants.h"
#include "pbs_nodes.h"

int set_ncpus(struct pbsnode *, char *);
int set_ngpus(struct pbsnode *, int);
int set_state(struct pbsnode *, char *);
char *finish_gpu_status(char *str);
struct pbsnode *create_alps_subnode(struct pbsnode *parent, char *node_id);
struct pbsnode *find_alpsnode_by_name(struct pbsnode *parent, char *node_id);
struct pbsnode *determine_node_from_str(char *str, struct pbsnode *parent, struct pbsnode *current);
int check_if_orphaned(char *str);
int process_alps_status(char *, dynamic_string *);
int process_reservation_id(struct pbsnode *pnode, char *rsv_id_str);

char buf[4096];

char *alps_status = "node=1\0CPROC=12\0state=UP\0reservation_id=12\0<cray_gpu_status>\0gpu_id=0\0clock_mhz=2600\0gpu_id=1\0clock_mhz=2600\0</cray_gpu_status>\0\0";
/*node=2\0CPROC=12\0state=UP\0<cray_gpu_status>\0gpu_id=0\0clock_mhz=2600\0gpu_id=1\0clock_mhz=2600\0</cray_gpu_status>\0node=3\0CPROC=12\0state=UP\0<cray_gpu_status>\0gpu_id=0\0clock_mhz=2600\0gpu_id=1\0clock_mhz=2600\0</cray_gpu_status>\0\0";*/



START_TEST(set_ncpus_test)
  {
  struct pbsnode  pnode;
  char           *proc1 = "CPROC=2";
  char           *proc2 = "CPROC=4";
  char           *proc3 = "CPROC=8";

  pnode.nd_nsn = 0;
  fail_unless(set_ncpus(&pnode, proc1) == 0, "Couldn't set ncpus to 2");
  snprintf(buf, sizeof(buf), "ncpus should be 2 but is %d", pnode.nd_nsn);
  fail_unless(pnode.nd_nsn == 2, buf);

  pnode.nd_nsn = 0;
  fail_unless(set_ncpus(&pnode, proc2) == 0, "Couldn't set ncpus to 4");
  snprintf(buf, sizeof(buf), "ncpus should be 4 but is %d", pnode.nd_nsn);
  fail_unless(pnode.nd_nsn == 4, buf);

  pnode.nd_nsn = 0;
  fail_unless(set_ncpus(&pnode, proc3) == 0, "Couldn't set ncpus to 8");
  snprintf(buf, sizeof(buf), "ncpus should be 8 but is %d", pnode.nd_nsn);
  fail_unless(pnode.nd_nsn == 8, buf);
  }
END_TEST




START_TEST(set_ngpus_test)
  {
  struct pbsnode pnode;

  memset(&pnode, 0, sizeof(pnode));

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
  char           *up_str   = "state=UP";
  char           *down_str = "state=DOWN";

  memset(&pnode, 0, sizeof(pnode));

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
  char  status[] = "o\0n\0</cray_gpu_status>\0tom";
  char  str[sizeof(status)];
  char *end;

  memcpy(str, status, sizeof(status));

  end = finish_gpu_status(str);
  snprintf(buf, sizeof(buf), "penultimate string isn't correct, should be '%s' but is '%s'",
    CRAY_GPU_STATUS_END, end);
  fail_unless(!strcmp(end, CRAY_GPU_STATUS_END), buf);

  end += strlen(end) + 1;
  snprintf(buf, sizeof(buf), "last string isn't correct, should be 'tom' but is '%s'",
    end);
  fail_unless(!strcmp(end, "tom"), buf);

  }
END_TEST




START_TEST(find_alpsnode_test)
  {
  struct pbsnode  parent;
  char           *node_id = "tom";
  struct pbsnode *alpsnode;

  parent.alps_subnodes.allnodes_mutex = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(parent.alps_subnodes.allnodes_mutex, NULL);

  alpsnode = find_alpsnode_by_name(&parent, node_id);
  fail_unless(alpsnode == NULL, "returned a non-NULL node?");

  }
END_TEST




START_TEST(determine_node_from_str_test)
  {
  struct pbsnode  parent;
  char           *node_str1 = "node=tom";
  char           *node_str2 = "node=george";
  struct pbsnode *new_node;

  memset(&parent, 0, sizeof(parent));
  parent.nd_name = strdup("george");
  parent.alps_subnodes.allnodes_mutex = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(parent.alps_subnodes.allnodes_mutex, NULL);

  new_node = determine_node_from_str(node_str1, &parent, &parent);
  fail_unless(new_node != NULL, "new node is NULL?");
  fail_unless(new_node->nd_lastupdate != 0, "update time not set");

  new_node = determine_node_from_str(node_str2, &parent, &parent);
  fail_unless(new_node == &parent, "advanced current when current should've remained the same");

  }
END_TEST




START_TEST(check_orphaned_test)
  {
  char *rsv_id = "tom";

  fail_unless(check_if_orphaned(rsv_id) == 0, "bad return code");
  }
END_TEST




START_TEST(create_alps_subnode_test)
  {
  struct pbsnode  parent;
  char           *node_id = "tom";
  struct pbsnode *subnode;

  memset(&parent, 0, sizeof(struct pbsnode));

  subnode = create_alps_subnode(&parent, node_id);
  fail_unless(subnode != NULL, "subnode was returned NULL?");
  fail_unless(subnode->parent == &parent, "parent set incorrectly");
  fail_unless(subnode->nd_ntype == NTYPE_CLUSTER, "node type incorrect");

  /* scaffolding makes it fail the second time */
  subnode = create_alps_subnode(&parent, node_id);
  fail_unless(subnode == NULL, "subnode isn't NULL when it should be");
  }
END_TEST



START_TEST(whole_test)
  {
  dynamic_string *ds = get_dynamic_string(2048, NULL);
  int             rc;
  
  ds->str = alps_status;
 
  rc = process_alps_status("tom", ds);
  fail_unless(rc == 0, "didn't process alps status");
  }
END_TEST



START_TEST(process_reservation_id_test)
  {
  struct pbsnode pnode;
  struct pbssubn sub;

  memset(&pnode, 0, sizeof(struct pbsnode));
  memset(&sub, 0, sizeof(struct pbssubn));
  sub.jobs = calloc(1, sizeof(struct jobinfo));
  strcpy(sub.jobs->jobid, "bob");
  pnode.nd_psn = &sub;

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

