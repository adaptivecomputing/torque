#include <stdlib.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>


#include "utils.h"
#include <string>
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>


char log_buffer[16384];






void log_err(int errnum, const char *routine, const char *msg) {}



/*
 * status stings are in the format str1\0str2\0...strn\0
 *
 * This search function returns 1 if str is in the entire
 * status string, 0 otherwise
 */

int search_dynamic_string_status(

<<<<<<< HEAD
  dynamic_string *status,
  char           *str,
  unsigned int   *offset)
=======
    boost::ptr_vector<std::string>& status,
  char           *str)
>>>>>>> master

  {
  char         first_char = str[0];

  for (boost::ptr_vector<std::string>::iterator i = status.begin(); i != status.end(); i++)
    {
    const char *ptr = i->c_str();
    while(*ptr != '\0')
      {
<<<<<<< HEAD
      if (!strncmp(str, status->str + i, strlen(str)))
        {
        if (offset != NULL)
          *offset = i;
        return(1);
        }
=======
      if (*ptr == first_char)
        {
        if (!strncmp(str, ptr, strlen(str)))
          return(1);
        }
      ptr++;
>>>>>>> master
      }
    }

  return(0);
  } /* END search_dynamic_string_status() */


int find_error_type(

  xmlNode *node)

  {
  return(0);
  }
