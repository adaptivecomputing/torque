#include "license_pbs.h" /* See here for the software license */

#include "common_cmds.h"
#include "pbs_config.h"
#include "pbs_ifl.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

void strtolower(
  char *value)
  {
  int pos = 0;
  
  while(value[pos] != '\0')
    {
    value[pos] = tolower(value[pos]);
    pos++;
    }

  } /* END strtolower() */


/* Parse a single command line and return the name/value
 * name & value are both allocated inside the function
 */
int parse_env_line(
  memmgr **mm,            /* memory manager */
  char *one_var,
  char **name,
  char **value)
  {
  char *tmp_char = NULL;
  int pos_eq = 0, tmp_pos = 0;
  int len_name = 0, len_value = 0, len_total = 0;

  len_total = strlen(one_var);
  tmp_char = strchr(one_var, '=');
  pos_eq = tmp_char - one_var;
  len_name = pos_eq;
  tmp_pos = pos_eq + 1;

  calloc_or_fail(mm, name, len_name+1, "Allocating name for hash_map");
  /* Remove preceeding spaces in an env var
   * This will NOT affect a env var that is all spaces
   */
  while (one_var[tmp_pos] == ' ')
    {
    tmp_pos++;
    }
  if (tmp_pos == len_total)
    {
    /* The value consists of ALL spaces, reset to = pos + 1 */
    tmp_pos = pos_eq + 1;
    }
  len_value = len_total - tmp_pos;
  if (len_value == 0)
    {
    *value = NULL;
    }
  else
    {
    calloc_or_fail(mm, value, len_value+1, "Allocating value for hash_map");
    strncpy(*name, one_var, len_name);
    strncpy(*value, one_var + tmp_pos, len_value);
    }

  return(len_name);
  } /* END parse_env_line() */


/* Reads all the environment variables from envp and populates
 * a hashmap for use through out the execution of the command
 */
void set_env_opts(
  memmgr **mm,            /* memory manager */
  job_data **env_attr,
  char **envp)
  {
  int len_name = 0, var_num = 0;
  char *name = NULL;
  char *value = NULL;

  while (envp[var_num] != NULL)
    {
    len_name = parse_env_line(mm, envp[var_num], &name, &value);
    if (value != NULL)
      {
      strtolower(name);
      hash_add_item(mm, env_attr, name, value, ENV_DATA, SET);
      memmgr_free(mm, name); name = NULL;
      memmgr_free(mm, value); value = NULL;
      }
    if (name) memmgr_free(mm, name);

    var_num++;
    }
  } /* END set_env_opts() */


/* A wrapper for client side memory allocation to handle it all the same way
 * return indicates success, failure will exit the run
 */
void calloc_or_fail(
  memmgr **mm,            /* memory manager */
  char **dest,
  int alloc_size,
  char *err_msg)
  {
  *dest = (char *)memmgr_calloc(mm, 1, alloc_size);
  if (*dest == NULL)
    {
    fprintf(stderr, "Allocation of %d bytes failed %s", alloc_size, err_msg);
    exit(1);
    }
  } /* END calloc_or_fail() */


/* This takes comma delimited list of name=value pairs.
 * If value is empty (i.e. name=,name1=...) the the value is pulled from the
 * env
 * return 1 on success, 0 on failure
 */
void parse_variable_list(
  memmgr   **mm,        /* memory manager */
  job_data **dest_hash, /* This is the dest hashmap for vars found */
  job_data  *user_env,  /* This is the source hashmap */
  int        var_type,  /* Type for vars not pulled from the source hash */
  int        op_type,   /* Op for vars not pulled from the source hash */
  char      *the_list)  /* name=value,name1=value1,etc to be parsed */
  {
  int alloc_size = 0;
  char *s = NULL;
  char *e = NULL;
  char *delim = NULL;
  char *name = NULL;
  char *val = NULL;
  job_data *hash_var = NULL;
  s = the_list;
  while (s)
    {
    delim = strpbrk(s, "=,");

    /* There is no = or , in the following string */
    /* The string is improperly formatted (, found when = should have been) */
    /* The start character is a , or = */
    if ((delim == NULL) || (*delim == ',') || (delim == s))
      break;

    e = strchr(delim+1, ',');
    /* This is last value */
    if (!e)
      e = strchr(delim+1, '\0');

    if ((e - delim) == 1)
      {
      /* Get the variable from the src hash */
      alloc_size = delim - s;
      calloc_or_fail(mm, &name, alloc_size, "parse_variable_list name");
      if (hash_find(user_env, name, &hash_var))
        hash_add_or_exit(mm, dest_hash, name, hash_var->value, hash_var->var_type);
      }
    else
      {
      /* Set the variable from the incoming data */
      alloc_size = delim - s;
      calloc_or_fail(mm, &name, alloc_size, "parse_variable_list name");
      strncpy(name, s, alloc_size);
      delim++; /* Move past the = */
      alloc_size = e - delim;
      calloc_or_fail(mm, &val, alloc_size, "parse_variable_list val");
      strncpy(val, delim, alloc_size);
      hash_add_or_exit(mm, dest_hash, name, val, var_type);
      }
    if (*e == '\0')
      s = NULL; /* End of string previously found */
    else
      s = e + 1; /* Set the start to one char beyond the , */
    }
  } /* END parse_variable_list() */




