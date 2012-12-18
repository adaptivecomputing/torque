#include "license_pbs.h" /* See here for the software license */
#include "node_func.h"
#include "test_node_func.h"
#include "pbs_error.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_nodes.h" /* pbs_nodes, node_check_info, node_iterator, all_nodes */
#include "attribute.h" /* svrattrl, struct  */

int cray_enabled;

void initialize_allnodes(all_nodes *an, struct pbsnode *n1, struct pbsnode *n2)
  {
  an->allnodes_mutex = calloc(1, sizeof(pthread_mutex_t));
  an->ra = calloc(1, sizeof(resizable_array));
  an->ra->slots = calloc(3, sizeof(slot));

  an->ra->slots[0].item = NULL;
  an->ra->slots[1].item = n1;
  an->ra->slots[2].item = n2;
  }

START_TEST(PGetNodeFromAddr_test)
  {
  pbs_net_t address = 0;
  //alloc mutex, use, restore
  pthread_mutex_t *mutex = allnodes.allnodes_mutex;
  allnodes.allnodes_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(allnodes.allnodes_mutex, NULL);

  struct pbsnode *result = PGetNodeFromAddr(address);

  free((void*)allnodes.allnodes_mutex);
  allnodes.allnodes_mutex = mutex;

  fail_unless(result == NULL, "null address input fail");
  }
END_TEST

START_TEST(bad_node_warning_test)
  {
  pbs_net_t address = 0;
  struct pbsnode node;
  initialize_pbsnode(&node, NULL, NULL, 0);

  //alloc mutex, use, restore
  pthread_mutex_t *mutex = allnodes.allnodes_mutex;
  allnodes.allnodes_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(allnodes.allnodes_mutex, NULL);

  bad_node_warning(address, NULL);
  bad_node_warning(address, &node);

  //free((void*)allnodes.allnodes_mutex);
  allnodes.allnodes_mutex = mutex;
  }
END_TEST

START_TEST(addr_ok_test)
  {
  pbs_net_t address = 0;
  struct pbsnode node;
  initialize_pbsnode(&node, NULL, NULL, 0);

  //alloc mutex, use, restore
  pthread_mutex_t *mutex = allnodes.allnodes_mutex;
  allnodes.allnodes_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(allnodes.allnodes_mutex, NULL);

  int result = addr_ok(address, NULL);
  fail_unless(result == 1, "NULL node input fail: %d", result);

  result = addr_ok(address, &node);
  fail_unless(result == 1, "empty node input fail: %d", result);

  free((void*)allnodes.allnodes_mutex);
  allnodes.allnodes_mutex = mutex;
  }
END_TEST

START_TEST(initialize_all_nodes_array_test)
  {
  struct all_nodes allnodes;
  memset(&allnodes, 0, sizeof(allnodes));
  initialize_all_nodes_array(&allnodes);
  fail_unless(allnodes.ra != NULL, "all_nodes resizable_array was not initialized");
  fail_unless(allnodes.ht != NULL, "all_nodes hash_table_t was not initialized");
  fail_unless(allnodes.allnodes_mutex != NULL, "all_nodes pthread_mutex_t was not initialized");
  }
END_TEST

START_TEST(find_node_in_allnodes_test)
  {
  struct pbsnode *result = NULL;
  struct all_nodes allnodes;
  struct pbsnode test_node;
  char *test_node_name = "test_node";
  test_node.nd_name = test_node_name;

  initialize_pbsnode(&test_node, NULL, NULL, 0);
  memset(&allnodes, 0, sizeof(allnodes));
  initialize_all_nodes_array(&allnodes);

  result = find_node_in_allnodes(NULL, "nodename");
  fail_unless(result == NULL, "NULL input all_nodes struct pointer fail");

  result = find_node_in_allnodes(&allnodes, NULL);
  fail_unless(result == NULL, "NULL input nodename fail");

  result = find_node_in_allnodes(&allnodes, "nodename");
  fail_unless(result == NULL, "find unexpected node fail");

/* TODO: think about apropriate mocking for the test below
  insert_node(&allnodes, &test_node);
  result = find_node_in_allnodes(&allnodes, "test_node");
  fail_unless(result == &test_node, "find node fail");
*/
  }
