#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include "start_exec.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <set>
#include <map>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <grp.h>

#include "pbs_error.h"
#include "pbs_nodes.h"
#include "test_uut.h"

int job_nodes(job &pjob);
int get_indices_from_exec_str(const char *exec_str, char *buf, int buf_size);
int  remove_leading_hostname(char **jobpath);
int get_num_nodes_ppn(const char*, int*, int*);
int become_the_user(job*, bool);
int TMomCheckJobChild(pjobexec_t*, int, int*, int*);
int update_path_attribute(job*, job_atr);

#if NO_SPOOL_OUTPUT == 1
int save_supplementary_group_list(int*, gid_t**);
int restore_supplementary_group_list(int, gid_t*);
#endif

#ifdef NUMA_SUPPORT
extern nodeboard node_boards[];
#endif

extern char mom_alias[];

#define MAX_TEST_ENVP 150
#define MAX_TEST_BLOCK 8096

char *penv[MAX_TEST_ENVP]; /* max number of pointers bld_env_variables will create */
char *envBuffer = NULL; /* points to the max block that bld_env_variables would ever need in this test suite */
int spoolasfinalname = FALSE;
extern int  logged_event;
extern int  num_contacted;
extern int  send_sisters_called;
extern int  send_ms_called;
extern bool bad_pwd;
extern bool fail_init_groups;
extern bool fail_site_grp_check;
extern bool am_ms;
extern bool addr_fail;

extern int global_poll_fd;
extern int global_poll_timeout_sec;

void create_command(std::string &cmd, char **argv);
void no_hang(int sig);
void exec_bail(job *pjob, int code, std::set<int> *sisters_contacted);
void escape_spaces(const char *str, std::string &escaped);

#ifdef PENABLE_LINUX_CGROUPS
unsigned long long get_memory_limit_from_resource_list(job *pjob);
#endif

int jobstarter_privileged = 0;
 
 
START_TEST(test_escape_spaces)
  {
  std::string escaped;

  const char *ws1 = "one two";
  const char *ws2 = "one two three";
  const char *ws3 = "one two three four";

  escape_spaces(ws1, escaped);
  // should have added a slash
  fail_unless(escaped.size() == strlen(ws1) + 1);
  fail_unless(escaped == "one\\ two");

  escaped.clear();
  escape_spaces(ws2, escaped);
  // should have added two slashes
  fail_unless(escaped.size() == strlen(ws2) + 2);
  fail_unless(escaped == "one\\ two\\ three");

  escaped.clear();
  escape_spaces(ws3, escaped);
  // should have added three slashes
  fail_unless(escaped.size() == strlen(ws3) + 3);
  fail_unless(escaped == "one\\ two\\ three\\ four");
  }
END_TEST

#ifdef PENABLE_LINUX_CGROUPS
START_TEST(get_memory_limit_from_resource_list_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  pjob.ji_vnods = (vnodent *)calloc(4, sizeof(vnodent));
  pjob.ji_numvnod = 4;

  for (int i = 0; i < pjob.ji_numvnod; i++)
    pjob.ji_vnods[i].vn_host = (hnodent *)calloc(1, sizeof(hnodent));

  // Do the full case
  unsigned long long mem_limit = get_memory_limit_from_resource_list(&pjob);
  fail_unless(mem_limit == 4 * 1024 * 1024);
  
  // Test where only half the vnods are on this node
  for (int i = pjob.ji_numvnod / 2; i < pjob.ji_numvnod; i++)
    pjob.ji_vnods[i].vn_host->hn_node = 1;

  mem_limit = get_memory_limit_from_resource_list(&pjob);
  fail_unless(mem_limit == 2 * 1024 * 1024);
  }
END_TEST
#endif


START_TEST(remove_leading_hostname_test)
  {
  char *p1 = strdup("napali:/home/dbeer");
  char *p2 = NULL;

  fail_unless(remove_leading_hostname(NULL) == FAILURE);
  fail_unless(remove_leading_hostname(&p2) == FAILURE);
  fail_unless(remove_leading_hostname(&p1) == SUCCESS);
  fail_unless(!strcmp(p1, "/home/dbeer"), p1);
  // for some reason it returns failure if a ':' isn't present
  fail_unless(remove_leading_hostname(&p1) == FAILURE);
  // make sure it didn't change
  fail_unless(!strcmp(p1, "/home/dbeer"), p1);
  }
