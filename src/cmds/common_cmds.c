#include "license_pbs.h" /* See here for the software license */

#include "common_cmds.h"
#include "pbs_config.h"
#include "pbs_ifl.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "pbs_error_db.h"

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


/* Copies env variable into an allocated string 
 * and escapes nested characters if necessary.
 */
int copy_env_value(
  memmgr **mm,            /* memory manager */
  char    *cur_val,
  char   **value)
  {
  char *tmpPtr;
  int   escape_count = 0, len_value = 0;
  char  escape_chars[] = {'"', '\'', '\\', '\n', ',', '\0'};

  len_value = strlen(cur_val);

  tmpPtr = cur_val;
  while (*tmpPtr)
    { 
    int escape_index = 0;
    for (; escape_chars[escape_index] != '\0'; escape_index++)
      {
      if (*tmpPtr == escape_chars[escape_index])
        {
        escape_count++;
        break;
        }
      }

    tmpPtr++;
    }

  calloc_or_fail(mm, value, len_value + escape_count + 1, "Allocating value for hash_map");

  if (escape_count > 0)
    {
    char *tmpVal = *value;
    tmpPtr = cur_val;
    while(*tmpPtr)
      {
      int escape_index = 0;
      for (; escape_chars[escape_index] != '\0'; escape_index++)
        {
        if (*tmpPtr == escape_chars[escape_index])
          {
          *tmpVal++ = '\\';
          break;
          }
        }

      *tmpVal++ = *tmpPtr++;
      }
    }
  else
    strncpy(*value, cur_val, len_value);

  return (PBSE_NONE);
  }


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
  /* check to make sure we got an '=' */
  if(tmp_char == NULL)
    return(PBSE_BAD_PARAMETER);

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
    /* Env values can contain nested commas which must be 
     * escaped to be preserved. */

    copy_env_value(mm, one_var + tmp_pos, value);
    strncpy(*name, one_var, len_name);
    }

  return(PBSE_NONE);
  } /* END parse_env_line() */


/* Reads all the environment variables from envp and populates
 * a hashmap for use through out the execution of the command
 */
void set_env_opts(
  memmgr **mm,            /* memory manager */
  job_data **env_attr,
  char **envp)
  {
  int   var_num = 0;
  char *name = NULL;
  char *value = NULL;
  int   rc = PBSE_NONE;

  while (envp[var_num] != NULL)
    {
    rc = parse_env_line(mm, envp[var_num], &name, &value);
    if (rc != PBSE_NONE) 
      {
      fprintf(stderr, "Malformed environment variable %s. Will not add to job environment\n", envp[var_num]);
      ;
      exit(1);
      }

/*      strtolower(name); */
    hash_add_item(mm, env_attr, name, value, ENV_DATA, SET);
    memmgr_free(mm, name); name = NULL;
    memmgr_free(mm, value); value = NULL;

    var_num++;
    }
  } /* END set_env_opts() */


/* A wrapper for client side memory allocation to handle it all the same way
 * return indicates success, failure will exit the run
 */
void calloc_or_fail(

  memmgr **mm,            /* memory manager */
  char   **dest,
  int      alloc_size,
  char    *err_msg)

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
  char *tmp_name = NULL;
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
      {
      if (delim == NULL && s != NULL)
        {
        alloc_size = strlen(s)+1;
        calloc_or_fail(mm, &tmp_name, alloc_size, "parse_variable_list name");
        memcpy(tmp_name, s, alloc_size);
        if ((hash_find(user_env, tmp_name, &hash_var)))
          {
          calloc_or_fail(mm, &name, alloc_size+8, "parse_variable_list name");
          memcpy(name, "pbs_var_", 8);
          memcpy(name+8, s, alloc_size);
          hash_add_or_exit(mm, dest_hash, name, hash_var->value, hash_var->var_type);
          }
        }
      break;
      }

    e = strchr(delim+1, ',');
    /* This is last value */
    if (!e)
      e = strchr(delim+1, '\0');

    /* Get the variable from the src hash */
    /* Set the variable from the incoming data */
    alloc_size = delim - s;
    /* the +8 is for prepending the value of pbs_var_ to the value
     * This is used and removed in build_var_list later */
    calloc_or_fail(mm, &name, alloc_size+8, "parse_variable_list name");
    memcpy(name, "pbs_var_", 8);
    memcpy(name+8, s, alloc_size);
    if ((e - delim) == 1)
      {
      calloc_or_fail(mm, &tmp_name, alloc_size, "parse_variable_list name");
      memcpy(tmp_name, s, alloc_size);
      if (hash_find(user_env, tmp_name, &hash_var))
        {
        hash_add_or_exit(mm, dest_hash, name, hash_var->value, hash_var->var_type);
        }
      else
        {
        hash_add_or_exit(mm, dest_hash, name, "", CMDLINE_DATA);
        }
      }
    else
      {
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




