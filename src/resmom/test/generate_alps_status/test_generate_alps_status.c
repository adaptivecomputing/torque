#include <stdio.h>
#include <check.h>
#include "alps_constants.h"
#include "dynamic_string.h"

int parse_alps_output(dynamic_string *output, dynamic_string *status);


char *sample_start1 = (char *)"<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> ";
char *sample_start2 = (char *)"<ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
char *sample_start3 = (char *)"</Processor> <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='5' architecture='x86_64' ";
char *sample_start4 = (char *)"clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
char *sample_start5 = (char *)"</Processor> <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='10' architecture='x86_64' ";
char *sample_start6 = (char *)"clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
char *sample_start7 = (char *)"</Processor> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='3' architecture='x86_64' ";
char *sample_start8 = (char *)"clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
char *sample_start9 = (char *)"</Processor> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='8' architecture='x86_64' ";
char *sample_start10 = (char *)"clock_mhz='2100'/> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/> </ProcessorArray> <MemoryArray> ";
char *sample_start11 = (char *)"<Memory type='OS' page_size_kb='4' page_count='4096000'> <MemoryAllocation reservation_id='2869' page_count='6824960'/> </Memory> </MemoryArray> <LabelArray> <Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/> </LabelArray> </Node> <Node node_id='6143' name='c3-0c0s0n1' ";
char *sample_start12 = (char *)"architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='3' architecture='x86_64' ";
char *sample_start13 = (char *)"clock_mhz='2100'/> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='8' ";
char *sample_start14 = (char *)"architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='12' architecture='x86_64' clock_mhz='2100'/> ";
char *sample_start15 = (char *)"<Processor ordinal='13' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='14' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='15' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='16' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='17' architecture='x86_64' ";
char *sample_start16 = (char *)"clock_mhz='2100'/> <Processor ordinal='18' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='19' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='20' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='21' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='22' ";
char *sample_start17 = (char *)"architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='23' architecture='x86_64' clock_mhz='2100'/> </ProcessorArray> <MemoryArray> <Memory type='OS' page_size_kb='4' page_count='8192000'/> </MemoryArray> <LabelArray> <Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/> ";
char *sample_start18 = (char *)"</LabelArray> </Node> <Node node_id='6144' name='c3-0c0s0n2' architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='2' architecture='x86_64' ";
char *sample_start19 = (char *)"clock_mhz='2100'/> <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='7' ";
char *sample_start20 = (char *)"architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/> ";
char *sample_start21 = (char *)"<Processor ordinal='12' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='13' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='14' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='15' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='16' architecture='x86_64' ";
char *sample_start22 = (char *)"clock_mhz='2100'/> <Processor ordinal='17' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='18' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='19' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='20' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='21' ";
char *sample_start23 = (char *)"architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='22' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='23' architecture='x86_64' clock_mhz='2100'/> </ProcessorArray> <MemoryArray> <Memory type='OS' page_size_kb='4' page_count='8192000'/> </MemoryArray> <LabelArray> ";
char *sample_start24 = (char *)"<Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/> </LabelArray> </Node> </NodeArray> <ReservationArray> <Reservation reservation_id='1772' user_name='pgarias' account_name='DEFAULT'/> <Reservation reservation_id='2549' user_name='daryal' account_name='DEFAULT'/> <Reservation reservation_id='2869' user_name='zachary' account_name='DEFAULT'/> </ReservationArray> </Inventory> </ResponseData> </BasilResponse> ";

char *sample_norole_withres = (char *)"<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
char *sample_norole_nores = (char *)"<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
char *sample_batrole_withres = (char *)"<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
char *sample_batrole_nores = (char *)"<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
char *sample_introle_withres = (char *)"<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='INTERACTIVE' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
char *sample_introle_nores = (char *)"<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='INTERACTIVE' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
char *sample_unkrole_withres = (char *)"<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='UNKNOWN' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";
char *sample_unkrole_nores = (char *)"<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='UNKNOWN' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> </Processor> </ProcessorArray> </Node> </NodeArray> </Inventory> </ResponseData> </BasilResponse> ";

int search_dynamic_string_status(dynamic_string *status, char *str, unsigned int *offset);
int generate_alps_status(dynamic_string *status, const char *path, const char *protocol);