END_TEST


START_TEST(exec_bail_test)
  {
  job pjob;
  memset(&pjob, 0, sizeof(pjob));
  
  am_ms = false;
  send_ms_called = 0;
  send_sisters_called = 0;
  exec_bail(&pjob, 1, NULL);
  // make sure we called send_ms() and not send_sisters()
  fail_unless(send_ms_called > 0);
  fail_unless(send_sisters_called == 0);

  am_ms = true;
  pjob.ji_stdout = 11;
  pjob.ji_stderr = 12;
  exec_bail(&pjob, 2, NULL);
  fail_unless(pjob.ji_qs.ji_substate == JOB_SUBSTATE_EXITING);
  fail_unless(pjob.ji_qs.ji_un.ji_momt.ji_exitstat == 2); // must match the code passed in
  fail_unless(send_sisters_called > 0);

  }
END_TEST


START_TEST(no_hang_test)
  {
  logged_event = 0;
  no_hang(SIGUSR1);

  fail_unless(logged_event > 0);
  }
END_TEST


START_TEST(create_command_test)
  {
  char  *argv[] = {(char *)"ls", (char *)"-ltr", (char *)"/home/dbeer", (char *)"|", (char *)"grep", (char *)"bob", NULL};
  std::string  cmd;

  create_command(cmd, argv);
  fail_unless(cmd == "ls -ltr /home/dbeer | grep bob", cmd.c_str());
  }
END_TEST


START_TEST(job_nodes_test)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  pjob->ji_usages = new std::map<std::string, job_host_data>();

  pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("napali/0-9+waimea/0-9");
  pjob->ji_wattr[JOB_ATR_exec_port].at_val.at_str = strdup("15002+15002");

  fail_unless(job_nodes(*pjob) != PBSE_NONE);
  // nothing should've happened since the flag isn't set
  fail_unless(pjob->ji_numnodes == 0);
  
  pjob->ji_wattr[JOB_ATR_exec_host].at_flags = ATR_VFLAG_SET;

  // Force it to not resolve the hostname
  addr_fail = true;
  int rc = job_nodes(*pjob);
  fail_unless(rc == PBSE_CANNOT_RESOLVE, "Error is %d", rc);
  addr_fail = false; // allow things to work normally


  fail_unless(job_nodes(*pjob) == PBSE_NONE);
  fail_unless(pjob->ji_numnodes == 2);
  fail_unless(pjob->ji_numvnod == 20);

  fail_unless(!strcmp(pjob->ji_hosts[0].hn_host, "napali"));
  fail_unless(!strcmp(pjob->ji_hosts[1].hn_host, "waimea"));
  fail_unless(pjob->ji_hosts[2].hn_node == TM_ERROR_NODE);
  fail_unless(pjob->ji_vnods[20].vn_node == TM_ERROR_NODE);

  for (int i = 0; i < 2; i++)
    {
    fail_unless(pjob->ji_hosts[i].hn_node == i);
    fail_unless(pjob->ji_hosts[i].hn_sister == SISTER_OKAY);
    fail_unless(pjob->ji_hosts[i].hn_port == 15002);
    fail_unless(GET_NEXT(pjob->ji_hosts[i].hn_events) == NULL);
    }
  
  for (int i = 0; i < 20; i++)
    {
    fail_unless(pjob->ji_vnods[i].vn_node == i);
    fail_unless(pjob->ji_vnods[i].vn_index == i % 10);
    fail_unless(pjob->ji_vnods[i].vn_host == &pjob->ji_hosts[i/10]);
    }

  nodes_free(pjob);
  fail_unless(pjob->ji_vnods == NULL);
  fail_unless(pjob->ji_hosts == NULL);
  }
END_TEST