END_TEST

START_TEST(find_nodebyname_test)
  {
  struct pbsnode  node1;
  struct pbsnode  node2;
  struct pbsnode  reporter;
  struct pbsnode *pnode;

  alps_reporter = &reporter;

  memset(&node1, 0, sizeof(node1));
  memset(&node2, 0, sizeof(node2));

  node1.nd_name = "bob";
  node2.nd_name = "tom";
  initialize_allnodes(&allnodes, &node1, &node2);
  initialize_allnodes(&alps_reporter->alps_subnodes, &node1, &node2);

  cray_enabled = FALSE;

  pnode = find_nodebyname(NULL);
  fail_unless(pnode == NULL, "NULL nodename input fail");

  pnode = find_nodebyname("george");
  fail_unless(pnode == NULL, "george found but doesn't exist");

  pnode = find_nodebyname("bob");
  fail_unless(pnode == &node1, "couldn't find bob?");

  pnode = find_nodebyname("tom");
  fail_unless(pnode == &node2, "couldn't find tom?");

  pnode = find_nodebyname(strdup("tom-0"));
  fail_unless(!strcmp(pnode->nd_name, "0"), "found an incorrect node name");

  pnode = find_nodebyname(strdup("tom-1"));
  fail_unless(!strcmp(pnode->nd_name, "1"), "found an incorrect node name");

  pnode = find_nodebyname(strdup("tom-10"));
  fail_unless(pnode == NULL, "found an incorrect node name");

  pnode = find_nodebyname(strdup("bob/0"));
  fail_unless(pnode == &node1, "couldn't find bob with the exec_host format");

  allnodes.ra->slots[1].item = NULL;
  allnodes.ra->slots[2].item = NULL;

  cray_enabled = TRUE;

  pnode = find_nodebyname("tom");
  fail_unless(pnode == &node2, "couldn't find tom?");

  cray_enabled = TRUE;

  pnode = find_nodebyname("bob");
  fail_unless(pnode == &node1, "couldn't find bob?");

  cray_enabled = TRUE;

  pnode = find_nodebyname("george");
  fail_unless(pnode == NULL, "george found but doesn't exist");

  }
END_TEST

START_TEST(save_characteristic_test)
  {
  struct pbsnode node;
  struct node_check_info node_info;
  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&node_info, 0, sizeof(node_info));
  save_characteristic(NULL, &node_info);
  save_characteristic(&node, NULL);
  save_characteristic(&node, &node_info);
  }
END_TEST

START_TEST(chk_characteristic_test)
  {
  struct pbsnode node;
  struct node_check_info node_info;
  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&node_info, 0, sizeof(node_info));
  int result = 0;
  int mask = 0;

  result = chk_characteristic(NULL, &node_info, &mask);
  fail_unless(result != PBSE_NONE, "NULL input node pointer fail");

  result = chk_characteristic(&node, NULL, &mask);
  fail_unless(result != PBSE_NONE, "NULL input node info pointer fail");

  result = chk_characteristic(&node, &node_info, NULL);
  fail_unless(result != PBSE_NONE, "NULL input mask pointer fail");

  result =  chk_characteristic(&node, &node_info, &mask);
  fail_unless(result == PBSE_NONE, "chk_characteristic fail");
  }
END_TEST

START_TEST(login_encode_jobs_test)
  {
  struct pbsnode node;
  struct list_link list;
  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&list, 0, sizeof(list));

  int result = login_encode_jobs(NULL, &list);
  fail_unless(result != PBSE_NONE, "NULL input node pointer fail");

  result = login_encode_jobs(&node, NULL);
  fail_unless(result != PBSE_NONE, "NULL input list_link pointer fail");

  result = login_encode_jobs(&node, &list);
  fail_unless(result != PBSE_NONE, "login_encode_jobs_test fail");
  }
