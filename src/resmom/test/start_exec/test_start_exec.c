#include "license_pbs.h" /* See here for the software license */
#include "start_exec.h"
#include "test_start_exec.h"
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"

#define MAX_TEST_ENVP 150
#define MAX_TEST_BLOCK 8096

char *penv[MAX_TEST_ENVP]; /* max pointers a bld_env_variables will create */
char *envBuffer = NULL; /* points to the max block that bld_env_variables would ever need in this test suite */

void setup_vtable(
  struct var_table *vtable, 
  int v_envpsz, 
  int v_blocksz)

  {
  int i;

  memset(vtable, 0, sizeof(struct var_table));
  vtable->v_ensize = v_envpsz;
  vtable->v_envp = calloc(vtable->v_ensize, sizeof(char *));
  vtable->v_bsize = v_blocksz;
  vtable->v_block_start = calloc(1, vtable->v_bsize);
  vtable->v_block = vtable->v_block_start;

  for (i = 0; i < MAX_TEST_ENVP; ++i)
    penv[i] = NULL;

  envBuffer = calloc(MAX_TEST_BLOCK, sizeof(char));
  }

int add_env_variables(
  struct var_table *vtable,
  int n_env_vars, 
  int n_mem_to_use)

  {
  char *buf;
  char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghigjklmnopqrstuvwxyz";
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
    buf = calloc(siz, sizeof(char));
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
  sr = srunner_create(start_exec_suite());
  srunner_set_log(sr, "start_exec_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