void setup_vtable(
  struct var_table *vtable, 
  int v_envpsz, 
  int v_blocksz)

  {
  int i;

  memset(vtable, 0, sizeof(struct var_table));
  vtable->v_ensize = v_envpsz;
  vtable->v_envp = (char **)calloc(vtable->v_ensize, sizeof(char *));
  vtable->v_bsize = v_blocksz;
  vtable->v_block_start = (char *)calloc(1, vtable->v_bsize);
  vtable->v_block = vtable->v_block_start;

  for (i = 0; i < MAX_TEST_ENVP; ++i)
    *(penv + i) = NULL;

  envBuffer = (char *)calloc(MAX_TEST_BLOCK, sizeof(char));
  }

int add_env_variables(
  struct var_table *vtable,
  int n_env_vars, 
  int n_mem_to_use)

  {
  char *buf;
  char *alphabet = strdup("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghigjklmnopqrstuvwxyz");
  char *p, *bp, *ptest_block;
  int n = n_mem_to_use/n_env_vars;
  int m = n_mem_to_use % n_env_vars;
  int siz, i, j, len;
  char varname[64];
  char *varNval;

  ptest_block = envBuffer;
  for (i=0; i < n_env_vars; i++)
    {
    if (i != (n_env_vars - 1))
      siz = n;
    else
      siz = n + m;
    buf = (char *)calloc(siz, sizeof(char));
    p = alphabet;
    bp = buf;
    for(j=0; j < (siz - 1); ++j)
      {
      if (*p == '\0')
         p = alphabet;
      *bp++ = *p++;
      }

    sprintf(varname, "var%d=", i);
    len = strlen(varname) + strlen(buf) + 1;
    varNval = (char *) calloc(len, sizeof(char));
    sprintf(varNval, "%s%s", varname, buf);
    penv[i] = ptest_block;
    strcpy(penv[i], varNval);
    ptest_block += strlen(varNval) + 1;
    if (bld_env_variables(vtable, varNval, NULL) != PBSE_NONE)
      {
      return -1;
      }
    free(buf);
    free(varNval);
    }
  return 0;
  }

void free_used_mem(struct var_table *vtable)
  {
  if (vtable->v_block_start)
      free(vtable->v_block_start);
  if (vtable->v_envp) 
      free(vtable->v_envp);
  if (envBuffer) 
      free(envBuffer);
  }

int check_content(
  struct var_table *vtable)

  {
  int i;
  for (i=0; i < vtable->v_used; ++i)
    if (strcmp((char *)*(vtable->v_envp + i), (char *)penv[i]))
      return -1;

  return PBSE_NONE;
  }

START_TEST(test_bld_env_variables_no_realloc)
  {
  struct var_table vtable;
  char *envp, *block;
  int rc;

  setup_vtable(&vtable, 20, 4096);
  envp = (char *)vtable.v_envp;
  block = (char *)vtable.v_block_start;
  rc = add_env_variables(&vtable, 10, 1024);
  fail_unless(rc == PBSE_NONE, "something went wrong in bld_env_variables unexpectely");
  fail_unless((char *)vtable.v_envp == (char *)envp, "bld_env_variables should not have reallocated env pointers");
  fail_unless(vtable.v_block_start == block, "bld_env_variables should not have reallocated the block of memory");
  fail_unless(vtable.v_ensize == 20, "size of pointers allocation were altered: expected 20, actual %d", vtable.v_ensize);
  fail_unless(vtable.v_used == 10, "env used were not correctly updated; expected 10, actual %d", vtable.v_used);
  rc = check_content(&vtable);
  fail_unless(rc == PBSE_NONE, "env variables were not written as expected");
  free_used_mem(&vtable);
  }
END_TEST


START_TEST(test_bld_env_variables_realloc_env)
  {
  struct var_table vtable;
  char *envp, *block;
  int rc;

  setup_vtable(&vtable, 6, 4096);
  envp = (char *)vtable.v_envp;
  block = (char *)vtable.v_block_start;
  rc = add_env_variables(&vtable, 10, 1024);
  fail_unless(rc == PBSE_NONE, "something went wrong in bld_env_variables unexpectely");
  fail_unless((char *)vtable.v_envp != (char *)envp, "bld_env_variables should have been reallocated for env pointers");
  fail_unless(vtable.v_block_start == block, "bld_env_variables should not have been reallocated the block of memory");
  rc = 6 + EN_THRESHOLD;
  fail_unless(vtable.v_ensize == rc, "size of env pointers were readjusted incorrectly: expected %d, actual %d", rc, vtable.v_ensize);
  fail_unless(vtable.v_used == 10, "env used were not correctly updated; expected 10, actual %d", vtable.v_used);
  free_used_mem(&vtable);
  }
