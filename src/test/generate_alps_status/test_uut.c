#include <stdio.h>
#include "alps_constants.h"
#include "alps_functions.h"
#include <string>
#include <vector>
#include <check.h>

int log_event_called = 0;


int parse_alps_output(std::string& alps_output);
void update_status(std::vector<std::string> &status);

void log_event( int eventtype, int objclass, const char *objname, const char *text);



const char *sample_start1 = "<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> ";
const char *sample_start2 = "<ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
const char *sample_start3 = "</Processor> <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='5' architecture='x86_64' ";
const char *sample_start4 = "clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
const char *sample_start5 = "</Processor> <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='10' architecture='x86_64' ";
const char *sample_start6 = "clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
const char *sample_start7 = "</Processor> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='3' architecture='x86_64' ";
const char *sample_start8 = "clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
const char *sample_start9 = "</Processor> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='8' architecture='x86_64' ";
const char *sample_start10 = "clock_mhz='2100'/> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/> </ProcessorArray> <MemoryArray> ";
const char *sample_start11 = "<Memory type='OS' page_size_kb='4' page_count='4096000'> <MemoryAllocation reservation_id='2869' page_count='6824960'/> </Memory> </MemoryArray> <LabelArray> <Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/> </LabelArray> </Node> <Node node_id='6143' name='c3-0c0s0n1' ";
const char *sample_start12 = "architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='3' architecture='x86_64' ";
const char *sample_start13 = "clock_mhz='2100'/> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='8' ";
const char *sample_start14 = "architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='12' architecture='x86_64' clock_mhz='2100'/> ";
const char *sample_start15 = "<Processor ordinal='13' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='14' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='15' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='16' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='17' architecture='x86_64' ";
const char *sample_start16 = "clock_mhz='2100'/> <Processor ordinal='18' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='19' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='20' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='21' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='22' ";
const char *sample_start17 = "architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='23' architecture='x86_64' clock_mhz='2100'/> </ProcessorArray> <MemoryArray> <Memory type='OS' page_size_kb='4' page_count='8192000'/> </MemoryArray> <LabelArray> <Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/> ";
const char *sample_start18 = "</LabelArray> </Node> <Node node_id='6144' name='c3-0c0s0n2' architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='2' architecture='x86_64' ";
const char *sample_start19 = "clock_mhz='2100'/> <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='7' ";
const char *sample_start20 = "architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/> ";
const char *sample_start21 = "<Processor ordinal='12' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='13' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='14' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='15' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='16' architecture='x86_64' ";
const char *sample_start22 = "clock_mhz='2100'/> <Processor ordinal='17' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='18' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='19' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='20' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='21' ";
const char *sample_start23 = "architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='22' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='23' architecture='x86_64' clock_mhz='2100'/> </ProcessorArray> <MemoryArray> <Memory type='OS' page_size_kb='4' page_count='8192000'/> </MemoryArray> <LabelArray> ";
const char *sample_start24 = "<Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/> </LabelArray> </Node> </NodeArray> <ReservationArray> <Reservation reservation_id='1772' user_name='pgarias' account_name='DEFAULT'/> <Reservation reservation_id='2549' user_name='daryal' account_name='DEFAULT'/> <Reservation reservation_id='2869' user_name='zachary' account_name='DEFAULT'/> </ReservationArray> </Inventory> </ResponseData> </BasilResponse> ";

const char *sample_norole_withres = "<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
const char *sample_norole_nores = "<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
const char *sample_batrole_withres = "<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
const char *sample_batrole_nores = "<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
const char *sample_introle_withres = "<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='INTERACTIVE' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
const char *sample_introle_nores = "<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='INTERACTIVE' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
const char *sample_unkrole_withres = "<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='UNKNOWN' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
const char *sample_unkrole_nores = "<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='UNKNOWN' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
const char *sample_failure_downnode = "<?xml version='1.0'?><BasilResponse protocol='1.2'> <ResponseData status='FAILURE' method='QUERY' error_class='PERMANENT' error_source='BACKEND'> <Message severity='ERROR'>Node mapping mismatch: SDB = 15309, ALPS = 15410.</Message> </ResponseData> </BasilResponse>";

int search_dynamic_string_status(std::vector<std::string> &status, char *str);
int generate_alps_status(std::vector<std::string> &status, const char *path, const char *protocol);
int get_knl_information(const char *apbasil_path);

