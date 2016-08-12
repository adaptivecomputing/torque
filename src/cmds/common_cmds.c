#include "license_pbs.h" /* See here for the software license */

#include "common_cmds.h"
#include "pbs_config.h"
#include "pbs_ifl.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "pbs_error_db.h"
#include <pbs_constants.h>
#include <string>
#include "pbs_helper.h"

#define  JOB_ENV_START_SIZE 2048


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



void add_env_value_to_string(

  std::string &val,
  const char  *env_val_to_add)

  {
  const char *ptr = env_val_to_add;

  while (*ptr != '\0')
    {
    switch (*ptr)
      {
      case '"':
      case '\'':
      case '\\':
      case '\n':
      case ',':

        val += '\\';
        // fall through

      default:
        
        val += *ptr;

        break;
      }
    
    ptr++;
    }

  } /* END add_env_value_to_string() */


/* Copies env variable into an allocated string 
 * and escapes nested characters if necessary.
 */
int copy_env_value(
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

  calloc_or_fail(value, len_value + escape_count + 1, "Allocating value for hash_map");

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
  char    *one_var,
  char   **name,
  char   **value)

  {
  char *tmp_char = NULL;
  int   pos_eq = 0;
  int   tmp_pos = 0;
  int   len_name = 0;
  int   len_value = 0;
  int   len_total = 0;

  len_total = strlen(one_var);
  tmp_char = strchr(one_var, '=');

  /* check to make sure we got an '=' */
  if (tmp_char == NULL)
    return(PBSE_BAD_PARAMETER);

  pos_eq = tmp_char - one_var;
  len_name = pos_eq;
  tmp_pos = pos_eq + 1;

  calloc_or_fail(name, len_name+1, "Allocating name for hash_map");
  strncpy(*name, one_var, len_name);
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

    copy_env_value(one_var + tmp_pos, value);
    }

  return(PBSE_NONE);
  } /* END parse_env_line() */


/* Reads all the environment variables from envp and populates
 * a hashmap for use through out the execution of the command
 */
void set_env_opts(
  job_data_container *env_attr,
  char              **envp)

  {
  int   var_num = 0;
  char *name = NULL;
  char *value = NULL;
  int   rc = PBSE_NONE;

  while (envp[var_num] != NULL)
    {
    rc = parse_env_line(envp[var_num], &name, &value);
    if (rc == PBSE_NONE) 
      {
      hash_add_item(env_attr, name, value, ENV_DATA, SET);
      free(name); name = NULL;
      free(value); value = NULL;
      }

    var_num++;
    }
  } /* END set_env_opts() */


/* A wrapper for client side memory allocation to handle it all the same way
 * return indicates success, failure will exit the run
 */
void calloc_or_fail(
  char   **dest,
  int      alloc_size,
  const char    *err_msg)

  {
  *dest = (char *)calloc(1, alloc_size);
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

int parse_variable_list(

  job_data_container *dest_hash, /* This is the dest hashmap for vars found */
  job_data_container * UNUSED(user_env),  /* This is the source hashmap */
  int                  UNUSED(var_type),  /* Type for vars not pulled from the source hash */
  int                  UNUSED(op_type),   /* Op for vars not pulled from the source hash */
  char               *the_list)  /* name=value,name1=value1,etc to be parsed */

  {
  int             alloc_size = 0;
  std::string     job_env = "";
  char            name[JOB_ENV_START_SIZE];
  char           *s = NULL;
  char           *c = NULL;
  char           *delim = NULL;

  s = the_list;

  while (s)
    {
    delim = strpbrk(s, "=,");

    if (delim == s)
      {
      fprintf(stderr, "invalid -v syntax\n");
      return(3);
      }

    /* If delim is ','or NULL we have no value. Get the environment variable in s */ 
    /* If delim is '=' and delim+1 is ',' or NULL we also need to get 
       the environment variable in s */
    if (delim == NULL || *delim == ',' ||
       ((*delim == '=') && (*(delim + 1) == ',')) ||
       ((*delim == '=') && ((delim + 1) == NULL)))
      {
      if (delim == NULL)
        alloc_size = strlen(s);
      else
        alloc_size = delim - s;

      memcpy(name, s, alloc_size);
      name[alloc_size] = '\0';
      c = getenv(name);

      if (c != NULL)
        {
        job_env += name;
        job_env += "=";
        add_env_value_to_string(job_env, c);

        if (delim == NULL)
          s = NULL;
        else
          {
          job_env += ",";
          s = delim + 1;
          if (*s == ',') /* This ended in '='. Move one more */
            s++;
          }
        }
      else
        {
        /* No environment variable set for this name. Pass it on with value "" */
        if (delim == NULL)
          {
          snprintf(name, sizeof(name), "%s", s);
          job_env += "name";
          job_env += "=";
          s = NULL;
          }
        else
          {
          memcpy(name, s, delim - s);
          name[delim - s] = '\0';
          job_env += name;
          job_env += "=,";
          s = ++delim + 1;
          }
        }
      }
    else
      {
      /* We have a key value pair */
      delim = strchr(s, ',');
      if (delim == NULL)
        {
        /* we are at the end */
        job_env += s;
        s = NULL;
        }
      else
        {
        /* We have another variable in the list. Take care of the current one */
        alloc_size = delim - s;
        memcpy(name, s, alloc_size);
        name[alloc_size] = '\0';
        job_env += name;
        job_env += ",";
        s = delim + 1;
        }
      }
    }

  hash_add_or_exit(dest_hash, ATTR_v, job_env.c_str(), ENV_DATA);

  return(PBSE_NONE);
  } /* END parse_variable_list() */ 



