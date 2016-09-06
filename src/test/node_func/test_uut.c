#include "pbs_config.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <sstream>
#include <fstream>
#include "license_pbs.h" /* See here for the software license */
#include "node_func.h"
#include "test_node_func.h"
#include "pbs_error.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "pbs_nodes.h" /* pbs_nodes, node_check_info, node_iterator, all_nodes */
#include "attribute.h" /* svrattrl, struct  */
#include "work_task.h"
#include "id_map.hpp"
#include "alps_constants.h"

//#define HOST_NAME_MAX 255

void write_compute_node_properties(struct pbsnode &reporter, FILE *nin);
void add_to_property_list(std::string &property_list, const char *token);
int login_encode_jobs(struct pbsnode *pnode, tlist_head *phead);
extern bool cray_enabled;
int read_val_and_advance(int *val, const char **str);
char *parse_node_token(char **start, int flags, int *err, char *term);
int add_node_attribute_to_list(char *token, char **line_ptr, tlist_head *atrlist_ptr, int linenum);
void add_node_property(std::string &propstr, const char *token, bool &is_alps_starter, bool &is_alps_reporter, bool &is_alps_compute);
int record_node_property_list(std::string const &propstr, tlist_head *atrlist_ptr);
void handle_cray_specific_node_values(char *nodename, bool is_alps_reporter, bool is_alps_starter, bool is_alps_compute, svrattrl *pal);
char *parse_node_name(char **ptr, int &err, int linenum);
void load_node_notes();

void attrlist_free();

extern svrattrl *s;
extern std::string attrname;
extern std::string attrval;


extern char *path_nodenote;

void initialize_allnodes(all_nodes *an, struct pbsnode *n1, struct pbsnode *n2)
  {
  an->lock();
  an->clear();
  if(n1 != NULL) an->insert(n1,n1->get_name());
  if(n2 != NULL) an->insert(n2,n2->get_name());
  an->unlock();
  }


START_TEST(add_node_property_test)
  {
  std::string props;
  bool        is_alps_reporter = false;
  bool        is_alps_starter = false;
  bool        is_alps_compute = false;

  add_node_property(props, "cray_compute", is_alps_starter, is_alps_reporter, is_alps_compute);
  fail_unless(props == "cray_compute");
  fail_unless(is_alps_compute == true);
  fail_unless(is_alps_starter == false);
  fail_unless(is_alps_reporter == false);

  is_alps_compute = false;
  props.clear();
  add_node_property(props, alps_starter_feature, is_alps_starter, is_alps_reporter, is_alps_compute);
  fail_unless(props == alps_starter_feature);
  fail_unless(is_alps_compute == false);
  fail_unless(is_alps_starter == true);
  fail_unless(is_alps_reporter == false);
  is_alps_starter = false;

  props.clear();
  add_node_property(props, alps_reporter_feature, is_alps_starter, is_alps_reporter, is_alps_compute);
  fail_unless(props == "cray_compute");
  fail_unless(is_alps_compute == false);
  fail_unless(is_alps_starter == false);
  fail_unless(is_alps_reporter == true);

  is_alps_reporter = false;
  props.clear();
  add_node_property(props, "bigmem", is_alps_starter, is_alps_reporter, is_alps_compute);
  fail_unless(props == "bigmem");
  fail_unless(is_alps_compute == false);
  fail_unless(is_alps_starter == false);
  fail_unless(is_alps_reporter == false);
  }
END_TEST


START_TEST(record_node_property_list_test)
  {
  tlist_head th;
  std::string props("bigmem fast");
  std::string empty;

  attrname.clear();
  attrval.clear();
  CLEAR_HEAD(th);

  // empty list should do nothing
  fail_unless(record_node_property_list(empty, &th) == PBSE_NONE);
  fail_unless(attrname.size() == 0);
  fail_unless(attrval.size() == 0);
  attrlist_free();
  attrname.clear();
  attrval.clear();

/*  fail_unless(record_node_property_list(props, &th) == PBSE_NONE);
  fail_unless(attrname == ATTR_NODE_properties);
  fail_unless(attrval == props);
  attrlist_free();
  attrname.clear();
  attrval.clear();*/

  }
