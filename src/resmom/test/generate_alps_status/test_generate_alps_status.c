#include <check.h>
#include "alps_constants.h"
#include "dynamic_string.h"

int parse_alps_output(dynamic_string *output, dynamic_string *status);


char *sample_start1 = "<BasilResponse protocol='1.0'> <ResponseData status='SUCCESS' method='QUERY'> <Inventory> <NodeArray> <Node node_id='6142' name='c3-0c0s0n0' architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> ";
char *sample_start2 = "<ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
char *sample_start3 = "</Processor> <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='5' architecture='x86_64' ";
char *sample_start4 = "clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
char *sample_start5 = "</Processor> <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='10' architecture='x86_64' ";
char *sample_start6 = "clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
char *sample_start7 = "</Processor> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='3' architecture='x86_64' ";
char *sample_start8 = "clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> ";
char *sample_start9 = "</Processor> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'> <ProcessorAllocation reservation_id='2869'/> </Processor> <Processor ordinal='8' architecture='x86_64' ";
char *sample_start10 = "clock_mhz='2100'/> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/> </ProcessorArray> <MemoryArray> ";
char *sample_start11 = "<Memory type='OS' page_size_kb='4' page_count='4096000'> <MemoryAllocation reservation_id='2869' page_count='6824960'/> </Memory> </MemoryArray> <LabelArray> <Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/> </LabelArray> </Node> <Node node_id='6143' name='c3-0c0s0n1' ";
char *sample_start12 = "architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='2' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='3' architecture='x86_64' ";
char *sample_start13 = "clock_mhz='2100'/> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='7' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='8' ";
char *sample_start14 = "architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='12' architecture='x86_64' clock_mhz='2100'/> ";
char *sample_start15 = "<Processor ordinal='13' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='14' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='15' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='16' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='17' architecture='x86_64' ";
char *sample_start16 = "clock_mhz='2100'/> <Processor ordinal='18' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='19' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='20' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='21' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='22' ";
char *sample_start17 = "architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='23' architecture='x86_64' clock_mhz='2100'/> </ProcessorArray> <MemoryArray> <Memory type='OS' page_size_kb='4' page_count='8192000'/> </MemoryArray> <LabelArray> <Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/> ";
char *sample_start18 = "</LabelArray> </Node> <Node node_id='6144' name='c3-0c0s0n2' architecture='XT' role='BATCH' state='UP'> <ProcessorArray> <Processor ordinal='0' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='1' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='2' architecture='x86_64' ";
char *sample_start19 = "clock_mhz='2100'/> <Processor ordinal='3' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='4' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='5' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='6' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='7' ";
char *sample_start20 = "architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='8' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='9' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='10' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='11' architecture='x86_64' clock_mhz='2100'/> ";
char *sample_start21 = "<Processor ordinal='12' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='13' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='14' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='15' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='16' architecture='x86_64' ";
char *sample_start22 = "clock_mhz='2100'/> <Processor ordinal='17' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='18' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='19' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='20' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='21' ";
char *sample_start23 = "architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='22' architecture='x86_64' clock_mhz='2100'/> <Processor ordinal='23' architecture='x86_64' clock_mhz='2100'/> </ProcessorArray> <MemoryArray> <Memory type='OS' page_size_kb='4' page_count='8192000'/> </MemoryArray> <LabelArray> ";
char *sample_start24 = "<Label name='MOAB:FEATURE=regmem' type='SOFT' disposition='ATTRACT'/> </LabelArray> </Node> </NodeArray> <ReservationArray> <Reservation reservation_id='1772' user_name='pgarias' account_name='DEFAULT'/> <Reservation reservation_id='2549' user_name='daryal' account_name='DEFAULT'/> <Reservation reservation_id='2869' user_name='zachary' account_name='DEFAULT'/> </ReservationArray> </Inventory> </ResponseData> </BasilResponse> ";



int search_dynamic_string_status(dynamic_string *status, char *str);
int generate_alps_status(dynamic_string *status, char *path, char *protocol);

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

  fail_unless(search_dynamic_string_status(status, "6142") == 1, "Couldn't find node 6142 in the status");
  fail_unless(search_dynamic_string_status(status, "6143") == 1, "Couldn't find node 6143 in the status");
  fail_unless(search_dynamic_string_status(status, "6144") == 1, "Couldn't find node 6144 in the status");
  fail_unless(search_dynamic_string_status(status, "CPROC") == 1, "Couldn't find CPROC in the status");
  fail_unless(search_dynamic_string_status(status, "APROC") == 1, "Couldn't find APROC in the status");
  }

END_TEST

START_TEST(full_generate_test)
  {
  dynamic_string *status = get_dynamic_string(-1, NULL);
  int             rc;
  char           *path = "../../../test/test_scripts/get_inventory.sh";
  char           *protocol = "1.0";

  rc = generate_alps_status(status, path, protocol);

  fail_unless(rc == 0, "Couldn't generate the status");

  fail_unless(search_dynamic_string_status(status, "GPU") == 1, "Couldn't find the GPUs reported in the status");
  fail_unless(search_dynamic_string_status(status, "cheeseburger") == 1, "Couldn't find the feature cheeseburger in the status");
  }
END_TEST

Suite *node_func_suite(void)
  {
  Suite *s = suite_create("alps helper suite methods");
  TCase *tc_core = tcase_create("parse_alps_output_test");
  tcase_add_test(tc_core, parse_alps_output_test);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("full_generate_test");
  tcase_add_test(tc_core, full_generate_test);
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

