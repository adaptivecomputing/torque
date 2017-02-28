#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include <map>
#include <set>

#include "pbs_config.h"
#include "mom_mach.h"
#include "test_mom_mach.h"
#include "pbs_job.h"
#include "pbs_error.h"
#include "mom_memory.h"
#include "resmon.h"

std::string cg_memory_path;

int get_job_sid_from_pid(int);
int injob(job*, int);
unsigned long cput_sum(job*);
int overmem_proc(job*, unsigned long long);
int overcpu_proc(job*, unsigned long);
unsigned long long resi_sum(job*);
unsigned long long mem_sum(job*);
proc_mem_t *get_proc_mem();
const char *physmem  (struct rm_attribute *);
const char *totmem  (struct rm_attribute *);


double cputfactor;

int pagesize;

job_pid_set_t global_job_sid_set;
pid2jobsid_map_t pid2jobsid_map;

extern pid2procarrayindex_map_t pid2procarrayindex_map;
extern proc_stat_t   *proc_array;

extern void *get_next_return_value;

extern unsigned long max_memory;
extern unsigned long max_swap;


START_TEST(memory_tests)
  {
  proc_mem_t   *pm = get_proc_mem();
  const char   *mem_str;
  char          buf[1024];
  rm_attribute  ra;

  if (pm != NULL)
    {
    // Unset these parameters
    max_memory = 0;
    max_swap = 0;

    mem_str = physmem(&ra);
    fail_unless(mem_str != NULL);
    sprintf(buf, "%llukb", pm->mem_total >> 10);
    fail_unless(!strcmp(buf, mem_str));

    mem_str = totmem(&ra);
    fail_unless(mem_str != NULL);
    sprintf(buf, "%llukb", (pm->mem_total >> 10) + (pm->swap_total >> 10));
    fail_unless(!strcmp(buf, mem_str));

    // Make max_memory and max_swap half of the machine's memory and swap
    max_memory = pm->mem_total >> 11;
    max_swap = pm->swap_total >> 11;
    mem_str = physmem(&ra);
    fail_unless(mem_str != NULL);
    sprintf(buf, "%lukb", max_memory);
    fail_unless(!strcmp(buf, mem_str));

    mem_str = totmem(&ra);
    fail_unless(mem_str != NULL);
    sprintf(buf, "%lukb", max_memory + max_swap);
    fail_unless(!strcmp(buf, mem_str));
    }
  }
END_TEST


START_TEST(test_get_job_sid_from_pid)
  { 
  int index = 0;

  /* all the maps/sets should be empty */
  pid2jobsid_map.clear();
  pid2procarrayindex_map.clear();
  global_job_sid_set.clear();

  /* expect fail for pids < 2 */
  fail_unless(get_job_sid_from_pid(0) == -1);
  fail_unless(get_job_sid_from_pid(1) == -1);

  /* expect fail for unknown pid */
  fail_unless(get_job_sid_from_pid(5) == -1);

  /* insert sid into sid set */
  global_job_sid_set.insert(1000);

  /* create space for 2 pids */
  proc_array = (proc_stat_t *) calloc(2, sizeof(proc_stat_t));
  fail_unless(proc_array != NULL);

  proc_array[index].pid = 10;
  proc_array[index].ppid = 20;
  proc_array[index].session = 1000;

  /* map pid to index */
  pid2procarrayindex_map[proc_array[index].pid] = index;

  /* pid's sid is in global_job_sid_set so expect success */
  fail_unless(get_job_sid_from_pid(10) == 1000);

  /* change the pid's session to something not in global_job_sid_set */
  proc_array[index].session = 2000;

  /* set up additional pid (parent of pid 10) */
  index++;
  proc_array[index].pid = 20;
  proc_array[index].ppid = 1;
  proc_array[index].session = 1000;

  /* map pid to index */
  pid2procarrayindex_map[proc_array[index].pid] = index;

  /* find the session using recursion--expect success */
  fail_unless(get_job_sid_from_pid(10) == 1000);

  /* make pid 20's session id be something not in the global_job_sid_set */
  proc_array[index].session = 2000;

  /* pid 20's session is not in global_job_sid_set and has parent pid 1 so expect failure */ 
  fail_unless(get_job_sid_from_pid(20) == -1);
  }
END_TEST