END_TEST


START_TEST(handle_cray_specific_node_values_test)
  {
  extern int created_subnode;

  created_subnode = 0;

  // nothing should happen if cray_enabled == false
  cray_enabled = false;
  handle_cray_specific_node_values(NULL, false, false, true, NULL);
  fail_unless(created_subnode == 0);

  // should create subnode with cray enabled and is_alps_compute set to true
  cray_enabled = true;
  handle_cray_specific_node_values(NULL, false, false, true, NULL);
  fail_unless(created_subnode == 1);
  }
END_TEST


START_TEST(parse_node_name_test)
  {
  char line[1024];
  char *ptr;
  int   err;
  char *hostname;

  // invalid characters should fail
  sprintf(line, "*$!@!@");
  ptr = line;
  cray_enabled = false;
  fail_unless(parse_node_name(&ptr, err, 1) == NULL);
  fail_unless(err != PBSE_NONE);
  
  err = PBSE_NONE;
  sprintf(line, "napali");
  ptr = line;
  fail_unless((hostname = parse_node_name(&ptr, err, 1)) != NULL);
  fail_unless(err == PBSE_NONE);
  fail_unless(!strcmp(hostname, "napali"));
  
  err = PBSE_NONE;
  sprintf(line, "1napali");
  ptr = line;
  fail_unless(parse_node_name(&ptr, err, 1) == NULL);
  fail_unless(err != PBSE_NONE);
  
  err = PBSE_NONE;
  sprintf(line, "1214");
  ptr = line;
  cray_enabled = true;
  fail_unless((hostname = parse_node_name(&ptr, err, 1)) != NULL);
  fail_unless(err == PBSE_NONE);
  fail_unless(!strcmp(hostname, "1214"));
  }
END_TEST


START_TEST(add_node_attribute_to_list_test)
  {
  char  line[1024];
  char *ptr;
  tlist_head th;
  int ret;
  svrattrl *sattr;

  CLEAR_HEAD(th);

  // this should work and create the attribute np with the value of 100
  snprintf(line, sizeof(line), "100");
  ptr = line;

  ret = add_node_attribute_to_list(strdup("np"), &ptr, &th, 1);
  fail_unless(ret == PBSE_NONE);
  sattr = (svrattrl *)GET_NEXT(th); 
  fail_unless(strcmp(sattr->al_name, "np") == 0);
  fail_unless(strcmp(sattr->al_value, "100") == 0);
  attrlist_free();
  attrname.clear();
  attrval.clear();
  CLEAR_HEAD(th);

  // this is invalid syntax
  snprintf(line, sizeof(line), "100=");
  ptr = line;
  fail_unless(add_node_attribute_to_list(strdup("np"), &ptr, &th, 1) != PBSE_NONE);

  CLEAR_HEAD(th);
  // run over the two special cases
  snprintf(line, sizeof(line), "100");
  ptr = line;
  fail_unless(add_node_attribute_to_list(strdup("TTL"), &ptr, &th, 1) == PBSE_NONE);
  sattr = (svrattrl *)GET_NEXT(th); 
  fail_unless(strcmp(sattr->al_name, "TTL") == 0);
  fail_unless(strcmp(sattr->al_value, "100") == 0);
  attrlist_free();
  attrname.clear();
  attrval.clear();

  CLEAR_HEAD(th);
  snprintf(line, sizeof(line), "bob,tom");
  ptr = line;
  fail_unless(add_node_attribute_to_list(strdup("acl"), &ptr, &th, 1) == PBSE_NONE);
  sattr = (svrattrl *)GET_NEXT(th); 
  fail_unless(strcmp(sattr->al_name, "acl") == 0);
  fail_unless(strcmp(sattr->al_value, "bob,tom") == 0);
  attrlist_free();
  attrname.clear();
  attrval.clear();
  }
END_TEST