END_TEST

START_TEST(test_bld_env_variables_realloc_block)
 {
  struct var_table vtable;
  char *envp, *block;
  int rc;

  setup_vtable(&vtable, 120, 1024);
  envp = (char *)vtable.v_envp;
  block = (char *)vtable.v_block_start;
  rc = add_env_variables(&vtable, 10, 2048);
  fail_unless(rc == PBSE_NONE, "something went wrong in bld_env_variables unexpectely");
  fail_unless((char *)vtable.v_envp == (char *)envp, "bld_env_variables should not have reallocated env pointers");
  fail_unless(vtable.v_block_start != block, "bld_env_variables should have been reallocated the block of memory");
  rc = 1024 + EXTRA_VARIABLE_SPACE;
  fail_unless(vtable.v_ensize == 120, "size of env pointers were readjusted incorrectly: expected 120, actual %d", vtable.v_ensize);
  fail_unless(vtable.v_used == 10, "env used were not correctly updated; expected 10, actual %d", vtable.v_used); 
  rc = check_content(&vtable);
  fail_unless(rc == PBSE_NONE, "env variables were not written as expected");
  free_used_mem(&vtable);
  }
END_TEST

START_TEST(test_bld_env_variables_realloc_all)
  {
  struct var_table vtable;
  char *envp, *block;
  int rc;

  setup_vtable(&vtable, 5, 1024);
  envp = (char *)vtable.v_envp;
  block = (char *)vtable.v_block_start;
  rc = add_env_variables(&vtable, 12, 2048);
  fail_unless(rc == PBSE_NONE, "something went wrong in bld_env_variables unexpectely");
  fail_unless((char *)vtable.v_envp != (char *)envp, "bld_env_variables should have reallocated env pointers");
  fail_unless(vtable.v_block_start != block, "bld_env_variables should have been reallocated the block of memory");
  rc = 1024 + EXTRA_VARIABLE_SPACE;
  fail_unless((vtable.v_bsize + vtable.v_block - vtable.v_block_start) == rc, "allocated block not correct");
  fail_unless(vtable.v_ensize == 105, "size of env pointers were readjusted incorrectly: expected 120, actual %d", vtable.v_ensize);
  fail_unless(vtable.v_used == 12, "env used were not correctly updated; expected 10, actual %d", vtable.v_used); 
  rc = check_content(&vtable);
  fail_unless(rc == PBSE_NONE, "env variables were not written as expected");
  free_used_mem(&vtable);
  }
END_TEST

START_TEST(test_get_indices_from_exec_str)
  {
  char  buf[1024];

  fail_unless(get_indices_from_exec_str(NULL, NULL, 0) != PBSE_NONE);

  strcpy(mom_alias, "slesmic");

  fail_unless(get_indices_from_exec_str("slesmic-0-mic/1+slesmic-0-mic/0", buf, sizeof(buf)) == PBSE_NONE);
  fail_unless(!strcmp(buf, "1,0"));
  
  strcpy(mom_alias, "napali");

  fail_unless(get_indices_from_exec_str("napali-gpu/1+napali-gpu/2+napali-gpu/3", buf, sizeof(buf)) == PBSE_NONE);
  fail_unless(!strcmp(buf, "1,2,3"), buf);
  
  fail_unless(get_indices_from_exec_str("napali-gpu/1+napali-gpu/2+napali-gpu/3+waimea-gpu/0+waimea-gpu/1+waimea-gpu/2", buf, sizeof(buf)) == PBSE_NONE);
  fail_unless(!strcmp(buf, "1,2,3"), buf);
  }
END_TEST

