#include <stdlib.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <vector>

#include "utils.h"
#include <string>
#include <vector>

extern int log_event_called;

char log_buffer[16384];
std::string msg_err;
int  LOGLEVEL;


void log_err(int errnum, const char *routine, const char *msg) {}

void log_event(
    
  int         eventtype,
  int         objclass,
  const char *objname,
  const char *text)

  {
  log_event_called = 1;
  }

void log_record(

  int         eventtype,  /* I */
  int         objclass,   /* I */
  const char *objname,    /* I */
  const char *text)       /* I */

  {
  msg_err = text;
  }


/*
 * status stings are in the format str1\0str2\0...strn\0
 *
 * This search function returns 1 if str is in the entire
 * status string, 0 otherwise
 */

int search_dynamic_string_status(

  std::vector<std::string>& status,
  char           *str)

  {
  char         first_char = str[0];

  for (unsigned int i = 0; i < status.size(); i++)
    {
    const char *ptr = status[i].c_str();
    while(*ptr != '\0')
      {
      if (*ptr == first_char)
        {
        if (!strncmp(str, ptr, strlen(str)))
          return(1);
        }
      ptr++;
      }
    }

  return(0);
  } /* END search_dynamic_string_status() */


int find_error_type(

  xmlNode *node)

  {
  return(0);
  }

int translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  indices.push_back(6142);
  indices.push_back(6143);
  return(PBSE_NONE);
  }