/*
START_TEST(test_load_node_notes)
  {
  struct pbsnode  node1;
  struct pbsnode  node2;
  struct pbsnode  node3;
  struct pbsnode  node4;
  
  node1.change_name("node01");
  node2.change_name("node02");
  node3.change_name("node03");
  node4.change_name("node04");
  path_nodenote = strdup("test_notes.txt");
  initialize_allnodes(&allnodes, &node1, &node2);
  allnodes.insert(&node3, node3.get_name());
  allnodes.insert(&node4, node4.get_name());

  load_node_notes();
  fail_unless(strstr(node1.nd_note.c_str(), "1-minute load average too high") != NULL);
  fail_unless(node2.nd_note == "cloning issues");
  fail_unless(strstr(node3.nd_note.c_str(), "Health check failed:") != NULL);
  fail_unless(node4.nd_note == "Needs BIOS update");
  }
END_TEST*/


START_TEST(parse_node_token_test)
  {
  char *line = strdup("bob tom marty");
  char *ptr = line;
  int   err;
  char  term;

  fail_unless(!strcmp(parse_node_token(&ptr, 0, &err, &term), "bob"));
  fail_unless(!strcmp(ptr, "tom marty"), ptr);
  fail_unless(!strcmp(parse_node_token(&ptr, 0, &err, &term), "tom"));
  fail_unless(!strcmp(ptr, "marty"), ptr);
  fail_unless(!strcmp(parse_node_token(&ptr, 0, &err, &term), "marty"));
  fail_unless(parse_node_token(&ptr, 0, &err, &term) == NULL);
  }
END_TEST


START_TEST(read_val_and_advance_test)
  {
  int   val;
  const char *str = NULL;

  fail_unless(read_val_and_advance(NULL, &str) == PBSE_BAD_PARAMETER);
  fail_unless(read_val_and_advance(&val, &str) == PBSE_BAD_PARAMETER);

  str = strdup("64,16,16");
  fail_unless(read_val_and_advance(&val, &str) == PBSE_NONE);
  fail_unless(val == 64);
  fail_unless(read_val_and_advance(&val, &str) == PBSE_NONE);
  fail_unless(val == 16);
  fail_unless(read_val_and_advance(&val, &str) == PBSE_NONE);
  fail_unless(val == 16);
  }
END_TEST


START_TEST(node_exists_check)
  {
  pbsnode  node1;
  pbsnode  node2;
  
  node1.change_name("bob");
  node2.change_name("tom");

  initialize_allnodes(&allnodes, &node1, &node2);
  fail_unless(node_exists("bob") == true);
  fail_unless(node_exists("tom") == true);
  fail_unless(node_exists("joe") == false);
  }
END_TEST
  

START_TEST(add_to_property_list_test)
  {
  std::string properties;

  add_to_property_list(properties, NULL);
  add_to_property_list(properties, "one");
  add_to_property_list(properties, "two");
  add_to_property_list(properties, "three");
  fail_unless(!strcmp(properties.c_str(), "one,two,three"));
  }
END_TEST


START_TEST(addr_ok_test)
  {
  pbs_net_t address = 0;
  pbsnode node;
  int result;

  result = addr_ok(address, NULL);
  fail_unless(result == 1, "NULL node input fail: %d", result);

  result = addr_ok(address, &node);
  fail_unless(result == 1, "empty node input fail: %d", result);

  }
END_TEST


START_TEST(find_node_in_allnodes_test)
  {
  struct pbsnode *result = NULL;
  all_nodes allnodes;
  pbsnode test_node;
  const char *test_node_name = "test_node";
  test_node.change_name(test_node_name);

  result = find_node_in_allnodes(NULL, (char *)"nodename");
  fail_unless(result == NULL, "NULL input all_nodes struct pointer fail");

  result = find_node_in_allnodes(&allnodes, NULL);
  fail_unless(result == NULL, "NULL input nodename fail");

  result = find_node_in_allnodes(&allnodes, (char *)"nodename");
  fail_unless(result == NULL, "find unexpected node fail");

/* TODO: think about apropriate mocking for the test below
  insert_node(&allnodes, &test_node);
  result = find_node_in_allnodes(&allnodes, "test_node");
  fail_unless(result == &test_node, "find node fail");
*/
  }
END_TEST


