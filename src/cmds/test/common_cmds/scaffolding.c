
#include <stdlib.h>
#include <stdio.h>
#include "u_hash_map_structs.h"
#include "u_memmgr.h"


int env_add_call = 0;
int env_del_call = 0;
int env_find_call = 0;
int tc_num = 0;

int hash_add_item(memmgr **mm, job_data **head, char *name, char *value, int var_type, int op_type)
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

int hash_del_item(memmgr **mm, job_data **head, char *name)
  {
  if (strcmp(name, "_") == 0)
    {
    env_del_call++;
    }
  return 0;
  }

int hash_find(job_data *head, char *name, job_data **env_var)
  {
  if (tc_num == 1)
    {
    env_find_call++;
    *env_var = (job_data *)calloc(sizeof(job_data), 1);
    (*env_var)->name = (char *)calloc(1, 10);
    (*env_var)->value = (char *)calloc(1, 10);
    strcpy((*env_var)->name, "_");
    strcpy((*env_var)->value, "CMD");
    }
  else if (tc_num == 2)
    {
    *env_var = (job_data *)calloc(sizeof(job_data), 1);
    (*env_var)->name = (char *)calloc(1, 10);
    (*env_var)->value = (char *)calloc(1, 10);
    strcpy((*env_var)->name, "me");
    strcpy((*env_var)->value, "notme");
    }
  return 1;
  }

void hash_add_or_exit(memmgr **mm, job_data **head, char *name, char *value, int var_type)
  {
  if (tc_num == 2)
    {
    *head = (job_data *)calloc(sizeof(job_data), 1);
    (*head)->name = (char *)calloc(1, 10);
    (*head)->value = (char *)calloc(1, 10);
    strcpy((*head)->name, "me");
    strcpy((*head)->value, "notme");
    }
  }