START_TEST(parse_alps_output_test)
  {
  dynamic_string *output = get_dynamic_string(-1, NULL);
  dynamic_string *status = get_dynamic_string(-1, NULL);
  int             rc;

  append_dynamic_string(output, sample_start1);
  append_dynamic_string(output, sample_start2);
  append_dynamic_string(output, sample_start3);
  append_dynamic_string(output, sample_start4);
  append_dynamic_string(output, sample_start5);
  append_dynamic_string(output, sample_start6);
  append_dynamic_string(output, sample_start7);
  append_dynamic_string(output, sample_start8);
  append_dynamic_string(output, sample_start9);
  append_dynamic_string(output, sample_start10);
  append_dynamic_string(output, sample_start11);
  append_dynamic_string(output, sample_start12);
  append_dynamic_string(output, sample_start13);
  append_dynamic_string(output, sample_start14);
  append_dynamic_string(output, sample_start15);
  append_dynamic_string(output, sample_start16);
  append_dynamic_string(output, sample_start17);
  append_dynamic_string(output, sample_start18);
  append_dynamic_string(output, sample_start19);
  append_dynamic_string(output, sample_start20);
  append_dynamic_string(output, sample_start21);
  append_dynamic_string(output, sample_start22);
  append_dynamic_string(output, sample_start23);
  append_dynamic_string(output, sample_start24);
  
  rc = parse_alps_output(output, status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"6142", NULL) == 1, "Couldn't find node 6142 in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"6143", NULL) == 1, "Couldn't find node 6143 in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"6144", NULL) == 1, "Couldn't find node 6144 in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"CPROC", NULL) == 1, "Couldn't find CPROC in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"APROC", NULL) == 1, "Couldn't find APROC in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=UP", NULL) == 1, "Couldn't find state=UP in the status");

  // role is not specified, reservation id set

  clear_dynamic_string(output);
  clear_dynamic_string(status);

  append_dynamic_string(output, sample_norole_withres);

  rc = parse_alps_output(output, status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=", NULL) == 1, "Couldn't find reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=BUSY", NULL) == 1, "Couldn't find state in the status");

  // role is not specified, reservation id not set

  clear_dynamic_string(output);
  clear_dynamic_string(status);

  append_dynamic_string(output, sample_norole_nores);

  rc = parse_alps_output(output, status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=", NULL) == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=UP", NULL) == 1, "Couldn't find state in the status");

  // role is BATCH, reservation id set

  clear_dynamic_string(output);
  clear_dynamic_string(status);

  append_dynamic_string(output, sample_batrole_withres);

  rc = parse_alps_output(output, status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=", NULL) == 1, "Couldn't find reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=BUSY", NULL) == 1, "Couldn't find state in the status");

  // role is BATCH, reservation id not set

  clear_dynamic_string(output);
  clear_dynamic_string(status);

  append_dynamic_string(output, sample_batrole_nores);

  rc = parse_alps_output(output, status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=", NULL) == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=UP", NULL) == 1, "Couldn't find state in the status");
 
  // role is interactive, reservation id set

  clear_dynamic_string(output);
  clear_dynamic_string(status);

  append_dynamic_string(output, sample_introle_withres);

  rc = parse_alps_output(output, status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=", NULL) == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=BUSY", NULL) == 1, "Couldn't find state in the status");

  // role is interactive, reservation id not set

  clear_dynamic_string(output);
  clear_dynamic_string(status);

  append_dynamic_string(output, sample_introle_nores);

  rc = parse_alps_output(output, status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=", NULL) == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=DOWN", NULL) == 1, "Couldn't find state in the status");

  // role is unknown, reservation id set

  clear_dynamic_string(output);
  clear_dynamic_string(status);

  append_dynamic_string(output, sample_unkrole_withres);

  rc = parse_alps_output(output, status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=", NULL) == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=BUSY", NULL) == 1, "Couldn't find state in the status");

  // role is unknown, reservation id not set

  clear_dynamic_string(output);
  clear_dynamic_string(status);

  append_dynamic_string(output, sample_unkrole_nores);

  rc = parse_alps_output(output, status);
  fail_unless(rc == 0, "Couldn't parse alps output");

  fail_unless(search_dynamic_string_status(status, (char *)"reservation_id=", NULL) == 0, "Found reservation in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"state=DOWN", NULL) == 1, "Couldn't find state in the status");
 } 
END_TEST 