START_TEST(login_encode_jobs_test)
  {
  pbsnode   node;
  struct list_link list;
  int              result;
  memset(&list, 0, sizeof(list));

  result = login_encode_jobs(NULL, &list);
  fail_unless(result != PBSE_NONE, "NULL input node pointer fail");

  result = login_encode_jobs(&node, NULL);
  fail_unless(result != PBSE_NONE, "NULL input list_link pointer fail");

  result = login_encode_jobs(&node, &list);
  fail_unless(result == PBSE_NONE, "login_encode_jobs_test fail");
  }
END_TEST


START_TEST(find_nodebyname_test)
  {
  struct pbsnode  node1;
  struct pbsnode  node2;
  struct pbsnode  reporter;
  struct pbsnode *pnode;

  alps_reporter = &reporter;

  node1.change_name("bob");
  node2.change_name("tom");
  alps_reporter->alps_subnodes = new all_nodes();
  initialize_allnodes(&allnodes, &node1, &node2);
  initialize_allnodes(alps_reporter->alps_subnodes, &node1, &node2);

  cray_enabled = false;

  pnode = find_nodebyname(NULL);
  fail_unless(pnode == NULL, "NULL nodename input fail");

  pnode = find_nodebyname("george");
  fail_unless(pnode == NULL, "george found but doesn't exist");

  pnode = find_nodebyname("bob");
  fail_unless(pnode == &node1, "couldn't find bob?");
  pnode->unlock_node("a", "b", 0);

  pnode = find_nodebyname("tom");
  fail_unless(pnode == &node2, "couldn't find tom?");
  pnode->unlock_node("a", "b", 0);

  pnode = find_nodebyname(strdup("tom-0"));
  fail_unless(!strcmp(pnode->get_name(), "0"), "found an incorrect node name");
  pnode->unlock_node("a", "b", 0);

  pnode = find_nodebyname(strdup("tom-1"));
  fail_unless(!strcmp(pnode->get_name(), "1"), "found an incorrect node name");
  pnode->unlock_node("a", "b", 0);

  pnode = find_nodebyname(strdup("tom-10"));
  fail_unless(pnode == NULL, "found an incorrect node name");

  pnode = find_nodebyname(strdup("bob/0"));
  fail_unless(pnode == &node1, "couldn't find bob with the exec_host format");
  pnode->unlock_node("a", "b", 0);

  allnodes.lock();
  allnodes.clear();
  allnodes.unlock();

  cray_enabled = true;

  pnode = find_nodebyname("tom");
  fail_unless(pnode == &node2, "couldn't find tom?");

  cray_enabled = true;

  pnode = find_nodebyname("bob");
  fail_unless(pnode == &node1, "couldn't find bob?");

  cray_enabled = true;

  pnode = find_nodebyname("george");
  fail_unless(pnode == NULL, "george found but doesn't exist");
  alps_reporter = NULL;

  }
END_TEST


START_TEST(add_execution_slot_test)
  {
  pbsnode node;
  int result = 0;

  result = add_execution_slot(NULL);
  fail_unless(result == PBSE_RMBADPARAM, "NULL node pointer input fail");

  result = add_execution_slot(&node);
  fail_unless(result == PBSE_NONE, "add_execution_slot_test fail");
  }
END_TEST


