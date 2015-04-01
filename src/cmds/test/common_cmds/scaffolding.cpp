
#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "u_hash_map_structs.h"
#include "dynamic_string.h"


bool exit_called = false;
int env_add_call = 0;
int env_del_call = 0;
int env_find_call = 0;
int tc_num = 0;

int hash_add_item(job_data_container *head, const char *name, const char *value, int var_type, int op_type)
  {
  if ((strcmp(name, "ONE_VAR") == 0)
      && (strcmp(value, "ONE_VAL") == 0)
      && (tc_num == 1))
    {
    env_add_call++;
    }
  else if ((strcmp(name, "TWO_VAR") == 0)
      && (strcmp(value, "TWO_VAL") == 0)
      && (tc_num == 1))
    {
    env_add_call++;
    }
  else if ((strcmp(name, "THREE_VAR") == 0)
      && (strcmp(value, "   ") == 0)
      && (tc_num == 1))
    {
    env_add_call++;
    }
  else if ((strcmp(name, "_") == 0)
      && (strcmp(value, "CMD") == 0)
      && (tc_num == 1))
    {
    env_add_call++;
    }
  else if ((strcmp(name, "ARGV0") == 0)
      && (strcmp(value, "CMD") == 0)
      && (tc_num == 1))
    {
    env_add_call++;
    }
  return 0;
  }

int hash_del_item(job_data_container *head, const char *name)
  {
  if (strcmp(name, "_") == 0)
    {
    env_del_call++;
    }
  return 0;
  }

int hash_find(job_data_container *head, const char *name, job_data **env_var)
  {
  if (tc_num == 1)
    {
    env_find_call++;
    *env_var = new job_data(name,"CMD",0,0);
    }
  else if (tc_num == 2)
    {
    *env_var = new job_data("me","notme",0,0);
    }
  return 1;
  }

void hash_add_or_exit(job_data_container *head, const char *name, const char *value, int var_type)
  {
  if (tc_num == 2)
    {
    head->lock();
    head->insert(new job_data(name,value,var_type,0),name);
    head->unlock();
    }
  }

#if 0
int append_dynamic_string(dynamic_string *ds, const char *str)
  {
  strcat(ds->str, str);
  return(0);
  }

dynamic_string *get_dynamic_string(int size, const char *str)
  {
  dynamic_string *ds = (dynamic_string *)calloc(1, sizeof(dynamic_string));
  ds->str = (char *)calloc(1, 1024*10);
  return(ds);
  }

void free_dynamic_string(dynamic_string *ds)
  {
  free(ds->str);
  free(ds);
  }
#endif