START_TEST(test_injob)
  {
  job *pjob;

  /* all the maps/sets should be empty */
  pid2jobsid_map.clear();
  pid2procarrayindex_map.clear();
  global_job_sid_set.clear();

  /* create space for job structure */
  pjob = (job *) calloc(1, sizeof(job));
  fail_unless(pjob != NULL);
  pjob->ji_tasks = new std::vector<task *>();

  /* create job pid set */
  pjob->ji_job_pid_set = new job_pid_set_t;
  fail_unless(pjob->ji_job_pid_set != NULL);

  /* expect failure since 10 is not in the pid2jobsid_map */
  fail_unless(injob(pjob, 10) == FALSE);

  /* set job to have 1000 as a session id */
  pjob->ji_job_pid_set->insert(1000);

  /* create a mapping from pid 10 to session id 1000 */
  pid2jobsid_map[10] = 1000;

  /* expect success */
  fail_unless(injob(pjob, 10) == TRUE);

  /* set job to have 5000 as a session id */
  pjob->ji_job_pid_set->clear();
  pjob->ji_job_pid_set->insert(5000);

  /* expect failure since job session id of pid not in job's pid set */
  fail_unless(injob(pjob, 10) == FALSE);

  /* check a job's task */

  /* set pid's job sid to 2000 after removing old mapping */
  pid2jobsid_map.erase(10);
  pid2jobsid_map[10] = 2000;
 
  /* create task to hold the payload) */
  task *tp = new task();
  fail_unless(tp != NULL);

  /* populate it */
  tp->ti_qs.ti_sid = 2000;
  pjob->ji_tasks->push_back(tp);

  /* expect success since job session id of pid matches the first task sid */
  fail_unless(injob(pjob, 10) == TRUE);
  }
END_TEST

#ifndef PENABLE_LINUX_CGROUPS
START_TEST(test_cput_sum)
  {
  job *pjob;

  /* all the maps/sets should be empty */
  pid2jobsid_map.clear();
  pid2procarrayindex_map.clear();
  global_job_sid_set.clear();

  /* create space for job structure */
  pjob = (job *) calloc(1, sizeof(job));
  fail_unless(pjob != NULL);
  pjob->ji_tasks = new std::vector<task *>();

  /* create job pid set */
  pjob->ji_job_pid_set = new job_pid_set_t;
  fail_unless(pjob->ji_job_pid_set != NULL);

  /* empty pid2jobsid_map so 0 expected */
  fail_unless(cput_sum(pjob) == 0);

  /* expect mOM_NO_PROC to be set */
  fail_unless((pjob->ji_flags & MOM_NO_PROC) != 0);

  /* set up some preliminaries */
  pid2jobsid_map[10] = 1000;

  /* empty pid2procarrayindex_map so 0 expected */
  fail_unless(cput_sum(pjob) == 0);

  /* create space for 1 pid */
  proc_array = (proc_stat_t *) calloc(1, sizeof(proc_stat_t));
  fail_unless(proc_array != NULL);

  /* fill in some values */
  proc_array[0].pid = 10;
  proc_array[0].ppid = 20;
  proc_array[0].session = 1000;
  proc_array[0].utime = 10;
  proc_array[0].stime = 20;
  proc_array[0].cutime = 30;
  proc_array[0].cstime = 40;

  /* map pid to index */
  pid2procarrayindex_map[proc_array[0].pid] = 0;

  /* set job to have 1000 as a session id */
  pjob->ji_job_pid_set->insert(1000);

  cputfactor = 1.0;

  /* expect (utime + stime + cutime + cstime) * cputfactor = 100 */
  fail_unless(cput_sum(pjob) == 100);

  /* expect MOM_NO_PROC *not* to be set */
  fail_unless((pjob->ji_flags & MOM_NO_PROC) == 0);
  }
END_TEST

#endif