START_TEST(parse_alps13_output_test)
  {
  /* test 1.3 protocol strings */

  #define ALPS_13_INPUT_FILE "basil_13_short.xml"

  dynamic_string *output = get_dynamic_string(-1, NULL);
  dynamic_string *status = get_dynamic_string(-1, NULL);
  int             rc;
  FILE           *fp;
  char            linebuf[1024];

  if ((fp = fopen(ALPS_13_INPUT_FILE, "r")) == NULL)
    ck_abort_msg("Couldn't open ALPS 13 input file %s", ALPS_13_INPUT_FILE);

  while (fgets(linebuf, sizeof(linebuf), fp) != NULL)
    append_dynamic_string(output, linebuf);

  fclose(fp);

  rc = parse_alps_output(output, status);
  fail_unless(rc == 0, "Couldn't parse ALPS 1.3 output contained in file %s", ALPS_13_INPUT_FILE);

  fail_unless(search_dynamic_string_status(status, (char *)"6142", NULL) == 1, "Couldn't find node 6142 in the 1.3 status");
  fail_unless(search_dynamic_string_status(status, (char *)"CPROC", NULL) == 1, "Couldn't find CPROC in the 1.3 status");
  fail_unless(search_dynamic_string_status(status, (char *)"APROC", NULL) == 1, "Couldn't find APROC in the 1.3 status");
  fail_unless(search_dynamic_string_status(status, (char *)"CCU", NULL) == 1, "Couldn't find CCU in the 1.3 status");

  }
END_TEST

START_TEST(full_generate_test)
  {
  dynamic_string *status = get_dynamic_string(-1, NULL);
  int             rc;
  char           *path = (char *)"../../../test/test_scripts/get_inventory.sh";
  char           *protocol = (char *)"1.0";

  rc = generate_alps_status(status, path, protocol);

  fail_unless(rc == 0, "Couldn't generate the status");

  fail_unless(search_dynamic_string_status(status, (char *)"GPU", NULL) > 0, "Couldn't find the GPUs reported in the status");
  fail_unless(search_dynamic_string_status(status, (char *)"cheeseburger", NULL) > 0, "Couldn't find the feature cheeseburger in the status");
  }
END_TEST 

START_TEST(full_generate_test_13)
  {
  dynamic_string *status = get_dynamic_string(-1, NULL);
  int             rc;
  char           *path = (char *)"../../../test/test_scripts/get_inventory_13.sh";
  char           *protocol = (char *)"1.3";
  unsigned int    offset;

  rc = generate_alps_status(status, path, protocol);

  fail_unless(rc == 0, "Couldn't generate the 1.3 status");

  rc = search_dynamic_string_status(status, (char *)"CPROC", &offset);
  fail_unless(rc > 0 && !strcmp(status->str+offset, "CPROC=24"), "expected CPROC=24 but not found in 1.3 status");

  search_dynamic_string_status(status, (char *)"CCU", &offset);
  fail_unless(rc > 0 && !strcmp(status->str+offset, "CCU=12"), "expected CCU=12 but not found in 1.3 status");

  fail_unless(search_dynamic_string_status(status, (char *)"GPU", NULL) > 0, "Couldn't find the GPUs reported in the 1.3 status");
  fail_unless(search_dynamic_string_status(status, (char *)"cheeseburger", NULL) > 0, "Couldn't find the feature cheeseburger in the 1.3 status");
  }
END_TEST

START_TEST(label_generate_test)
  {
  dynamic_string *status = get_dynamic_string(-1, NULL);
  int             rc;
  char           *path = (char *)"../../../test/test_scripts/label_inventory.sh";
  char           *protocol = (char *)"1.0";
  
  rc = generate_alps_status(status, path, protocol);

  fail_unless(rc == 0, "Couldn't generate the status");
  fail_unless(search_dynamic_string_status(status, (char *)"regmem", NULL) > 0);
  fail_unless(search_dynamic_string_status(status, (char *)"regmem,", NULL) == 0);
  fail_unless(search_dynamic_string_status(status, (char *)"regmemregmem", NULL) == 0);
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
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("full_generate_test");
  tcase_add_test(tc_core, full_generate_test);
  tcase_add_test(tc_core, label_generate_test);
  suite_add_tcase(s, tc_core); 
  
  tc_core = tcase_create("full_generate_test_13");
  tcase_add_test(tc_core, full_generate_test_13);
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