/*
START_TEST(save_characteristic_test)
  {
  pbsnode node;
  node_check_info node_info;
  save_characteristic(NULL, &node_info);
  save_characteristic(&node, NULL);
  save_characteristic(&node, &node_info);
  }
END_TEST


START_TEST(chk_characteristic_test)
  {
  pbsnode node;
  node_check_info node_info;
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


START_TEST(status_nodeattrib_test)
  {
  struct svrattrl attributes;
  struct attribute_def node_attributes;
  pbsnode node;
  struct list_link list;
  int result_mask = 0;
  int result = 0;

  memset(&attributes, 0, sizeof(attributes));
  memset(&node_attributes, 0, sizeof(node_attributes));
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
  }
END_TEST


START_TEST(effective_node_delete_test)
  {
  struct pbsnode *node = NULL;

  allnodes.lock();
  allnodes.clear();
  allnodes.unlock();

  // accidental null pointer delete call
  effective_node_delete(NULL);
  effective_node_delete(&node);

  // pthread_mutex_init(allnodes.allnodes_mutex, NULL);
  // delete shouldn't work with nameless node
  node = new pbsnode();
  effective_node_delete(&node);
  fail_unless(node != NULL, "shouldn't delete a nameless node");
  node->change_name("nodename");
  effective_node_delete(&node);
  fail_unless(node != NULL, "shouldn't delete a non-inserted node");

  allnodes.insert(node, node->get_name());
  effective_node_delete(&node);
  fail_unless(node == NULL, "unsuccessful node deletion %d", node);

  }
END_TEST


START_TEST(update_nodes_file_test)
  {
  int result = -1;
  pbsnode node;

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
  struct prop *result = init_prop(name);
  fail_unless(result->name == name, "name init fail");
  }
END_TEST 

START_TEST(create_a_gpusubnode_test)
  {
  int result = -1;
  pbsnode node;

  result = create_a_gpusubnode(NULL);
  fail_unless(result != PBSE_NONE, "NULL node pointer input fail");

  result = create_a_gpusubnode(&node);
  fail_unless(result == PBSE_NONE, "create_a_gpusubnode fail");
  }
END_TEST

START_TEST(copy_properties_test)
  {
  int result = -1;
  pbsnode source_node;
  pbsnode destination_node;

  result = source_node.copy_properties(NULL);
  fail_unless(result != PBSE_NONE, "NULL destanation pointer input fail");

  // TODO: fill in source node
  result = source_node.copy_properties(&destination_node);
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
  pbsnode node;
  struct pbs_attribute attributes;

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
END_TEST */

START_TEST(node_mom_port_action_test)
  {
  int result = -1;
  pbsnode node;
  struct pbs_attribute attributes;

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
  pbsnode node;
  struct pbs_attribute attributes;

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
  pbsnode node;
  struct pbs_attribute attributes;

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
  pbsnode node;
  struct pbs_attribute attributes;

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
  pbsnode node;
  struct pbs_attribute attributes;

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
  pbsnode node;
  struct pbs_attribute attributes;

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

  allnodes.lock();
  allnodes.clear();
  allnodes.unlock();

  result = create_partial_pbs_node(NULL, 0, 0);
  fail_unless(result != PBSE_NONE, "NULL input name fail");

  result = create_partial_pbs_node(name, 0, 0);
  fail_unless(result == PBSE_NONE, "create_partial_pbs_node fail");
  }
END_TEST

START_TEST(next_node_test)
  {
  struct pbsnode *result = NULL;
  pbsnode node;
  struct node_iterator it;
  memset(&it, 0, sizeof(it));
  it.node_index = NULL;

  allnodes.lock();
  allnodes.clear();
  allnodes.unlock();

  result = next_node(NULL, &node, &it);
  fail_unless(result == NULL, "NULL input all_nodes fail");

  /*TODO: NOTE: needs more complicated solution to get apropriate result*/
  result = next_node(&allnodes, NULL, &it);
  fail_unless(result == NULL, "NULL input pbsnode fail");

  result = next_node(&allnodes, &node, NULL);
  fail_unless(result == NULL, "NULL input iterator fail");

  result = next_node(&allnodes, &node, &it);
  fail_unless(result == NULL, "next_node fail");
  }
END_TEST

START_TEST(insert_node_test)
  {
  all_nodes test_all_nodes;
  pbsnode node;
  int result = -1;

  test_all_nodes.lock();
  test_all_nodes.clear();
  test_all_nodes.unlock();

  result = insert_node(NULL, &node);
  fail_unless(result != PBSE_NONE, "NULL input all_nodes pointer fail");

  result = insert_node(&test_all_nodes, NULL);
  fail_unless(result != PBSE_NONE, "NULL input pbsnode pointer fail");

  node.change_name("node_name");

  result = insert_node(&test_all_nodes, &node);
  fail_unless(result == PBSE_NONE, "insert_node fail");

  }
END_TEST

