/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
*
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
*
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
*
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
*
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
*
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
*
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
*
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
*
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
*
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
*
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
*
* 7. DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/




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

  memmgr   **mm,            /* memory manager */
  job_data **dest_hash,   /* This is the dest hashmap for vars found */
  job_data  *user_env,     /* This is the source hashmap */
  int        var_type,           /* Type for vars not pulled from the source hash */
  int        op_type,            /* Op for vars not pulled from the source hash */
  char      *the_list)         /* name=value,name1=value1,etc to be parsed */

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