START_TEST(test_overmem_proc)
  {
  job *pjob;

  /* all the maps/sets should be empty */
  pid2jobsid_map.clear();
  pid2procarrayindex_map.clear();
  global_job_sid_set.clear();

  /* create space for job structure */
  pjob = (job *) calloc(1, sizeof(job));
  fail_unless(pjob != NULL);
  pjob->ji_tasks = new std::vector<task *>();

  /* create job pid set */
  pjob->ji_job_pid_set = new job_pid_set_t;
  fail_unless(pjob->ji_job_pid_set != NULL);

  /* pid2jobsid_map is empty so expect FALSE */
  fail_unless(overmem_proc(pjob, 0) == FALSE);

  /* create space for 1 pid */
  proc_array = (proc_stat_t *) calloc(1, sizeof(proc_stat_t));
  fail_unless(proc_array != NULL);

  /* fill in some values */
  proc_array[0].pid = 10;
  proc_array[0].ppid = 20;
  proc_array[0].session = 1000;
  proc_array[0].vsize = 10;

  /* map pid to index */
  pid2procarrayindex_map[proc_array[0].pid] = 0;

  /* setup values so that injob() will return false */
  pjob->ji_job_pid_set->insert(1000);
  pid2jobsid_map[10] = 2000;

  /* pid 10 is not in job so expect FALSE */
  fail_unless(overmem_proc(pjob, 0) == FALSE);

  /* add new sid for pid 10 */
  pid2jobsid_map.clear();
  pid2jobsid_map[10] = 1000;

  /* pid 10 is in job and vsize > 0 so expect TRUE */
  fail_unless(overmem_proc(pjob, 0) == TRUE);

  /* pid 10 is in job but vsize < 100 so expect FALSE */
  fail_unless(overmem_proc(pjob, 100) == FALSE);

  /* clear map */
  pid2procarrayindex_map.clear();

  /* pid 10 is in job but can't look up pid in pid2procarrayindex_map so expect FALSE */
  fail_unless(overmem_proc(pjob, 0) == FALSE);
  }
END_TEST

START_TEST(test_overcpu_proc)
  {
  job *pjob;

  /* all the maps/sets should be empty */
  pid2jobsid_map.clear();
  pid2procarrayindex_map.clear();
  global_job_sid_set.clear();

  /* create space for job structure */
  pjob = (job *) calloc(1, sizeof(job));
  fail_unless(pjob != NULL);
  pjob->ji_tasks = new std::vector<task *>();

  /* create job pid set */
  pjob->ji_job_pid_set = new job_pid_set_t;
  fail_unless(pjob->ji_job_pid_set != NULL);

  /* pid2jobsid_map is empty so expect FALSE */
  fail_unless(overcpu_proc(pjob, 0) == FALSE);

  /* create space for 1 pid */
  proc_array = (proc_stat_t *) calloc(1, sizeof(proc_stat_t));
  fail_unless(proc_array != NULL);

  /* fill in some values */
  proc_array[0].pid = 10;
  proc_array[0].ppid = 20;
  proc_array[0].session = 1000;
  proc_array[0].utime = 10;
  proc_array[0].stime = 20;

  /* map pid to index */
  pid2procarrayindex_map[proc_array[0].pid] = 0;

  /* setup values so that injob() will return false */
  pjob->ji_job_pid_set->insert(1000);
  pid2jobsid_map[10] = 2000;

  /* pid 10 is not in job so expect FALSE */
  fail_unless(overcpu_proc(pjob, 0) == FALSE);

  /* add new sid for pid 10 */
  pid2jobsid_map.clear();
  pid2jobsid_map[10] = 1000;

  cputfactor = 1.0;

  /* pid 10 is in job but cputime > 0 so expect TRUE */
  fail_unless(overcpu_proc(pjob, 0) == TRUE);

  /* pid 10 is in job but cputime < 100 so expect FALSE */
  fail_unless(overcpu_proc(pjob, 100) == FALSE);

  /* clear map */
  pid2procarrayindex_map.clear();

  /* pid 10 is in job but can't look up pid in pid2procarrayindex_map so expect FALSE */
  fail_unless(overcpu_proc(pjob, 0) == FALSE);
  }
END_TEST