START_TEST(remove_node_test)
  {
  all_nodes test_all_nodes;
  pbsnode node;
  int result = -1;

  result = remove_node(NULL, &node);
  fail_unless(result != PBSE_NONE, "NULL input all_nodes pointer fail");

  result = remove_node(&test_all_nodes, NULL);
  fail_unless(result != PBSE_NONE, "NULL input pbsnode pointer fail");

  result = remove_node(&test_all_nodes, &node);
  fail_unless(result != PBSE_NONE, "remove_node fail");

  node.change_name("nodeName");
  result = remove_node(&test_all_nodes, &node);
  fail_unless(result != PBSE_NONE, "node wasn't present in set");

  test_all_nodes.insert(&node, node.get_name());
  result = remove_node(&test_all_nodes, &node);
  fail_unless(result == PBSE_NONE, "couldn't remove node that was present");
  }
END_TEST

START_TEST(next_host_test)
  {
  all_nodes test_all_nodes;
  pbsnode node;
  all_nodes_iterator *it = NULL;
  struct pbsnode *result = NULL;

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

Suite *node_func_suite(void)
  {
  Suite *s = suite_create("node_func_suite methods");
  TCase *tc_core = tcase_create("addr_ok_test");
  tcase_add_test(tc_core, addr_ok_test);
  tcase_add_test(tc_core, login_encode_jobs_test);
  tcase_add_test(tc_core, find_nodebyname_test);
  tcase_add_test(tc_core, find_node_in_allnodes_test);
  suite_add_tcase(s, tc_core);

  /*tc_core = tcase_create("find_node_in_allnodes_test");
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("save_characteristic_test");
  tcase_add_test(tc_core, save_characteristic_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("chk_characteristic_test");
  tcase_add_test(tc_core, chk_characteristic_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("status_nodeattrib_test");
  tcase_add_test(tc_core, status_nodeattrib_test);
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

  tc_core = tcase_create("create_a_gpusubnode_test");
  tcase_add_test(tc_core, create_a_gpusubnode_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("copy_properties_test");
  tcase_add_test(tc_core, copy_properties_test);
  tcase_add_test(tc_core, test_load_node_notes);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("create_pbs_node_test");
  tcase_add_test(tc_core, create_pbs_node_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("setup_nodes_test");
  tcase_add_test(tc_core, setup_nodes_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("node_np_action_test");
  tcase_add_test(tc_core, node_np_action_test);
  suite_add_tcase(s, tc_core);*/

  tc_core = tcase_create("node_mom_port_action_test");
  tcase_add_test(tc_core, node_mom_port_action_test);
  tcase_add_test(tc_core, node_mom_rm_port_action_test);
  tcase_add_test(tc_core, add_execution_slot_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("node_gpus_action_test");
  tcase_add_test(tc_core, node_gpus_action_test);
  tcase_add_test(tc_core, node_numa_action_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("numa_str_action_test");
  tcase_add_test(tc_core, numa_str_action_test);
  tcase_add_test(tc_core, gpu_str_action_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("create_partial_pbs_node_test");
  tcase_add_test(tc_core, create_partial_pbs_node_test);
  tcase_add_test(tc_core, next_node_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("insert_node_test");
  tcase_add_test(tc_core, insert_node_test);
  tcase_add_test(tc_core, remove_node_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("next_host_test");
  tcase_add_test(tc_core, next_host_test);
  tcase_add_test(tc_core, add_to_property_list_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("add_node_attribute_to_list_test");
  tcase_add_test(tc_core, add_node_attribute_to_list_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("add_node_property_test");
  tcase_add_test(tc_core, add_node_property_test);
  tcase_add_test(tc_core, record_node_property_list_test);
  tcase_add_test(tc_core, node_exists_check);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("read_val_and_advance_test");
  tcase_add_test(tc_core, read_val_and_advance_test);
  tcase_add_test(tc_core, parse_node_token_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("handle_cray_specific_node_values_test");
  tcase_add_test(tc_core, handle_cray_specific_node_values_test);
  tcase_add_test(tc_core, parse_node_name_test);
  suite_add_tcase(s, tc_core);

#if 0
  /* Will uncoment this test once we use the real, append_link, etc */
  tc_core = tcase_create("create_pbs_node_real_test");
  tcase_add_test(tc_core, create_pbs_node_real_test);
  suite_add_tcase(s, tc_core);
#endif

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