END_TEST

START_TEST(status_nodeattrib_test)
  {
  struct svrattrl attributes;
  struct attribute_def node_attributes;
  struct pbsnode node;
  struct list_link list;
  int result_mask = 0;
  int result = 0;
  memset(&attributes, 0, sizeof(attributes));
  memset(&node_attributes, 0, sizeof(node_attributes));
  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&list, 0, sizeof(list));


  result = status_nodeattrib(&attributes,
                             NULL,
                             &node,
                             0,
                             0,
                             &list,
                             &result_mask);
  fail_unless(result != PBSE_NONE, "NULL input attribute_def pointer fail: %d" ,result);

  result = status_nodeattrib(&attributes,
                             &node_attributes,
                             NULL,
                             0,
                             0,
                             &list,
                             &result_mask);
  fail_unless(result != PBSE_NONE, "NULL input pbsnode pointer fail: %d" ,result);

  result = status_nodeattrib(&attributes,
                             &node_attributes,
                             &node,
                             0,
                             0,
                             NULL,
                             &result_mask);
  fail_unless(result != PBSE_NONE, "NULL input tlist_head pointer fail: %d" ,result);

  result = status_nodeattrib(&attributes,
                             &node_attributes,
                             &node,
                             0,
                             0,
                             &list,
                             NULL);
  fail_unless(result != PBSE_NONE, "NULL input result_mask pointer fail: %d" ,result);

  result = status_nodeattrib(NULL,
                             &node_attributes,
                             &node,
                             0,
                             0,
                             &list,
                             &result_mask);
  /*FIXME: NOTE: this is probably a correct set of input parameters, but still returns -1*/
 //   fail_unless(result != PBSE_NONE, "NULL input svrattrl pointer fail: %d" ,result);

  result = status_nodeattrib(&attributes,
                             &node_attributes,
                             &node,
                             0,
                             0,
                             &list,
                             &result_mask);
  /*FIXME: NOTE: this is probably a correct set of input parameters, but still returns -1*/
  fail_unless(result != PBSE_NONE, "status_nodeattrib fail: %d" ,result);
  }
END_TEST

START_TEST(initialize_pbsnode_test)
  {
  struct pbsnode node;
  int result = -1;

  result = initialize_pbsnode(NULL, NULL, NULL, 0);
  fail_unless(result != PBSE_NONE, "NULL input node pointer fail");

  result = initialize_pbsnode(&node, NULL, NULL, 0);
  fail_unless(result == PBSE_NONE, "initialization fail");
  }
END_TEST

START_TEST(effective_node_delete_test)
  {
  struct pbsnode *node = NULL;

  initialize_all_nodes_array(&allnodes);
  //alloc mutex, use, restore
  pthread_mutex_t *mutex = allnodes.allnodes_mutex;
  allnodes.allnodes_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(allnodes.allnodes_mutex, NULL);

  /*accidental null pointer delete call*/
  effective_node_delete(NULL);
  effective_node_delete(&node);

  //pthread_mutex_init(allnodes.allnodes_mutex, NULL);
  node = (struct pbsnode *)malloc(sizeof(struct pbsnode));
  initialize_pbsnode(node, NULL, NULL, 0);
  effective_node_delete(&node);

  fail_unless(node == NULL, "unsuccessfull node delition %d", node);

  free((void*)allnodes.allnodes_mutex);
  allnodes.allnodes_mutex = mutex;
  }
END_TEST

START_TEST(update_nodes_file_test)
  {
  int result = -1;
  struct pbsnode node;
  initialize_pbsnode(&node, NULL, NULL, 0);

  result = update_nodes_file(NULL);
  fail_unless(result != PBSE_NONE, "update_nodes_file_test NULL input fail");

  result = update_nodes_file(&node);
  fail_unless(result != PBSE_NONE, "update_nodes_file_test empty node fail");
  }