START_TEST(test_check_pwd_euser)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int pwd = -1;

  pwd = check_pwd(pjob);
  fail_unless(pwd != PBSE_NONE, "check_pwd succeeded with an empty job");

  bad_pwd = true;
  decode_str(&pjob->ji_wattr[JOB_ATR_euser], "euser", NULL, "rightsaidfred", 0);
  pwd = check_pwd(pjob);
  fail_unless(pwd != PBSE_NONE, "bad pwd fail");

  bad_pwd = false;
  fail_init_groups = true;
  decode_str(&pjob->ji_wattr[JOB_ATR_euser], "euser", NULL, "dbeer", 0);
  pwd = check_pwd(pjob);
  fail_unless(pwd != PBSE_NONE, "bad grp fail");

  pjob->ji_grpcache = NULL;
  fail_init_groups = false;
  fail_site_grp_check = true;
  decode_str(&pjob->ji_wattr[JOB_ATR_euser], "euser", NULL, "dbeer", 0);
  pwd = check_pwd(pjob);
  fail_unless(pwd != PBSE_NONE, "bad site fail");
  
  pjob->ji_grpcache = NULL;
  fail_site_grp_check = false;
  decode_str(&pjob->ji_wattr[JOB_ATR_euser], "euser", NULL, "dbeer", 0);
  pwd = check_pwd(pjob);
  fail_unless(pwd == PBSE_NONE);
  }
END_TEST

START_TEST(test_check_pwd_no_user)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  struct passwd *pwd = NULL;

  decode_str(&pjob->ji_wattr[JOB_ATR_euser], "euser", NULL, "bogus", 0);
  fail_unless(pwd == NULL, "check_pwd still succeeded with bogus user");
  }
END_TEST

START_TEST(test_check_pwd_adaptive_user)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  struct passwd *pwd = NULL;

  decode_str(&pjob->ji_wattr[JOB_ATR_euser], "euser", NULL, "adaptive", 0);
  fail_unless(pwd == NULL, "check_pwd still succeeded with bogus user");
  }
END_TEST

START_TEST(test_get_num_nodes_ppn)
  {
  int num_nodes = 0;
  int num_ppn = 0;
  char maxstr[1024];

  // invalid parameter tests
  
  fail_unless(get_num_nodes_ppn(NULL, &num_nodes, &num_ppn) != PBSE_NONE);
  fail_unless(get_num_nodes_ppn("20", NULL, &num_ppn) != PBSE_NONE);
  fail_unless(get_num_nodes_ppn("20", &num_nodes, NULL) != PBSE_NONE);

  // overflow tests
 
  // overflow in node count 
  snprintf(maxstr, sizeof(maxstr), "%llu", (long long unsigned)LONG_MAX+1ULL);
  fail_unless(get_num_nodes_ppn(maxstr, &num_nodes, &num_ppn) != PBSE_NONE);
  // clear errno
  errno = 0;

  // overflow in ppn
  snprintf(maxstr, sizeof(maxstr), "1:ppn=%llu", (long long unsigned)LONG_MAX+1ULL);
  fail_unless(get_num_nodes_ppn(maxstr, &num_nodes, &num_ppn) != PBSE_NONE);
  // clear errno
  errno = 0;

  // overflow in extra node count
  snprintf(maxstr, sizeof(maxstr), "1:ppn=2+%llu", (long long unsigned)LONG_MAX+1ULL);
  fail_unless(get_num_nodes_ppn(maxstr, &num_nodes, &num_ppn) != PBSE_NONE);
  // clear errno
  errno = 0;

  // invalid hostname tests

  fail_unless(get_num_nodes_ppn("!:ppn=10", &num_nodes, &num_ppn) != PBSE_NONE);
  fail_unless(get_num_nodes_ppn("20:ppn=10+*", &num_nodes, &num_ppn) != PBSE_NONE);

  // valid tests

  fail_unless((get_num_nodes_ppn("a", &num_nodes, &num_ppn) == PBSE_NONE) &&
    (num_nodes == 1) && (num_ppn == 1));
  fail_unless((get_num_nodes_ppn("20:ppn=10", &num_nodes, &num_ppn) == PBSE_NONE) &&
    (num_nodes == 20) && (num_ppn == 10));
  fail_unless((get_num_nodes_ppn("myhost:ppn=24", &num_nodes, &num_ppn) == PBSE_NONE) &&
    (num_nodes == 1) && (num_ppn == 24));
  fail_unless((get_num_nodes_ppn("myhost:ppn=24+myhost2+myhost3", &num_nodes, &num_ppn) == PBSE_NONE) &&
    (num_nodes == 3) && (num_ppn == 24));
  fail_unless((get_num_nodes_ppn("myhost:ppn=24+100+200", &num_nodes, &num_ppn) == PBSE_NONE) &&
    (num_nodes == 301) && (num_ppn == 24));
  fail_unless((get_num_nodes_ppn("500:ppn=48+100+200", &num_nodes, &num_ppn) == PBSE_NONE) &&
    (num_nodes == 800) && (num_ppn == 48));
  fail_unless((get_num_nodes_ppn("500:ppn=48+100+200+myhost", &num_nodes, &num_ppn) == PBSE_NONE) &&
    (num_nodes == 801) && (num_ppn == 48));
  fail_unless((get_num_nodes_ppn("500:ppn=29+myhost1+100+200+myhost2", &num_nodes, &num_ppn) == PBSE_NONE) &&
    (num_nodes == 802) && (num_ppn == 29));
  }
