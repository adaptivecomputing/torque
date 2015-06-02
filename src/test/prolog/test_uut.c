#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include <string>

#include "pbs_config.h"
#include "pbs_error.h"
#include "pbs_job.h"
#include "mom_func.h"
#include "test_prolog.h"

int check_pelog_permissions(struct stat &sbuf, int reduce_checks, job *pjob, const char *pelog, int which);
void setup_pelog_arguments(char *pelog, job *pjob, int which, char **arg);
void setup_pelog_environment(job *pjob, int which);


extern std::map<std::string, std::string> env_map;


START_TEST(test_check_pelog_permissions)
  {
  struct stat sbuf;
  job         pjob;

  memset(&sbuf, 0, sizeof(sbuf));
  memset(&pjob, 0, sizeof(pjob));

  fail_unless(check_pelog_permissions(sbuf, TRUE, &pjob, "bob", PE_PROLOGUSERJOB) != PBSE_NONE);
  sbuf.st_mode = S_IXUSR | S_IFREG;
  fail_unless(check_pelog_permissions(sbuf, TRUE, &pjob, "bob", PE_PROLOGUSERJOB) == PBSE_NONE);
  fail_unless(check_pelog_permissions(sbuf, FALSE, &pjob, "bob", PE_PROLOGUSERJOB) != PBSE_NONE);
  sbuf.st_mode |= S_IRUSR;
  fail_unless(check_pelog_permissions(sbuf, FALSE, &pjob, "bob", PE_PROLOGUSERJOB) == PBSE_NONE);
  fail_unless(check_pelog_permissions(sbuf, FALSE, &pjob, "bob", PE_PROLOG) == PBSE_NONE);
  fail_unless(check_pelog_permissions(sbuf, FALSE, &pjob, "bob", PE_PROLOGUSER) != PBSE_NONE);
  sbuf.st_mode |= S_IROTH | S_IXOTH;
  fail_unless(check_pelog_permissions(sbuf, FALSE, &pjob, "bob", PE_PROLOGUSER) == PBSE_NONE);
  sbuf.st_mode |= S_IWOTH;
  fail_unless(check_pelog_permissions(sbuf, FALSE, &pjob, "bob", PE_PROLOG) != PBSE_NONE);
  }
END_TEST

START_TEST(test_setup_pelog_arguments)
  {
  char *argv[12];
  char *pelog = strdup("bob");
  job   pjob; 

  memset(&pjob, 0, sizeof(pjob));

  strcpy(pjob.ji_qs.ji_jobid, "1.napali");
  pjob.ji_wattr[JOB_ATR_euser].at_val.at_str = strdup("dbeer");
  pjob.ji_wattr[JOB_ATR_egroup].at_val.at_str = strdup("dbeer");
  pjob.ji_wattr[JOB_ATR_jobname].at_val.at_str = strdup("pare_data");
  pjob.ji_wattr[JOB_ATR_in_queue].at_val.at_str = strdup("batch");
  pjob.ji_wattr[JOB_ATR_account].at_val.at_str = strdup("dbeer");

  setup_pelog_arguments(pelog, &pjob, PE_PROLOG, argv);

  fail_unless(!strcmp(argv[0], pelog));
  fail_unless(!strcmp(argv[1], pjob.ji_qs.ji_jobid));
  fail_unless(!strcmp(argv[2], pjob.ji_wattr[JOB_ATR_euser].at_val.at_str));
  fail_unless(!strcmp(argv[3], pjob.ji_wattr[JOB_ATR_egroup].at_val.at_str));
  fail_unless(!strcmp(argv[4], pjob.ji_wattr[JOB_ATR_jobname].at_val.at_str));
  // nothing to check for 5 fail_unless(!strcmp(argv[5], pjob.ji_qs.ji_jobid);
  fail_unless(!strcmp(argv[6], pjob.ji_wattr[JOB_ATR_in_queue].at_val.at_str));
  fail_unless(!strcmp(argv[7], pjob.ji_wattr[JOB_ATR_account].at_val.at_str));
 
  // check for epilogues
  pjob.ji_wattr[JOB_ATR_session_id].at_val.at_long = 204;
  pjob.ji_qs.ji_un.ji_momt.ji_exitstat = 0;

  setup_pelog_arguments(pelog, &pjob, PE_EPILOG, argv);
  fail_unless(!strcmp(argv[5], "204"));
  // nothing to check for 6 or 7
  fail_unless(!strcmp(argv[8], pjob.ji_wattr[JOB_ATR_in_queue].at_val.at_str));
  fail_unless(!strcmp(argv[9], pjob.ji_wattr[JOB_ATR_account].at_val.at_str));
  fail_unless(!strcmp(argv[10], "0"));
  }
END_TEST


START_TEST(test_setup_pelog_environment)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));

  // initialize job so env variables can be added
  pjob.ji_numvnod = 20;
  pjob.ji_nodeid = 1;
  pjob.ji_vnods = (vnodent *)calloc(2, sizeof(vnodent));
  pjob.ji_vnods[0].vn_host = (hnodent *)calloc(1, sizeof(hnodent));
  pjob.ji_vnods[0].vn_host->hn_host = strdup("napali");
  pjob.ji_flags |= MOM_HAS_NODEFILE;
  strcpy(pjob.ji_qs.ji_jobid, "10.napali");
  pjob.ji_wattr[JOB_ATR_variables].at_val.at_arst = (struct array_strings *)calloc(1, 
      sizeof(struct array_strings));
  pjob.ji_wattr[JOB_ATR_variables].at_val.at_arst->as_usedptr = 0;

  setup_pelog_environment(&pjob, PE_PROLOG);

  // check for values
  
  // I can't check the resource values because spoofing the resources isn't working
  //std::map<std::string, std::string>::iterator it = env_map.find("PBS_RESOURCE_NODES");
  //fail_unless(!strcmp(it->second.c_str(), "5"));

  //it = env_map.find("PBS_NUM_NODES");
  //fail_unless(!strcmp(it->second.c_str(), "5"));

  //it = env_map.find("PBS_NUM_PPN");
  //fail_unless(!strcmp(it->second.c_str(), "1"));

//  it = env_map.find("PBS_NP");
//  fail_unless(!strcmp(it->second.c_str(), "20"));

//  it = env_map.find("PBS_RESOURCE_GRES");
//  fail_unless(!strcmp(it->second.c_str(), "5"));

  std::map<std::string, std::string>::iterator it = env_map.find("TMPDIR");
  fail_unless(!strcmp(it->second.c_str(), "/home/dbeer/scratch/"));

  it = env_map.find("PBS_NODENUM");
  fail_unless(!strcmp(it->second.c_str(), "1"));

  it = env_map.find("PBS_MSHOST");
  fail_unless(!strcmp(it->second.c_str(), "napali"));

  it = env_map.find("PBS_NODEFILE");
  fail_unless(!strcmp(it->second.c_str(), "/tmp/10.napali"));
  
  }
END_TEST


Suite *prolog_suite(void)
  {
  Suite *s = suite_create("prolog_suite methods");
  TCase *tc_core = tcase_create("test_check_pelog_permissions");
  tcase_add_test(tc_core, test_check_pelog_permissions);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_setup_pelog_arguments");
  tcase_add_test(tc_core, test_setup_pelog_arguments);
  tcase_add_test(tc_core, test_setup_pelog_environment);
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
  sr = srunner_create(prolog_suite());
  srunner_set_log(sr, "prolog_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