END_TEST

START_TEST(recompute_ntype_cnts_test)
  {
  recompute_ntype_cnts();
  }
END_TEST

START_TEST(init_prop_test)
  {
  char name[] = "node_name";
  struct prop * result = init_prop(NULL);
  fail_unless(result->name == NULL, "NULL name init fail");

  result = init_prop(name);
  fail_unless(result->name == name, "name init fail");
  }
END_TEST

START_TEST(create_subnode_test)
  {
  struct pbsnode node;
  struct pbssubn *result = NULL;
  initialize_pbsnode(&node, NULL, NULL, 0);

  result = create_subnode(NULL);
  fail_unless(result == NULL, "NULL node pointer input fail");

  result = create_subnode(&node);
  fail_unless(result->host == &node, "create_subnode_test fail");
  }
END_TEST

START_TEST(create_a_gpusubnode_test)
  {
  int result = -1;
  struct pbsnode node;
  initialize_pbsnode(&node, NULL, NULL, 0);

  result = create_a_gpusubnode(NULL);
  fail_unless(result != PBSE_NONE, "NULL node pointer input fail");

  result = create_a_gpusubnode(&node);
  fail_unless(result == PBSE_NONE, "create_a_gpusubnode fail");
  }
END_TEST

START_TEST(copy_properties_test)
  {
  int result = -1;
  struct pbsnode source_node;
  struct pbsnode destanation_node;
  initialize_pbsnode(&source_node, NULL, NULL, 0);
  initialize_pbsnode(&destanation_node, NULL, NULL, 0);

  result = copy_properties(NULL, &source_node);
  fail_unless(result != PBSE_NONE, "NULL destanation pointer input fail");

  result = copy_properties(&destanation_node, NULL);
  fail_unless(result != PBSE_NONE, "NULL source pointer input fail");

  /*TODO: fill in source node*/
  result = copy_properties(&destanation_node, &source_node);
  fail_unless(result == PBSE_NONE, "copy_properties return fail");
  }
END_TEST

START_TEST(create_pbs_node_test)
  {
  int result = -1;
  char name[] = "name";
  char value[] = "value";
  int mask = 0;
  struct svrattrl attributes;
  memset(&attributes, 0, sizeof(attributes));
  attributes.al_atopl.value = value;

  result = create_pbs_node(NULL, &attributes, 0, &mask);
  fail_unless(result != PBSE_NONE, "NULL input name fail");

  result = create_pbs_node(name, NULL, 0, &mask);
  fail_unless(result != PBSE_NONE, "NULL input attributes fail");

  result = create_pbs_node(name, &attributes, 0, NULL);
  fail_unless(result != PBSE_NONE, "NULL input mask fail");

  /*TODO: mock properly in order to success*/
  result = create_pbs_node(name, &attributes, 0, &mask);
  fail_unless(result != PBSE_NONE, "create_pbs_node fail");
  }
END_TEST

START_TEST(setup_nodes_test)
  {
  int result = -1;

  result = setup_nodes();
  fail_unless(result == PBSE_NONE, "setup_nodes fail");
  }
END_TEST

