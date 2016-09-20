#include <stdio.h>

#include "pbs_nodes.h"
#include "license_pbs.h" /* See here for the software license */
#include "mom_hierarchy_handler.h"
#include "test_mom_hierarchy_handler.h"

extern char      *path_mom_hierarchy;
extern all_nodes allnodes;
extern int lock_node_count;

class sendNodeHolder
  {
public:
  int id;
  unsigned short int level;
  sendNodeHolder(struct pbsnode *node)
    {
    id = node->nd_id;
    level = node->nd_hierarchy_level;
    }
  };

class test_mom_hierarchy_handler
  {
public:
  pbsnode *call_nextNode(mom_hierarchy_handler hierarchy_handler)
    {
    all_nodes_iterator *iter = NULL; 

    return hierarchy_handler.nextNode(&iter);
    }
  };


extern int xml_index;

START_TEST(test_next_node)
  {
  pbsnode *np;
  u_long pul = 0;
  pbsnode node("foo", &pul, true);
  test_mom_hierarchy_handler test_hierarchy_handler;

  // insert node into allnodes
  //node.change_name("foo");
  insert_node(&allnodes, &node);

  // confirm nextNode locks node
  lock_node_count = 0;
  np = test_hierarchy_handler.call_nextNode(hierarchy_handler);
  fail_unless(np != NULL);
  fail_unless(lock_node_count == 1);
  }
END_TEST


START_TEST(test_checkAndSendHierarchy)
  {
  u_long pul = 0;
  pbsnode node("foo", &pul, true);

  // first, remove node from allnodes (from previous test)
  allnodes.unlock();
  remove_node(&allnodes, &node);

  // insert node into allnodes
  node.nd_state = INUSE_DOWN|INUSE_OFFLINE|INUSE_NOHIERARCHY;
  insert_node(&allnodes, &node);

  hierarchy_handler.checkAndSendHierarchy(false);
  fail_unless(node.nd_state == INUSE_DOWN);
  }
END_TEST


START_TEST(load_hierarchy_test)
  {
  path_mom_hierarchy = (char *)"./dummy_hierarchy.txt"; //Need it to find something.
  hierarchy_handler.initialLoadHierarchy();

  /*
  allnodes.lock();
  all_nodes_iterator *it = allnodes.get_iterator();
  pbsnode *pNode;

  while((pNode = it->get_next_item()) != NULL)
    {
    fprintf(stderr,"%s %d %d\n",pNode->nd_name,pNode->nd_hierarchy_level,pNode->nd_id);
    }
  delete it;
  allnodes.unlock();
  */

  pbsnode node;
  sendNodeHolder *snh = new sendNodeHolder(&node);
  xml_index = 0;

  hierarchy_handler.sendHierarchyThreadTask((void *)snh);


  hierarchy_handler.reloadHierarchy();

  pbsnode node2;
  snh = new sendNodeHolder(&node2);
  xml_index = 0;

  hierarchy_handler.sendHierarchyThreadTask((void *)snh);

  }
END_TEST


Suite *mom_hierarchy_handler_suite(void)
  {
  Suite *s = suite_create("mom_hierarchy_handler_suite methods");

  TCase *tc_core = tcase_create("mom_hierarchy_handler_test");
  tcase_add_test(tc_core, load_hierarchy_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_next_node");
  tcase_add_test(tc_core, test_next_node);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_checkAndSendHierarchy");
  tcase_add_test(tc_core, test_checkAndSendHierarchy);
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
  sr = srunner_create(mom_hierarchy_handler_suite());
  srunner_set_log(sr, "mom_hierarchy_handler_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
