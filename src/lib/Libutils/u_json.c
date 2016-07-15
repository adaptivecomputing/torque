#include "license_pbs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "log.h"
#include "utils.h"
#include "pbs_error.h"


/* 
 * get_numberic_value()
 *
 * get_numberic_value returns a number in the value parameter
 *
 * @param ptr  - pointer to the JSON string being parsed. Will
 *               point to the next name/value pair when returned.
 * @param token - pointer to object to be parsed
 * @param  value - pointer to json object to be returned.
 *
 */

int get_numeric_value(

  char **ptr,
  char *token,
  std::string &value)
 
  {
  char *endptr;
  char buf[256];
          
  endptr = token;
  while(isdigit(*endptr))
    {
    endptr++;
    }
                               
  move_past_whitespace(&endptr);
  /* endptr should be pointing to a comma */
  if ((*endptr != ',') && (*endptr != '}'))
    {
    sprintf(buf, "expected %c. %s", 'c', endptr);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    return(PBSE_IVALREQ);
    }
                                                              
  *endptr = 0;
  endptr++;
  value = token;
  *ptr = endptr;
  sprintf(buf, "%d", *endptr);
  return(PBSE_NONE);
  }
  
/* get_object_value()
 *
 * get_object_value() returns a JSON object starting with a '{' and ending with a '}'
 *
 * @param ptr  - pointer to the JSON string being parsed. Will
 *               point to the next name/value pair when returned.
 * @param  token - pointer to object to be parsed
 * @param  value - pointer to json object to be returned.
 *
 */

int get_object_value(
 
  char **ptr,
  char *token,
  std::string &value)
  
  {
  char *endptr;
  char  buf[256];
           
  endptr = strchr(token, '}');
  if (*endptr == '\0')
    {
    sprintf(buf, "expected %c. %s", '}', endptr);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    return(PBSE_IVALREQ);
    }
                                       
  endptr++;
  move_past_whitespace(&endptr);
  /* endptr should be pointing to a , or if the end of the string a } */
  if ((*endptr != ',') && (*endptr != '}'))
    {
    sprintf(buf, "expected , or }  %s", endptr);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    return(PBSE_IVALREQ);
    }
                                                                       
  *endptr = 0;
  endptr++;
  value = token;
  *ptr = endptr;
  return(PBSE_NONE);
  }


/* get_array_value()
 *
 * get_array_value returns the entire JSON array from the opening '[' to the closing ']'
 * of the array and returns it as a value. Note this function does not currently
 * handle nested arrays.
 *
 * @param ptr  - pointer to the JSON string being parsed. Will
 *               point to the next name/value pair when returned.
 * @param token - poinger to the JSON string where an array begins
 * @param value - The array value returned.
 */
int get_array_value(

  char **ptr,
  char *token,
  std::string &value)

  {
  char *endptr;
  char  buf[256];

  endptr = strchr(token, ']');
  if (*endptr == '\0')
    {
    sprintf(buf, "expected %c. %s", ']', endptr);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    return(PBSE_IVALREQ);
    }

  endptr++;
  move_past_whitespace(&endptr);

  /* endptr should be pointing to a , or if the end of the string a } */
  if ((*endptr != ',') && (*endptr != '}'))
    {
    sprintf(buf, "expected , or }  %s", endptr);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    return(PBSE_IVALREQ);
    }

  *endptr = 0;
  endptr++;
  value = token;
  *ptr = endptr;
  return(PBSE_NONE);
  }


/*
 * get_name_value_pair()
 *
 * get_name_value_pair gets a JSON name value pair separated by a ':'
 * and returns the name in valName and the value in value.
 * Following is the syntax for JSON we are trying to follow.
 *
 * Name/value pair
 *    "name":"value"
 *
 * Values can be 
 *   A number (integer or floating piont)
 *   A stirng (in double quotes )
 *   A Boolean (true or false)
 *   An array (in square brackets)
 *   An object (in curly braces 
 *   null
 *
 * Objects
 *   one or more name/value pairs enclosed with curly braces
 *   {"name":"value", "lastname":"Doe"}
 *
 * Arrays
 *   Contained inside square brackets. An array can contain multiple objects
 *
 *   "an_array" : [
 *     {"name":"value, "lastname":"doe"},
 *     {"name":"value, "lastname":"doe"},
 *     {"name":"value, "lastname":"doe"}
 *   ]
 *
 * @param ptr  -  a pointer to the head of the JSON string being parsed. 
 *                The ptr will point to the next name value pair when done.
 * @param valName - The name of the name/value pair
 * @param value   - The value of the name/value pair
 *
 */

int get_name_value_pair(

  char **ptr,
  std::string &valName,
  std::string &value)

  {
  char *token;
  char *endptr;
  char buf[256];
  int  rc;

  token = *ptr;

  /* find a name */
  move_past_whitespace(&token);

  if (*token != '"')
    {
    sprintf(buf, "invalid name value. quote \" excpected. %s",  token);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    return(PBSE_IVALREQ);
    }

  /* move the pointer to the first character of the name */
  token++;

  /* find the end of the name delimited by a '"' character */
  endptr = strchr(token, '"');
  if (endptr == NULL)
    {
    sprintf(buf, "expected quote '\"' to end name. %s", token);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    return(PBSE_IVALREQ);
    }

  *endptr = '\0';
  endptr++;

  valName = token;

  /* find the value */
  token = endptr;
  endptr = strchr(token, ':');

  if (endptr == NULL)
    {
    sprintf(buf, "expected :. %s", token);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    return(PBSE_IVALREQ);
    }

  token = endptr;
  token++;
  /* we found the : delimeter. move to the next non-white space character */
  move_past_whitespace(&token);


  /* is it a number? */
  if (isdigit(*token))
    {
    rc = get_numeric_value(ptr, token, value);
    }
  else if (*token == '{')
    {
    rc = get_object_value(ptr, token, value);
    }
  else if (*token == '[')
    {
    rc = get_array_value(ptr, token, value);
    }

  return(rc);
                                                            }