START_TEST(get_knl_information_test)
  {
  const char *path = "../test_scripts/get_inventory_knl.sh";
  
  alps_nodes.clear();
  fail_unless(get_knl_information(path) == PBSE_NONE);
  fail_unless(alps_nodes.find(6142) != alps_nodes.end());
  fail_unless(alps_nodes.find(6143) != alps_nodes.end());
  fail_unless(alps_nodes[6142].os == "opsys=CLE_a2a_flat", alps_nodes[6142].os.c_str());
  fail_unless(alps_nodes[6142].hbm == "hbmem=4096000kb", alps_nodes[6142].hbm.c_str());
  }
END_TEST


START_TEST(parse_alps_output_test)
  {
  std::string output(sample_start1);
  std::vector<std::string> status;
  int             rc;

  output += sample_start2;
  output += sample_start3;
  output += sample_start4;
  output += sample_start5;
  output += sample_start6;
  output += sample_start7;
  output += sample_start8;
  output += sample_start9;
  output += sample_start10;
  output += sample_start11;
  output += sample_start12;
  output += sample_start13;
  output += sample_start14;
  output += sample_start15;
  output += sample_start16;
  output += sample_start17;
  output += sample_start18;
  output += sample_start19;
  output += sample_start20;
  output += sample_start21;
  output += sample_start22;
  output += sample_start23;
  output += sample_start24;
  
  rc = parse_alps_output(output);
  update_status(status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"6142") == 1, "Couldn't find node 6142 in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"6143") == 1, "Couldn't find node 6143 in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"6144") == 1, "Couldn't find node 6144 in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"CPROC") == 1, "Couldn't find CPROC in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"APROC") == 1, "Couldn't find APROC in the status");

  // role is not specified, reservation id set

  output.clear();
  status.clear();
  alps_nodes.clear();

  output = sample_norole_withres;

  rc = parse_alps_output(output);
  update_status(status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=") == 1, "Couldn't find reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=BUSY") == 1, "Couldn't find state in the status");

  // role is not specified, reservation id not set

  output.clear();
  status.clear();
  alps_nodes.clear();

  output = sample_norole_nores;

  rc = parse_alps_output(output);
  update_status(status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=") == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=UP") == 1, "Couldn't find state in the status");

  // role is BATCH, reservation id set

  output.clear();
  status.clear();
  alps_nodes.clear();

  output = sample_batrole_withres;

  rc = parse_alps_output(output);
  update_status(status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=") == 1, "Couldn't find reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=BUSY") == 1, "Couldn't find state in the status");

  // role is BATCH, reservation id not set

  output.clear();
  status.clear();
  alps_nodes.clear();

  output = sample_batrole_nores;

  rc = parse_alps_output(output);
  update_status(status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=") == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=UP") == 1, "Couldn't find state in the status");
 
  // role is interactive, reservation id set

  output.clear();
  status.clear();
  alps_nodes.clear();

  output = sample_introle_withres;

  rc = parse_alps_output(output);
  update_status(status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=") == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=BUSY") == 1, "Couldn't find state in the status");

  // role is interactive, reservation id not set

  output.clear();
  status.clear();
  alps_nodes.clear();

  output = sample_introle_nores;

  rc = parse_alps_output(output);
  update_status(status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=") == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=DOWN") == 1, "Couldn't find state in the status");

  // role is unknown, reservation id set

  output.clear();
  status.clear();
  alps_nodes.clear();

  output = sample_unkrole_withres;

  rc = parse_alps_output(output);
  update_status(status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=") == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=BUSY") == 1, "Couldn't find state in the status");

  // role is unknown, reservation id not set

  output.clear();
  status.clear();
  alps_nodes.clear();

  output = sample_unkrole_nores;

  rc = parse_alps_output(output);
  update_status(status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=") == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=DOWN") == 1, "Couldn't find state in the status");

  output.clear();
  status.clear();
  alps_nodes.clear();
  
  extern int      LOGLEVEL;
  extern std::string msg_err;
  LOGLEVEL = 10;

  output = sample_failure_downnode;

  rc = parse_alps_output(output);
  update_status(status);
  fail_unless(rc != 0, "Couldn't parse alps output");
  fail_unless(msg_err == "Failed to query ALPS: Node mapping mismatch: SDB = 15309, ALPS = 15410.", "error msg = '%s'", msg_err.c_str());
  } 
END_TEST 