END_TEST


#if NO_SPOOL_OUTPUT == 1
START_TEST(test_save_supplementary_group_list)
  {
  int ngroups;
  gid_t *group_list;
  int rc;

  rc = save_supplementary_group_list(&ngroups, &group_list);
  fail_unless(rc == 0);
  fail_unless(ngroups > 0);
  fail_unless((rc == 0) && (group_list != NULL));

  if (rc == 0)
    free(group_list);
  }
END_TEST

START_TEST(restore_supplementary_group_list_test)
  {
  int ngroups;
  gid_t *group_list;
  int rc;

  rc = restore_supplementary_group_list(0, NULL);
  fail_unless(rc < 0);


  // must be root to run the following since they call setgroups()
  if (getuid() == 0)
    {
    int i;
    int ngroups_alt;
    gid_t *group_list_alt;

    // save the supplementary groups
    rc = save_supplementary_group_list(&ngroups, &group_list);
    fail_unless(rc == 0);

    // make a back up copy of the array
    group_list_alt = (gid_t *)malloc(ngroups * sizeof(gid_t));
    fail_unless(group_list_alt != NULL);
    memcpy(group_list_alt, group_list, ngroups * sizeof(gid_t));

    // set to a smaller set
    rc = setgroups(1, group_list);
    fail_unless(rc == 0);

    // restore the full set
    rc = restore_supplementary_group_list(ngroups, group_list);
    fail_unless(rc == 0);

    // check to make sure full set restored
    ngroups = getgroups(0, NULL);
    group_list = (gid_t *)malloc(ngroups * sizeof(gid_t));
    fail_unless(group_list != NULL);
    rc = getgroups(ngroups, group_list);

    for (i = 0; i < ngroups; i++)
      fail_unless(group_list[i] == group_list_alt[i]);

    free(group_list_alt);
    }
  }
END_TEST
#endif

START_TEST(test_become_the_user)
  {
  int rc;
  job *pjob;
  int uid;
  int gid;

  // must be root to run this test
  if (getuid() != 0)
    return;

  pjob = (job *)calloc(1, sizeof(job));
  fail_unless(pjob != NULL);

  pjob->ji_grpcache = (struct grpcache *)calloc(1, sizeof(struct grpcache));
  fail_unless(pjob->ji_grpcache != NULL);

  pjob->ji_qs.ji_un.ji_momt.ji_exuid = 500;
  pjob->ji_qs.ji_un.ji_momt.ji_exgid = 500;

  pjob->ji_grpcache->gc_ngroup = 1;
  pjob->ji_grpcache->gc_groups[0] = 500;

  // fork so we can test the setxid/setexid calls in the child
  rc = fork();
  fail_unless(rc != -1);

  if (rc > 0)
    {
    int status;

    // parent
    wait(&status);
    return;
    }

  // child
  
  rc = become_the_user(pjob, true);
  fail_unless(rc == PBSE_NONE);

  // check the group list, uid, gid
  uid = geteuid();
  gid = getegid();
  fail_unless(uid != 500);
  fail_unless(gid != 500);

  // put things back in place
  fail_unless(seteuid(0) == 0);
  fail_unless(setegid(0) == 0);

  rc = become_the_user(pjob, false);
  fail_unless(rc == PBSE_NONE);

  // check the uid, gid
  uid = getuid();
  gid = getgid();
  fail_unless(uid != 500);
  fail_unless(gid != 500);
  }
