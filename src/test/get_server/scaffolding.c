#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pbs_ifl.h"
#include "cmds.h"

static char seq_number[PBS_MAXSEQNUM+PBS_MAXJOBARRAYLEN+1];
static int s_pos;
static char parent_server[MAXSERVERNAME];
static int p_pos;
static char current_server[MAXSERVERNAME];
static int c_pos;

#define ISNAMECHAR(x) ( (isgraph(x)) && ((x) != '#') && ( (x) != '@') )

extern "C"
  {
int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg)
  {
  return(0);
  }

  }

char *pbs_default()
  {
  return(strdup("napali"));
  }

int parse_jobid(
    
  const char *job_id, 
  char **arg_seq_number, 
  char **arg_parent_server, 
  char **arg_current_server)

  {
  int i;
  const char *c;

  /* initialize static data for this parsing call */

  for (i = 0; i < PBS_MAXSEQNUM + PBS_MAXJOBARRAYLEN + 1; i++) seq_number[i] = '\0';

  s_pos = 0;

  for (i = 0; i < MAXSERVERNAME; i++) parent_server[i] = '\0';

  p_pos = 0;

  for (i = 0; i < MAXSERVERNAME; i++) current_server[i] = '\0';

  c_pos = 0;

  /* Begin the parse */
  c = job_id;

  while (isspace(*c)) c++;

  /* Looking for a seq_number */
  while (*c != '\0')
    {
    /* look for a digit or array brackets */
    if (isdigit(*c) || *c == '[' || *c == ']')
      {
      if (s_pos >= PBS_MAXSEQNUM + PBS_MAXJOBARRAYLEN) return 3;

      seq_number[s_pos++] = *c;
      }
    else
      break;

    c++;
    }

  if (s_pos == 0) return 1;

  /* Looking for a parent_server */
  if (*c == '.')
    {
    c++;

    while (*c != '\0')
      {
      if (ISNAMECHAR(*c))
        {
        if (p_pos >= MAXSERVERNAME) return 3;

        parent_server[p_pos++] = *c;
        }
      else
        break;

      c++;
      }

    if (p_pos == 0) return 1;
    }

  /* Looking for a current_server */
  if (*c == '@')
    {
    c++;

    while (*c != '\0')
      {
      if (ISNAMECHAR(*c))
        {
        if (c_pos >= MAXSERVERNAME) return 3;

        current_server[c_pos++] = *c;
        }
      else
        break;

      c++;
      }

    if (c_pos == 0) return 1;
    }

  if (*c != '\0') return 2;

  /* set char * pointers to static data, to arguments */
  if (arg_seq_number != NULL) *arg_seq_number = seq_number;

  if (arg_parent_server != NULL) *arg_parent_server = parent_server;

  if (arg_current_server != NULL) *arg_current_server = current_server;

  return 0;
  }