START_TEST(node_np_action_test)
  {
  int result = -1;
  struct pbsnode node;
  struct pbs_attribute attributes;

  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&attributes, 0, sizeof(attributes));

  result = node_np_action(NULL, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input attributes fail");

  result = node_np_action(&attributes, NULL, ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input node fail");

  result = node_np_action(&attributes, (void*)(&node), 0);
  fail_unless(result != PBSE_NONE, "node_np_action fail");

  result = node_np_action(&attributes, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_NEW fail");

  result = node_np_action(&attributes, (void*)(&node), ATR_ACTION_ALTER);
  fail_unless(result == PBSE_BADATVAL, "ATR_ACTION_ALTER fail");
  }
END_TEST

START_TEST(node_mom_port_action_test)
  {
  int result = -1;
  struct pbsnode node;
  struct pbs_attribute attributes;

  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&attributes, 0, sizeof(attributes));

  result = node_mom_port_action(NULL, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input attributes fail");

  result = node_mom_port_action(&attributes, NULL, ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input node fail");

  result = node_mom_port_action(&attributes, (void*)(&node), 0);
  fail_unless(result == PBSE_INTERNAL, "node_np_action fail");

  result = node_mom_port_action(&attributes, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_NEW fail");

  result = node_mom_port_action(&attributes, (void*)(&node), ATR_ACTION_ALTER);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_ALTER fail");
  }
END_TEST

START_TEST(node_mom_rm_port_action_test)
  {
  int result = -1;
  struct pbsnode node;
  struct pbs_attribute attributes;

  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&attributes, 0, sizeof(attributes));

  result = node_mom_rm_port_action(NULL, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input attributes fail");

  result = node_mom_rm_port_action(&attributes, NULL, ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input node fail");

  result = node_mom_rm_port_action(&attributes, (void*)(&node), 0);
  fail_unless(result == PBSE_INTERNAL, "node_np_action fail");

  result = node_mom_rm_port_action(&attributes, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_NEW fail");

  result = node_mom_rm_port_action(&attributes, (void*)(&node), ATR_ACTION_ALTER);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_ALTER fail");
  }
END_TEST

START_TEST(node_gpus_action_test)
  {
  int result = -1;
  struct pbsnode node;
  struct pbs_attribute attributes;

  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&attributes, 0, sizeof(attributes));

  result = node_gpus_action(NULL, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input attributes fail");

  result = node_gpus_action(&attributes, NULL, ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input node fail");

  result = node_gpus_action(&attributes, (void*)(&node), 0);
  fail_unless(result == PBSE_INTERNAL, "node_np_action fail");

  result = node_gpus_action(&attributes, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_NEW fail");

  result = node_gpus_action(&attributes, (void*)(&node), ATR_ACTION_ALTER);
  fail_unless(result == PBSE_BADATVAL, "ATR_ACTION_ALTER fail");
  }
END_TEST

START_TEST(node_numa_action_test)
  {
  int result = -1;
  struct pbsnode node;
  struct pbs_attribute attributes;

  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&attributes, 0, sizeof(attributes));

  result = node_numa_action(NULL, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input attributes fail");

  result = node_numa_action(&attributes, NULL, ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input node fail");

  result = node_numa_action(&attributes, (void*)(&node), 0);
  fail_unless(result == PBSE_INTERNAL, "node_np_action fail");

  result = node_numa_action(&attributes, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_NEW fail");

  result = node_numa_action(&attributes, (void*)(&node), ATR_ACTION_ALTER);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_ALTER fail");
  }
END_TEST

START_TEST(numa_str_action_test)
  {
  int result = -1;
  struct pbsnode node;
  struct pbs_attribute attributes;

  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&attributes, 0, sizeof(attributes));

  result = numa_str_action(NULL, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input attributes fail");

  result = numa_str_action(&attributes, NULL, ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input node fail");

  result = numa_str_action(&attributes, (void*)(&node), 0);
  fail_unless(result == PBSE_INTERNAL, "node_np_action fail");

  result = numa_str_action(&attributes, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_NEW fail");

  result = numa_str_action(&attributes, (void*)(&node), ATR_ACTION_ALTER);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_ALTER fail");
  }
END_TEST

START_TEST(gpu_str_action_test)
  {
  int result = -1;
  struct pbsnode node;
  struct pbs_attribute attributes;

  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&attributes, 0, sizeof(attributes));

  result = gpu_str_action(NULL, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input attributes fail");

  result = gpu_str_action(&attributes, NULL, ATR_ACTION_NEW);
  fail_unless(result != PBSE_NONE, "NULL input node fail");

  result = gpu_str_action(&attributes, (void*)(&node), 0);
  fail_unless(result == PBSE_INTERNAL, "node_np_action fail");

  result = gpu_str_action(&attributes, (void*)(&node), ATR_ACTION_NEW);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_NEW fail");

  result = gpu_str_action(&attributes, (void*)(&node), ATR_ACTION_ALTER);
  fail_unless(result == PBSE_NONE, "ATR_ACTION_ALTER fail");
  }
END_TEST

START_TEST(create_partial_pbs_node_test)
  {
  int result = -1;
  char name[] = "name";
  initialize_all_nodes_array(&allnodes);
  //alloc mutex, use, restore
  pthread_mutex_t *mutex = allnodes.allnodes_mutex;
  allnodes.allnodes_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(allnodes.allnodes_mutex, NULL);

  result = create_partial_pbs_node(NULL, 0, 0);
  fail_unless(result != PBSE_NONE, "NULL input name fail");

  result = create_partial_pbs_node(name, 0, 0);
  fail_unless(result == PBSE_NONE, "create_partial_pbs_node fail");

  free((void*)allnodes.allnodes_mutex);
  allnodes.allnodes_mutex = mutex;
  }
END_TEST

START_TEST(next_node_test)
  {
  struct pbsnode *result = NULL;
  char name[] = "name";
  struct pbsnode node;
  struct node_iterator it;
  initialize_pbsnode(&node, NULL, NULL, 0);
  memset(&it, 0, sizeof(it));

  initialize_all_nodes_array(&allnodes);
  //alloc mutex, use, restore
  pthread_mutex_t *mutex = allnodes.allnodes_mutex;
  allnodes.allnodes_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(allnodes.allnodes_mutex, NULL);

  result = next_node(NULL, &node, &it);
  fail_unless(result == NULL, "NULL input all_nodes fail");

  /*TODO: NOTE: needs more complicated solution to get apropriate result*/
  result = next_node(&allnodes, NULL, &it);
  fail_unless(result == NULL, "NULL input pbsnode fail");

  result = next_node(&allnodes, &node, NULL);
  fail_unless(result == NULL, "NULL input iterator fail");

  result = next_node(&allnodes, &node, &it);
  fail_unless(result == NULL, "next_node fail");

  free((void*)allnodes.allnodes_mutex);
  allnodes.allnodes_mutex = mutex;
  }
END_TEST

START_TEST(insert_node_test)
  {
  struct all_nodes test_all_nodes;
  struct pbsnode node;
  int result = -1;

  initialize_all_nodes_array(&test_all_nodes);
  initialize_pbsnode(&node, NULL, NULL, 0);

  result = insert_node(NULL, &node);
  fail_unless(result != PBSE_NONE, "NULL input all_nodes pointer fail");

  result = insert_node(&test_all_nodes, NULL);
  fail_unless(result != PBSE_NONE, "NULL input pbsnode pointer fail");

  result = insert_node(&test_all_nodes, &node);
  fail_unless(result == PBSE_NONE, "insert_node fail");

  }
END_TEST

START_TEST(remove_node_test)
  {
  struct all_nodes test_all_nodes;
  struct pbsnode node;
  int result = -1;

  initialize_all_nodes_array(&test_all_nodes);
  initialize_pbsnode(&node, NULL, NULL, 0);

  result = remove_node(NULL, &node);
  fail_unless(result != PBSE_NONE, "NULL input all_nodes pointer fail");

  result = remove_node(&test_all_nodes, NULL);
  fail_unless(result != PBSE_NONE, "NULL input pbsnode pointer fail");

  result = remove_node(&test_all_nodes, &node);
  fail_unless(result == PBSE_NONE, "insert_node fail");

  }
END_TEST

START_TEST(next_host_test)
  {
  struct all_nodes test_all_nodes;
  struct pbsnode node;
  int it = 0;
  struct pbsnode *result = NULL;

  initialize_all_nodes_array(&test_all_nodes);
  initialize_pbsnode(&node, NULL, NULL, 0);

  result = next_host(NULL, &it, &node);
  fail_unless(result == NULL, "NULL input all_nodes pointer fail");

  result = next_host(&test_all_nodes, NULL, &node);
  fail_unless(result == NULL, "NULL input iterator pointer fail");

  result = next_host(&test_all_nodes, &it, NULL);
  fail_unless(result == NULL, "NULL input pbsnode pointer fail");

  result = next_host(&test_all_nodes, &it, &node);
  fail_unless(result == NULL, "insert_node fail");

  }
END_TEST

START_TEST(send_hierarchy_threadtask_test)
  {
  struct hello_info info;
  memset(&info, 0, sizeof(info));

  send_hierarchy_threadtask(NULL);
  send_hierarchy_threadtask((void*)(&info));
  }
END_TEST

START_TEST(send_hierarchy_test)
  {
  int result = -1;
  char* name = "name";

  result = send_hierarchy(NULL, 0);
  fail_unless(result != PBSE_NONE, "NULL input name fail");

  result = send_hierarchy(name, 0);
  fail_unless(result != PBSE_NONE, "send_hierarchy fail");
  }
END_TEST

START_TEST(initialize_hello_container_test)
  {
  struct hello_container container;
  struct hello_container* result = NULL;
  memset(&container, 0, sizeof(container));

  result = initialize_hello_container(NULL);
  fail_unless(result == NULL, "NULL input fail");

  result = initialize_hello_container(&container);
  fail_unless(result == &container, "wrong returned pointer");
  fail_unless(result->ra != NULL, "resizeble array was not initialized");

  result = initialize_hello_container(result);
  fail_unless(result->hello_mutex != NULL, "mutex was not initialized");

  }
END_TEST

START_TEST(add_hello_after_test)
  {
  struct hello_container container;
  char* node_name = "node_name";
  int result = -1;
  memset(&container, 0, sizeof(container));
  initialize_hello_container(&container);

  result = add_hello_after(NULL, node_name, 0);
  fail_unless(result != PBSE_NONE, "NULL input container pointer fail");

  result = add_hello_after(&container, NULL, 0);
  fail_unless(result == PBSE_NONE, "NULL input name pointer fail");

  result = add_hello_after(&container, node_name, 0);
  fail_unless(result == PBSE_NONE, "add_hello_after fail");

  }
END_TEST

START_TEST(add_hello_info_test)
  {
  struct hello_container container;
  struct hello_info info;
  int result = -1;
  memset(&container, 0, sizeof(container));
  memset(&info, 0, sizeof(info));
  initialize_hello_container(&container);

  result = add_hello_info(NULL, &info);
  fail_unless(result != PBSE_NONE, "NULL input container pointer fail");

  result = add_hello_info(&container, NULL);
  fail_unless(result == PBSE_NONE, "NULL input hello_info pointer fail");

  result = add_hello_info(&container, &info);
  fail_unless(result == PBSE_NONE, "add_hello_info fail");

  }
END_TEST

START_TEST(pop_hello_test)
  {
  struct hello_container container;
  struct hello_info *result = NULL;
  memset(&container, 0, sizeof(container));
  initialize_hello_container(&container);

  result = pop_hello(NULL);
  fail_unless(result == NULL, "NULL input container pointer fail");

  result = pop_hello(&container);
  fail_unless(result == NULL, "pop_hello fail");
  }
END_TEST

START_TEST(remove_hello_test)
  {
  struct hello_container container;
  char* node_name = "node_name";
  int result = -1;
  memset(&container, 0, sizeof(container));
  initialize_hello_container(&container);

  result = remove_hello(NULL, node_name);
  fail_unless(result != PBSE_NONE, "NULL input container pointer fail");

  result = remove_hello(&container, NULL);
  fail_unless(result != PBSE_NONE, "NULL input name pointer fail");

  result = remove_hello(&container, node_name);
  fail_unless(result == PBSE_NONE, "add_hello_after fail");

  }
END_TEST

Suite *node_func_suite(void)
  {
  Suite *s = suite_create("node_func_suite methods");
  TCase *tc_core = tcase_create("PGetNodeFromAddr_test");
  tcase_add_test(tc_core, PGetNodeFromAddr_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("bad_node_warning_test");
  tcase_add_test(tc_core, bad_node_warning_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("addr_ok_test");
  tcase_add_test(tc_core, addr_ok_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("initialize_all_nodes_array_test");
  tcase_add_test(tc_core, initialize_all_nodes_array_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("find_node_in_allnodes_test");
  tcase_add_test(tc_core, find_node_in_allnodes_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("find_nodebyname_test");
  tcase_add_test(tc_core, find_nodebyname_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("save_characteristic_test");
  tcase_add_test(tc_core, save_characteristic_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("chk_characteristic_test");
  tcase_add_test(tc_core, chk_characteristic_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("login_encode_jobs_test");
  tcase_add_test(tc_core, login_encode_jobs_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("status_nodeattrib_test");
  tcase_add_test(tc_core, status_nodeattrib_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("initialize_pbsnode_test");
  tcase_add_test(tc_core, initialize_pbsnode_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("effective_node_delete_test");
  tcase_add_test(tc_core, effective_node_delete_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("update_nodes_file_test");
  tcase_add_test(tc_core, update_nodes_file_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("recompute_ntype_cnts_test");
  tcase_add_test(tc_core, recompute_ntype_cnts_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("init_prop_test");
  tcase_add_test(tc_core, init_prop_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("create_subnode_test");
  tcase_add_test(tc_core, create_subnode_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("create_a_gpusubnode_test");
  tcase_add_test(tc_core, create_a_gpusubnode_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("copy_properties_test");
  tcase_add_test(tc_core, copy_properties_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("create_pbs_node_test");
  tcase_add_test(tc_core, create_pbs_node_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("setup_nodes_test");
  tcase_add_test(tc_core, setup_nodes_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("node_np_action_test");
  tcase_add_test(tc_core, node_np_action_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("node_mom_port_action_test");
  tcase_add_test(tc_core, node_mom_port_action_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("node_mom_rm_port_action_test");
  tcase_add_test(tc_core, node_mom_rm_port_action_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("node_gpus_action_test");
  tcase_add_test(tc_core, node_gpus_action_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("node_numa_action_test");
  tcase_add_test(tc_core, node_numa_action_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("numa_str_action_test");
  tcase_add_test(tc_core, numa_str_action_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("gpu_str_action_test");
  tcase_add_test(tc_core, gpu_str_action_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("create_partial_pbs_node_test");
  tcase_add_test(tc_core, create_partial_pbs_node_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("next_node_test");
  tcase_add_test(tc_core, next_node_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("insert_node_test");
  tcase_add_test(tc_core, insert_node_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("remove_node_test");
  tcase_add_test(tc_core, remove_node_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("next_host_test");
  tcase_add_test(tc_core, next_host_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("send_hierarchy_threadtask_test");
  tcase_add_test(tc_core, send_hierarchy_threadtask_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("send_hierarchy_test");
  tcase_add_test(tc_core, send_hierarchy_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("initialize_hello_container_test");
  tcase_add_test(tc_core, initialize_hello_container_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("add_hello_after_test");
  tcase_add_test(tc_core, add_hello_after_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("add_hello_info_test");
  tcase_add_test(tc_core, add_hello_info_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("pop_hello_test");
  tcase_add_test(tc_core, pop_hello_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("remove_hello_test");
  tcase_add_test(tc_core, remove_hello_test);
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