END_TEST

START_TEST(test_TMomCheckJobChild)
  {
  pjobexec_t TJE;
  int timeout_sec = 39;
  int count;
  int rc;
  int fd = 99;

  TJE.jsmpipe[0] = fd;
  TMomCheckJobChild(&TJE, timeout_sec, &count, &rc);
  fail_unless(global_poll_fd == fd);
  fail_unless(global_poll_timeout_sec == timeout_sec);
  }
END_TEST

START_TEST(test_update_path_attribute)
  {
  job *pjob = NULL;

  fail_unless(update_path_attribute(pjob, JOB_ATR_outpath) != 0);
  pjob = (job *)calloc(1, sizeof(job));
  fail_unless(update_path_attribute(pjob, JOB_ATR_LAST) != 0);
  fail_unless(update_path_attribute(pjob, JOB_ATR_outpath) == 0);
  fail_unless(update_path_attribute(pjob, JOB_ATR_errpath) == 0);
  }
END_TEST

Suite *start_exec_suite(void)
  {
  Suite *s = suite_create("start_exec_suite methods");

  TCase *tc_core = tcase_create("test bld_env_variables no reallocation");
  tcase_add_test(tc_core, test_bld_env_variables_no_realloc); 
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test bld_env_variables reallocating env pointers only"); 
  tcase_add_test(tc_core, test_bld_env_variables_realloc_env);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test bld_env_variables in reallocating mem block only");
  tcase_add_test(tc_core, test_bld_env_variables_realloc_block);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test bld_env_variables in reallocating both env pointers and mem block");
  tcase_add_test(tc_core, test_bld_env_variables_realloc_all);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_get_indices_from_exec_str");
  tcase_add_test(tc_core, test_get_indices_from_exec_str);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_check_pwd_euser");
  tcase_add_test(tc_core, test_check_pwd_euser);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_check_pwd_no_user");
  tcase_add_test(tc_core, test_check_pwd_no_user);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_check_pwd_adaptive_user");
  tcase_add_test(tc_core, test_check_pwd_adaptive_user);
  tcase_add_test(tc_core, job_nodes_test);
  tcase_add_test(tc_core, create_command_test);
  tcase_add_test(tc_core, no_hang_test);
  tcase_add_test(tc_core, exec_bail_test);
  tcase_add_test(tc_core, remove_leading_hostname_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_get_num_nodes_ppn");
  tcase_add_test(tc_core, test_get_num_nodes_ppn);
  tcase_add_test(tc_core, test_escape_spaces);
#ifdef PENABLE_LINUX_CGROUPS
  tcase_add_test(tc_core, get_memory_limit_from_resource_list_test);
#endif
  suite_add_tcase(s, tc_core);

#if NO_SPOOL_OUTPUT == 1
  tc_core = tcase_create("test_save_supplementary_group_list");
  tcase_add_test(tc_core, test_save_supplementary_group_list);
  tcase_add_test(tc_core, restore_supplementary_group_list_test);
  suite_add_tcase(s, tc_core);
#endif

  tc_core = tcase_create("test_become_the_user");
  tcase_add_test(tc_core, test_become_the_user);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_TMomCheckJobChild");
  tcase_add_test(tc_core, test_TMomCheckJobChild);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_update_path_attribute");
  tcase_add_test(tc_core, test_update_path_attribute);
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

#ifdef NUMA_SUPPORT
  node_boards[0].mic_start_index = 0;
  node_boards[1].mic_start_index = 2;
  node_boards[2].mic_start_index = 4;
#endif

  rundebug();
  sr = srunner_create(start_exec_suite());
  srunner_set_log(sr, "start_exec_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