#ifndef PENABLE_LINUX_CGROUPS
START_TEST(test_resi_sum)
  {
  job *pjob;

  /* all the maps/sets should be empty */
  pid2jobsid_map.clear();
  pid2procarrayindex_map.clear();
  global_job_sid_set.clear();

  /* create space for job structure */
  pjob = (job *) calloc(1, sizeof(job));
  fail_unless(pjob != NULL);
  pjob->ji_tasks = new std::vector<task *>();

  /* create job pid set */
  pjob->ji_job_pid_set = new job_pid_set_t;
  fail_unless(pjob->ji_job_pid_set != NULL);

  /* pid2jobsid_map is empty so expect 0 */
  fail_unless(resi_sum(pjob) == 0);

  /* create space for 1 pid */
  proc_array = (proc_stat_t *) calloc(1, sizeof(proc_stat_t));
  fail_unless(proc_array != NULL);

  /* fill in some values */
  proc_array[0].pid = 10;
  proc_array[0].ppid = 20;
  proc_array[0].session = 1000;
  proc_array[0].rss = 1;

  /* map pid to index */
  pid2procarrayindex_map[proc_array[0].pid] = 0;

  /* setup values so that injob() will return false */
  pjob->ji_job_pid_set->insert(1000);
  pid2jobsid_map[10] = 2000;

  /* pid 10 is not in job so expect 0 */
  fail_unless(resi_sum(pjob) == 0);

  /* add new sid for pid 10 */
  pid2jobsid_map.clear();
  pid2jobsid_map[10] = 1000;

  pagesize = 4096;

  /* pid 10 is in job so expect 1*pagesize */
  fail_unless(resi_sum(pjob) == (unsigned long long)pagesize);

  /* clear map */
  pid2procarrayindex_map.clear();

  /* pid is in job but can't look up pid in pid2procarrayindex_map so expect 0 */
  fail_unless(resi_sum(pjob) == 0);

  /* todo: test when USELIBMEMACCT set */
  }
END_TEST
#endif

START_TEST(test_mem_sum)
  {
  job *pjob;

  /* all the maps/sets should be empty */
  pid2jobsid_map.clear();
  pid2procarrayindex_map.clear();
  global_job_sid_set.clear();

  /* create space for job structure */
  pjob = (job *) calloc(1, sizeof(job));
  fail_unless(pjob != NULL);
  pjob->ji_tasks = new std::vector<task *>();

  /* create job pid set */
  pjob->ji_job_pid_set = new job_pid_set_t;
  fail_unless(pjob->ji_job_pid_set != NULL);

  /* pid2jobsid_map is empty so expect 0 */
  fail_unless(mem_sum(pjob) == 0);

  /* create space for 1 pid */
  proc_array = (proc_stat_t *) calloc(1, sizeof(proc_stat_t));
  fail_unless(proc_array != NULL);

  /* fill in some values */
  proc_array[0].pid = 10;
  proc_array[0].ppid = 20;
  proc_array[0].session = 1000;
  proc_array[0].vsize = 10;

  /* map pid to index */
  pid2procarrayindex_map[proc_array[0].pid] = 0;

  /* setup values so that injob() will return false */
  pjob->ji_job_pid_set->insert(1000);
  pid2jobsid_map[10] = 2000;

  /* pid 10 is not in job so expect 0 */
  fail_unless(mem_sum(pjob) == 0);

  /* add new sid for pid 10 */
  pid2jobsid_map.clear();
  pid2jobsid_map[10] = 1000;

  /* pid 10 is in job so expect 10 */
  fail_unless(mem_sum(pjob) == 10);

  /* clear map */
  pid2procarrayindex_map.clear();

  /* pid is in job but can't look up pid in pid2procarrayindex_map so expect 0 */
  fail_unless(mem_sum(pjob) == 0);
  }
END_TEST

Suite *mom_mach_suite(void)
  {
  Suite *s = suite_create("mom_mach_suite methods");
  TCase *tc_core = tcase_create("test_get_job_sid_from_pid");
  tcase_add_test(tc_core, test_get_job_sid_from_pid);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_injob");
  tcase_add_test(tc_core, test_injob);
  suite_add_tcase(s, tc_core);

#ifndef PENABLE_LINUX_CGROUPS
  /* for NUMA cput_sum uses cgroups. We need to 
     change the unit test for this function
     to create a mock cgroup file */
  tc_core = tcase_create("test_cput_sum");
  tcase_add_test(tc_core, test_cput_sum);
  /*suite_add_tcase(s, tc_core);*/
#endif

  tc_core = tcase_create("test_overmem_proc");
  tcase_add_test(tc_core, test_overmem_proc);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_overcpu_proc");
  tcase_add_test(tc_core, test_overcpu_proc);
  suite_add_tcase(s, tc_core);

#ifndef PENABLE_LINUX_CGROUPS
  tc_core = tcase_create("test_resi_sum");
  tcase_add_test(tc_core, test_resi_sum);
  suite_add_tcase(s, tc_core);
#endif 

  tc_core = tcase_create("test_mem_sum");
  tcase_add_test(tc_core, test_mem_sum);
  tcase_add_test(tc_core, memory_tests);
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
  sr = srunner_create(mom_mach_suite());
  srunner_set_log(sr, "mom_mach_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