START_TEST(parse_alps13_output_test)
  {
  /* test 1.3 protocol strings */

  #define ALPS_13_INPUT_FILE "basil_13_short.xml"

  std::string output = "";
  std::vector<std::string> status;
  int             rc;
  FILE           *fp;
  char            linebuf[1024];

  if ((fp = fopen(ALPS_13_INPUT_FILE, "r")) == NULL)
    ck_abort_msg("Couldn't open ALPS 13 input file %s", ALPS_13_INPUT_FILE);
  else
    {

    while (fgets(linebuf, sizeof(linebuf), fp) != NULL)
      output += linebuf;

    fclose(fp);
    
    rc = parse_alps_output(output);
    update_status(status);
    fail_unless(rc == 0, "Couldn't parse ALPS 1.3 output contained in file %s", ALPS_13_INPUT_FILE);

    fail_unless(search_dynamic_string_status(status, (char *)"6142") == 1, "Couldn't find node 6142 in the 1.3 status");
    fail_unless(search_dynamic_string_status(status, (char *)"CPROC") == 1, "Couldn't find CPROC in the 1.3 status");
    fail_unless(search_dynamic_string_status(status, (char *)"APROC") == 1, "Couldn't find APROC in the 1.3 status");
    fail_unless(search_dynamic_string_status(status, (char *)"CCU") == 1, "Couldn't find CCU in the 1.3 status");
    }

  }
END_TEST

START_TEST(full_generate_test)
  {
  std::vector<std::string> status;
  int             rc;
  char           *path = (char *)"../test_scripts/get_inventory.sh";
  char           *protocol = (char *)"1.0";

  rc = generate_alps_status(status, path, protocol);

  fail_unless(rc == 0, "Couldn't generate the status");

  if (rc == 0)
    fail_unless(log_event_called == 1, "log_event was not called within successful generate_alps_status()");

  fail_unless(search_dynamic_string_status(status, (char *)"GPU") > 0, "Couldn't find the GPUs reported in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"cheeseburger") > 0, "Couldn't find the feature cheeseburger in the status");
  }
END_TEST 

START_TEST(label_generate_test)
  {
  std::vector<std::string> status;
  int             rc;
  char           *path = (char *)"../test_scripts/label_inventory.sh";
  char           *protocol = (char *)"1.0";
  
  rc = generate_alps_status(status, path, protocol);

  fail_unless(rc == 0, "Couldn't generate the status");
  fail_unless(search_dynamic_string_status(status, (char *)"regmem") == 1);
  fail_unless(search_dynamic_string_status(status, (char *)"regmem,") != 1);
  fail_unless(search_dynamic_string_status(status, (char *)"regmemregmem") != 1);
  }
END_TEST

START_TEST(check_status_attribute_order)
  {
  std::vector<std::string> status;
  int             rc;
  char           *path = (char *)"../test_scripts/check_order_inventory.sh";
  char           *protocol = (char *)"1.7";
  int             index = 0;
  int             ccu_index = -1;
  int             cproc_index = -1;
  
  rc = generate_alps_status(status, path, protocol);

  fail_unless(rc == 0, "Couldn't generate the status");

  // make sure CCU comes before CPROC in status

  for(std::vector<std::string>::const_iterator i = status.begin(); i != status.end(); ++i)
    {
    if (i->substr(0, 4) == "CCU=")
      ccu_index = index;
    else if (i->substr(0, 6) == "CPROC=")
      cproc_index = index;

    index++;
    }

  fail_unless(ccu_index != -1, "CCU not found in status");
  fail_unless(cproc_index != -1, "CPROC not found in status");
  fail_unless(ccu_index < cproc_index, "CCU was not ordered before CPROC in status");
  }
END_TEST

Suite *node_func_suite(void)
  {
  Suite *s = suite_create("alps helper suite methods");
  TCase *tc_core;
 
  
  tc_core = tcase_create("parse_alps_output_test");
  tcase_add_test(tc_core, parse_alps_output_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("parse_alps13_output_test");
  tcase_add_test(tc_core, parse_alps13_output_test);
  tcase_add_test(tc_core, get_knl_information_test);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("full_generate_test");
  tcase_add_test(tc_core, full_generate_test);
  tcase_add_test(tc_core, label_generate_test);
  tcase_add_test(tc_core, check_status_attribute_order);
  suite_add_tcase(s, tc_core); 

  //tc_core = tcase_create("full_generate_test_13");
  //tcase_add_test(tc_core, full_generate_test_13);
  //suite_add_tcase(s, tc_core);

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

