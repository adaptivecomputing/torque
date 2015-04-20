#include <stdlib.h>
#include <string.h>
#include <vector>

#include "req.hpp"
#include "pbs_error.h"
#include "utils.h"

// define the class constants
const int USE_CORES = 0;
const int USE_THREADS = 1;
const int ALLOW_THREADS = 2;
const int USE_FAST_CORES = 3;

const int PLACE_NO_PREFERENCE = -1;
const int PLACE_NODE = 0;
const int PLACE_SOCKET = 1;
const int PLACE_NUMA_CHIP = 2;
const int PLACE_CORES = 2;
const int PLACE_THREADS = 2;

const int ALL_EXECUTION_SLOTS = -1;

const char *use_cores = "usecores";
const char *use_threads = "usethreads";
const char *allow_threads = "allowthreads";
const char *use_fast_cores = "usefastcores";
const char *place_node = "node";
const char *place_socket = "socket";
const char *place_numa = "numachip";
const char *place_core = "core";
const char *place_thread = "thread";

req::req() : execution_slots(1), mem(0), swap(0), disk(0), nodes(0),
             socket(0), numa_chip(0), cores(0), threads(0), thread_usage_policy(ALLOW_THREADS), 
             thread_usage_str(allow_threads), gpus(0), mics(0), maxtpn(0), gres(), placement_str(), 
             gpu_mode(), task_count(1), pack(false), single_job_access(false), index(0) 

  {
  }



req::req(

  char *work_str) : execution_slots(1), mem(0), swap(0), disk(0), nodes(0), socket(0),
                    numa_chip(0), cores(0), threads(0), thread_usage_policy(ALLOW_THREADS),
                    thread_usage_str(allow_threads), gpus(0), mics(0), maxtpn(0), gres(),
                    features(), placement_str(), gpu_mode(), task_count(1), pack(false),
                    single_job_access(false), index(0)
  
  {
  char *ptr = work_str;
  int   node_count = strtol(ptr, &ptr, 10);
  int   ppn_len = strlen(":ppn=");
  int   mic_len = strlen(":mics=");
  int   gpu_len = strlen(":gpus=");
  int   ppn = 1;
  int   mic = 0;
  int   gpu = 0;

  // Handle a node name
  if (node_count == 0)
    {
    node_count = 1;
    ptr = strchr(ptr, ':');
    }

  while ((ptr != NULL) &&
         (*ptr != '\0'))
    {
    if (!strncmp(ptr, ":ppn=", ppn_len))
      {
      ptr += ppn_len;
      ppn = strtol(ptr, &ptr, 10);
      }
    else if (!strncmp(ptr, ":mics=", mic_len))
      {
      ptr += mic_len;
      mic = strtol(ptr, &ptr, 10);
      }
    else if (!strncmp(ptr, ":gpus=", gpu_len))
      {
      ptr += gpu_len;
      gpu = strtol(ptr, &ptr, 10);
      }
    else
      {
      // Feature. Advance to the next ':'
      ptr = strchr(ptr + 1, ':');
      }
    }

  this->task_count = node_count;
  this->execution_slots = ppn;
  this->gpus = gpu;
  this->mics = mic;
  } // END Constructor from resource list


    
req::req(
    
  const req &other) : execution_slots(other.execution_slots), 
                      mem(other.mem), 
                      swap(other.swap),
                      disk(other.disk), 
                      nodes(other.nodes), 
                      socket(other.socket), 
                      numa_chip(other.numa_chip), 
                      cores(other.cores), 
                      threads(other.threads), 
                      thread_usage_policy(other.thread_usage_policy),
                      thread_usage_str(other.thread_usage_str), 
                      gpus(other.gpus), 
                      mics(other.mics), 
                      maxtpn(other.maxtpn), 
                      gres(other.gres), 
                      os(other.os), 
                      arch(other.arch), 
                      node_access_policy(other.node_access_policy), 
                      features(other.features), 
                      placement_str(other.placement_str),
                      req_attr(other.req_attr),
                      gpu_mode(other.gpu_mode), 
                      task_count(other.task_count),
                      pack(other.pack), 
                      single_job_access(other.single_job_access),
                      index(other.index),
                      hostlist(other.hostlist)

  {
  }


/*
 * parse_positive_integer()
 *
 * Parses an integer from str and verifies it is positive
 *
 * @param str - the c string we're reading an integer from
 * @param parsed - where we're storing the integer
 */

int parse_positive_integer(

  const char *str,
  int        &parsed)

  {
  if (strchr(str, '.') != NULL)
    return(PBSE_BAD_PARAMETER);

  parsed = strtol(str, NULL, 10);

  if (parsed < 1)
    return(PBSE_BAD_PARAMETER);

  return(PBSE_NONE);
  } // END parse_positive_integer()



/*
 * set_place_value()
 *
 * parses the place value and updates req values appropriately.
 * the place value is in the format:
 * place={node|socket|numachip|core|thread}[=#]
 *
 * @param value - the string in the above format
 * @return PBSE_NONE as long as value is in the proper format,
 *         PBSE_BAD_PARAMTER otherwise
 */

int req::set_place_value(

  const char *value)

  {
  char *work_str = strdup(value);
  char *equals = strchr(work_str, '=');
  char *numeric_value = NULL;
  int   rc = PBSE_NONE;

  this->placement_str = value;

  if (equals != NULL)
    {
    *equals = '\0';
    numeric_value = equals + 1;
    }

  if (!strcmp(work_str, place_node))
    {
    if (numeric_value != NULL)
      rc = parse_positive_integer(numeric_value, this->nodes);
    else
      this->nodes = 1;
    }
  else if (!strcmp(work_str, place_socket))
    {
    if (numeric_value != NULL)
      rc = parse_positive_integer(numeric_value, this->socket);
    else
      this->socket = 1;
    }
  else if (!strcmp(work_str, place_numa))
    {
    if (numeric_value != NULL)
      rc = parse_positive_integer(numeric_value, this->numa_chip);
    else
      this->numa_chip = 1;
    }
  else if (!strcmp(work_str, place_core))
    {
    if (numeric_value != NULL)
      {
      int count;
      rc = parse_positive_integer(numeric_value, count);
      this->cores = count;
      }
    else
      this->cores = 1;
      
    this->thread_usage_policy = USE_CORES;
    this->thread_usage_str = use_cores;
    }
  else if (!strcmp(work_str, place_thread))
    {
    if (numeric_value != NULL)
      {
      int count;
      rc = parse_positive_integer(numeric_value, count);
      this->threads = count;
      }
    else
      this->threads = 1;
      
    this->thread_usage_policy = USE_THREADS;
    this->thread_usage_str = use_threads;
    }
  else
    rc = PBSE_BAD_PARAMETER;

  free(work_str);

  return(rc);
  } // END set_place_value() 



/*
 * read_mem_value()
 *
 * Determines a memory value from a string in the format
 * <number><units> and returns the value in kb. For example: 12 gb
 */

int read_mem_value(

  const char    *value,
  unsigned long &parsed)

  {
  char      *suffix;
  long long  memval = strtoll((char *)value, &suffix, 10);

  if (memval < 1)
    return(PBSE_BAD_PARAMETER);
  
  switch (*suffix)
    {
      /* We will multiply by 1024 later as well. Default 
         multiplier for memory is kilobytes. */
    case 't':
    case 'T':

      memval *= 1024 * 1024 * 1024;

      break;

    case 'g':
    case 'G':

      memval *= 1024 * 1024;

      break;

    case 'm':
    case 'M':

      memval *= 1024;

      break;
    }

  parsed = memval;

  return(PBSE_NONE);
  } // END read_mem_value()



int req::append_gres(

  const char *gres_value)

  {
  char *work_str = strdup(gres_value);
  char *gres_name = work_str;
  char *equals = strchr(gres_name, '=');
  int   value = 0;

  if (equals != NULL)
    {
    *equals = '\0';
    value = strtol(equals + 1, NULL, 10);

    if (value < 1)
      {
      free(work_str);
      return(-1);
      }
    }

  char       *current_gres = strdup(this->gres.c_str());
  char       *current = current_gres;
  char       *ptr;
  const char *delim = ":";
  char        buf[MAXLINE];
  bool        found = false;

  this->gres.clear();

  while ((ptr = threadsafe_tokenizer(&current, delim)) != NULL)
    {
    if (this->gres.size() > 0)
      this->gres += ":";

    int len = strlen(gres_name);

    if (!strncmp(ptr, gres_name, len))
      {
      if ((ptr[len] == '\0') ||
          (ptr[len] == '='))
        {
        this->gres += gres_name;

        if (value != 0)
          {
          sprintf(buf, "=%d", value);
          this->gres += buf;
          }

        found = true;
        }
      else
        this->gres += ptr;
      }
    else
      this->gres += ptr;
    }

  if (found == false)
    {
    this->gres += ":";
    this->gres += gres_value;
    }

  free(work_str);
  free(current_gres);
  
  return(PBSE_NONE);
  } // END append_gres()



/*
 * set_name_value_pair()
 *
 * Sets lprocs, memory, gpus, mics, place, maxtpn (ignored), gres,
 * features, disk, opsys, mode, or reqattr for this req. 
 *
 * @param name - the name of the value we're setting
 * @param value - the string to parse for the value
 * @return PBSE_NONE for properly formatted values
 *         PBSE_BAD_PARAMETER otherwise
 */

int req::set_name_value_pair(

  const char *name,
  const char *value)

  {
  int rc = PBSE_NONE;
  if (!strcmp(name, "lprocs"))
    {
    if (value[0] == 'a')
      this->execution_slots = ALL_EXECUTION_SLOTS;
    else
      rc = parse_positive_integer(value, this->execution_slots);
    }
  else if (!strcmp(name, "memory"))
    rc = read_mem_value(value, this->mem);
  else if (!strcmp(name, "gpus"))
    rc = parse_positive_integer(value, this->gpus);
  else if (!strcmp(name, "mics"))
    rc = parse_positive_integer(value, this->mics);
  else if (!strcmp(name, "place"))
    return(set_place_value(value));
  else if (!strcmp(name, "maxtpn"))
    rc = parse_positive_integer(value, this->maxtpn);
  else if (!strcmp(name, "gres"))
    {
    if (this->gres.size() > 0)
      append_gres(value);
    else
      this->gres = value;
    }
  else if (!strcmp(name, "feature"))
    this->features = value;
  else if (!strcmp(name, "disk"))
    rc = read_mem_value(value, this->disk);
  else if (!strcmp(name, "opsys"))
    this->os = value;
  else if (!strcmp(name, "reqattr"))
    this->req_attr = value;
  else if (!strcmp(name, "swap"))
    rc = read_mem_value(value, this->swap);
  else
    return(PBSE_BAD_PARAMETER);

  return(rc);
  } // END set_name_value_pair() 



/*
 * set_attribute()
 *
 * Sets an attribute that doesn't require a value. It has to be
 * in this format:
 * 
 * [:{usecores|usethreads|allowthreads|usefastcores}][:pack]
 * [:{shared|exclusive_thread|prohibited|exclusive_process|exclusive|default|reseterr}]
 *
 * @param str - the value in the format specified above.
 * @return PBSE_NONE if correct, PBSE_BAD_PARAMETER otherwise.
 */
int req::set_attribute(

  const char *str)

  {
  if (!strcmp(str, use_cores))
    {
    this->thread_usage_policy = USE_CORES;
    this->thread_usage_str = str;
    }
  else if (!strcmp(str, use_threads))
    {
    this->thread_usage_policy = USE_THREADS;
    this->thread_usage_str = str;
    }
  else if (!strcmp(str, allow_threads))
    {
    this->thread_usage_policy = ALLOW_THREADS;
    this->thread_usage_str = str;
    }
  else if (!strcmp(str, use_fast_cores))
    {
    this->thread_usage_policy = USE_FAST_CORES;
    this->thread_usage_str = str;
    }
  else if (!strcmp(str, "pack"))
    {
    this->pack = true;
    }
  else if ((!strcasecmp(str, "shared")) ||
           (!strcasecmp(str, "exclusive_thread")) ||
           (!strcasecmp(str, "prohibited")) ||
           (!strcasecmp(str, "exclusive_process")) ||
           (!strcasecmp(str, "exclusive")) ||
           (!strcasecmp(str, "default")) ||
           (!strcasecmp(str, "reseterr")))
    {
    if (this->gpu_mode.size() > 0)
      {
      this->gpu_mode += ":";
      this->gpu_mode += str;
      }
    else
      this->gpu_mode = str;
    }
  else
    return(PBSE_BAD_PARAMETER);

  return(PBSE_NONE);
  } // END set_attribute()



/*
 * set_value_from_string()
 *
 * Takes a string in the format name[=value]
 * and calls set_name_value_pair() if there is
 * an =value piece, otherwise calls set_attribute()
 *
 * @param str - the string we're parsing
 * @return - the PBSE_NONE if correct, PBSE_BAD_PARAMETER
 * otherwise
 */

int req::set_value_from_string(

  char *str)

  {
  char *equals = strchr(str, '=');
  char *value = NULL;

  if (equals != NULL)
    {
    *equals = '\0';
    value = equals + 1;

    return(set_name_value_pair(str, value));
    }
  else
    return(set_attribute(str));

  } // END set_value_from_string()



/*
 * check_for_parameter()
 *
 * Checks str for the presence of param and returns a pointer
 * to param if it is present, or NULL if absent
 *
 * @param str - the c string to check
 * @param param - the c string to look for
 * @param len - the length of the c string we're looking for
 * @return a pointer to the string matching param or NULL if not present
 */

char *check_for_parameter(

  char       *str,
  const char *param,
  int         len)

  {
  char *begin = strstr(str, param);
  bool  found = false;

  if (begin != NULL)
    {
    if ((begin == str) ||
        (*(begin - 1) == ':'))
      {
      char *end = begin + len;

      switch (*end)
        {
        case '=':
        case ':':
        case '\0':

          found = true;

          break;
        }
      }
    }

  if (found == false)
    return(NULL);
  else
    return(begin);
  } // END check_for_parameter()



/*
 * are_conflicting_params_present()
 *
 * Checks if any of the strings in conflicting values are duplicated
 * or if multiples are present and returns true if these conditions are
 * detected
 *
 * @param str - the c string to check against
 * @param conflicting_params - a list of values that cannot co-exit or be duplicated
 * @param error - the string to populate with more detailed information about the error 
 * if an error exists
 * @return - true if anything in conflicting_params is duplicated or if multiple values
 * from conflicting_params are present
 */

bool are_conflicting_params_present(

  char                     *str,
  std::vector<std::string> &conflicting_params,
  std::string              &error)

  {
  std::string first_found;

  for (unsigned int i = 0; i < conflicting_params.size(); i++)
    {
    char *found = check_for_parameter(str, conflicting_params[i].c_str(),
                                      conflicting_params[i].size());

    if (found != NULL)
      {
      // Two of the conflicting values were found 
      if (first_found.size() != 0)
        {
        // reseterr may be used in conjunction with exclusive_process and exclusive_thread
        if ((conflicting_params[i] == "reseterr") &&
            ((first_found == "exclusive_process") ||
             (first_found == "exclusive_thread")))
          continue;

        error = "The parameter ";
        error += first_found;
        error += " should not be specified with the parameter ";
        error += conflicting_params[i];
        error += ".";
        return(true);
        }
      else
        {
        // record the first value found in conflicting values
        first_found = conflicting_params[i];

        if (check_for_parameter(found + first_found.size(),
                                conflicting_params[i].c_str(),
                                conflicting_params[i].size()) != NULL)
          {
          // The value was repeated
          error = "The parameter ";
          error += first_found;
          error += " should not be duplicated.";
          return(true);
          }
        }
      }
    }
  
  return(false);
  } // END are_conflicting_params_present()



/*
 * is_present_twice()
 *
 * Checks str to see if check_for_me is in there twice and 
 * returns true if the string is duplicated
 */

bool is_present_twice(

  char              *str,
  const std::string &check_for_me,
  std::string       &error)

  {
  const char *check = check_for_me.c_str();
  char       *found = check_for_parameter(str, check, check_for_me.size());

  if (found != NULL)
    {
    if (check_for_parameter(found + check_for_me.size(), check, check_for_me.size()) != NULL)
      {
      error = "The parameter ";
      error += found;
      error += " should not be duplicated.";
      return(true);
      }
    }

  return(false);
  } // END is_present_twice



/*
 * submission_string_has_duplicates()
 *
 * Discovers if the submission string has duplicate entries, which
 * is not allowed
 * tasks=#[:lprocs=#|all][:memory=#][:place={node|socket|numachip|core|thread}[=#]]
 * [:{usecores|usethreads|allowthreads|usefastcores}][:pack][:maxtpn=#]
 * [:gpus=#][:mics=#][:gres=xxx[=#]][:feature=xxx]
 *
 * @param str - the submission c string
 * @param error - the string to populate if an error is found
 * @return true if duplicate entries are found in the string, else false
 */

bool req::submission_string_has_duplicates(

  char        *str,
  std::string &error)

  {
  if ((is_present_twice(str, "lprocs", error)) ||
      (is_present_twice(str, "memory", error)) ||
      (is_present_twice(str, "disk", error)) ||
      (is_present_twice(str, "place", error)) ||
      (is_present_twice(str, "pack", error)) ||
      (is_present_twice(str, "maxtpn", error)) ||
      (is_present_twice(str, "gpus", error)) ||
      (is_present_twice(str, "mics", error)) ||
      (is_present_twice(str, "gres", error)) ||
      (is_present_twice(str, "feature", error)) ||
      (is_present_twice(str, "opsys", error)) ||
      (is_present_twice(str, "reqattr", error)))
    {
    return(true);
    }

  std::vector<std::string> thread_use_values;
  std::vector<std::string> gpu_mode_values;

  thread_use_values.push_back(use_cores);
  thread_use_values.push_back(use_threads);
  thread_use_values.push_back(allow_threads);
  thread_use_values.push_back(use_fast_cores);

  gpu_mode_values.push_back("shared");
  gpu_mode_values.push_back("exclusive_thread");
  gpu_mode_values.push_back("prohibited");
  gpu_mode_values.push_back("exclusive_process");
  gpu_mode_values.push_back("exclusive");
  gpu_mode_values.push_back("default");
  gpu_mode_values.push_back("reseterr");

  if ((are_conflicting_params_present(str, thread_use_values, error)) ||
      (are_conflicting_params_present(str, gpu_mode_values, error)))
    {
    return(true);
    }
  
  return(false);
  } // END submission_string_has_duplicates()



/*
 * has_conflicting_values()
 *
 * Checks for conflicting values in the req and returns true if they exist
 *
 * @param error - the string to populate with an error message if one exists
 */

bool req::has_conflicting_values(

  std::string &error)

  {
  if ((this->execution_slots == ALL_EXECUTION_SLOTS) &&
      (strncmp(this->placement_str.c_str(), "node", 4)))
    {
    error = "-lprocs=all may only be used in conjunction with place=node";
    return(true);
    }

  return(false);
  } // END has_conflicting_values()



int req::submission_string_precheck(

  char        *submission_str,
  std::string &error)

  {
  if (strchr(submission_str, '+') != NULL)
    {
    error = "Multi-req syntax should use multiple -L parameters and not the '+' delimiter.";
    return(PBSE_BAD_PARAMETER);
    }

  if (submission_string_has_duplicates(submission_str, error))
    return(PBSE_BAD_PARAMETER);

  return(PBSE_NONE);
  } // END submission_string_precheck() 
    


/*
 * set_from_submission_string()
 *
 * initializes req from a string in the format:
 *
 * tasks=#[:lprocs=#|all][:memory=#][:disk=#][:place={node|socket|numachip|core|thread}[=#]]
 * [:{usecores|usethreads|allowthreads|usefastcores}][:pack][:maxtpn=#]
 * [:gpus=#][:mics=#][:gres=xxx[=#]][:feature=xxx][reqattr=<reqattr_val>]
 *
 * We will get only the part after tasks=
 *
 * @param submission_str - the string the format above
 * @param error - where error messages should be saved
 *
 * @return PBSE_NONE if correct, PBSE_BAD_PARAMETER otherwise
 */

int req::set_from_submission_string(
    
  char        *submission_str,
  std::string &error)

  {
  char       *current;
  int         rc;

  if (submission_string_precheck(submission_str, error))
    return(PBSE_BAD_PARAMETER);

  this->task_count = strtol(submission_str, &current, 10);

  if ((this->task_count < 1) ||
      (current == submission_str))
    {
    error = "Bad tasks value: '";
    error += submission_str;
    error += "'";
    return(PBSE_BAD_PARAMETER);
    }

  if (*current == ':')
    current++;
  else if (current != '\0')
    {
    error = "Invalid task specification";
    return(PBSE_BAD_PARAMETER);
    }

  while ((current != NULL) && 
         (*current != '\0'))
    {
    char *ptr = strchr(current, ':');

    if (ptr != NULL)
      *ptr = '\0';

    // set the current value
    if ((rc = set_value_from_string(current)) != PBSE_NONE)
      {
      char  buf[MAXLINE];
      char *err_txt = pbse_to_txt(rc);

      error = "value '";
      error += current;
      error += "' returned the error ";

      if (err_txt == NULL)
        snprintf(buf, sizeof(buf), "(%d): 'null'", rc);
      else
        snprintf(buf, sizeof(buf), "(%d): '%s'", rc, err_txt);

      error += buf;

      break;
      }

    // advance the position
    if (ptr == NULL)
      current = ptr;
    else
      current = ++ptr;
    }

  if (rc == PBSE_NONE)
    {
    if (has_conflicting_values(error) == true)
      return(PBSE_BAD_PARAMETER);
    }

  return(rc);
  } // END set_from_submission_string() 



/*
 * Constructor that accepts a string
 *
 * The qsub request comes in the format:
 *
 * tasks=#[:lprocs=#|all][:memory=#][:place={node|socket|numachip|core|thread}[=#]]
 * [:{usecores|usethreads|allowthreads|usefastcores}][:pack][:maxtpn=#]
 * [:gpus=#][:mics=#][:gres=xxx[=#]][:feature=xxx]
 *
 * We will get only the part after tasks=
 *
 * Sets the default values in the constructor before parsing so that values
 * which aren't requested are 
 */

req::req(

   const std::string &resource_request) : execution_slots(1), mem(0), swap(0), disk(0),
                                         nodes(0), socket(0), numa_chip(0),
                                         cores(0), threads(0), thread_usage_str(allow_threads),
                                         maxtpn(0), gres(), placement_str(), gpu_mode(), 
                                         task_count(1), single_job_access(false) 

  {
  char       *work_str = strdup(resource_request.c_str());
  std::string error;

  set_from_submission_string(work_str, error);

  free(work_str);
  }



/*
 * equals operator
 */

req &req::operator =(
    
  const req &other)

  {
  if (this == &other)
    return(*this);

  this->execution_slots = other.execution_slots;
  this->mem = other.mem;
  this->swap = other.swap;
  this->disk = other.disk;
  this->nodes = other.nodes;
  this->socket = other.socket;
  this->numa_chip = other.numa_chip;
  this->thread_usage_policy = other.thread_usage_policy;
  this->thread_usage_str = other.thread_usage_str;
  this->gpus = other.gpus;
  this->gpu_mode = other.gpu_mode;
  this->mics = other.mics;
  this->maxtpn = other.maxtpn;
  this->gres = other.gres;
  this->os = other.os;
  this->arch = other.arch;
  this->node_access_policy = other.node_access_policy;
  this->features = other.features;
  this->placement_str = other.placement_str;
  this->req_attr = other.req_attr;
  this->task_count = other.task_count;
  this->pack = other.pack;
  this->single_job_access = other.single_job_access;
  this->index = other.index;
  this->hostlist = other.hostlist;
  this->cores = other.cores;
  this->threads = other.threads;

  return(*this);
  } // END operator =



/*
 * toString()
 *
 * outputs the object in the format it will be displayed by qstat
 *
 */

void req::toString(
    
  std::string &str) const

  {
  char buf[100];

  snprintf(buf, sizeof(buf), "    req[%d]\n", this->index);
  str += buf;

  snprintf(buf, sizeof(buf), "      task count: %d\n", this->task_count);
  str += buf;

  if (this->execution_slots != 0)
    {
    if (this->execution_slots == ALL_EXECUTION_SLOTS)
      str += "      lprocs: all\n";
    else
      {
      snprintf(buf, sizeof(buf), "      lprocs: %d\n", this->execution_slots);
      str += buf;
      }
    }

  if (this->mem != 0)
    {
    snprintf(buf, sizeof(buf), "      mem: %lukb\n", this->mem);
    str += buf;
    }

  if (this->swap != 0)
    {
    snprintf(buf, sizeof(buf), "      swap: %lukb\n", this->swap);
    str += buf;
    }

  if (this->disk != 0)
    {
    snprintf(buf, sizeof(buf), "      disk: %lukb\n", this->disk);
    str += buf;
    }

  if (this->nodes != 0)
    {
    snprintf(buf, sizeof(buf), "      node: %d\n", this->nodes);
    str += buf;
    }

  if (this->socket != 0)
    {
    snprintf(buf, sizeof(buf), "      socket: %d\n", this->socket);
    str += buf;
    }

  if (this->numa_chip != 0)
    {
    snprintf(buf, sizeof(buf), "      numachip: %d\n", this->numa_chip);
    str += buf;
    }

  if (this->cores != 0)
    {
    snprintf(buf, sizeof(buf), "      core: %d\n", this->cores);
    str += buf;
    }

  if (this->threads != 0)
    {
    snprintf(buf, sizeof(buf), "      thread: %d\n", this->threads);
    str += buf;
    }

  if (this->gpus != 0)
    {
    snprintf(buf, sizeof(buf), "      gpus: %d\n", this->gpus);
    str += buf;

    if (this->gpu_mode.size() != 0)
      {
      str += "      gpu mode: ";
      str += this->gpu_mode;
      str += '\n';
      }
    }

  if (this->mics != 0)
    {
    snprintf(buf, sizeof(buf), "      mics: %d\n", this->mics);
    str += buf;
    }

  if (this->maxtpn != 0)
    {
    snprintf(buf, sizeof(buf), "      max tpn: %d\n", this->maxtpn);
    str += buf;
    }

  str += "      thread usage policy: ";
  str += this->thread_usage_str;
  str += '\n';

  if (this->placement_str.size() != 0)
    {
    str += "      placement type: ";
    str += this->placement_str;
    str += '\n';
    }

  if (this->req_attr.size() != 0)
    {
    str += "      reqattr: ";
    str += this->req_attr;
    str += '\n';
    }

  if (this->gres.size() != 0)
    {
    str += "      gres: ";
    str += this->gres;
    str += '\n';
    }

  if (this->os.size() != 0)
    {
    str += "      os: ";
    str += this->os;
    str += '\n';
    }

  if (this->arch.size() != 0)
    {
    str += "      arch: ";
    str += this->arch;
    str += '\n';
    }

  if (this->hostlist.size() != 0)
    {
    str += "      hostlist: ";
    str += this->hostlist;
    str += '\n';
    }

  if (this->features.size() != 0)
    {
    str += "      features: ";
    str += this->features;
    str += '\n';
    }

  if (this->single_job_access == true)
    str += "      single job access\n";

  if (this->pack == true)
    str += "      pack\n";
  } // END toString() 



/*
 * get_values()
 *
 * Gets the name and value of each part of this class that is set
 *
 * For each value that is set:
 * name=<name of field>:index value=<value of field
 * i.e. name=lprocs[index] value=10
 *
 * @param names - the vector where each name is stored
 * @param values - the vector where each value is stored
 *
 */

void req::get_values(
    
  std::vector<std::string> &names,
  std::vector<std::string> &values) const

  {
  char buf[100];

  snprintf(buf, sizeof(buf), "task_count.%d", this->index);
  names.push_back(buf);
  snprintf(buf, sizeof(buf), "%d", this->task_count);
  values.push_back(buf);
  
  if (this->execution_slots != 0)
    {
    snprintf(buf, sizeof(buf), "lprocs.%d", this->index);
    names.push_back(buf);
    if (this->execution_slots == ALL_EXECUTION_SLOTS)
      values.push_back("all");
    else
      {
      snprintf(buf, sizeof(buf), "%d", this->execution_slots);
      values.push_back(buf);
      }
    }

  if (this->mem != 0)
    {
    snprintf(buf, sizeof(buf), "memory.%d", this->index);
    names.push_back(buf);
    snprintf(buf, sizeof(buf), "%lukb", this->mem);
    values.push_back(buf);
    }

  if (this->swap != 0)
    {
    snprintf(buf, sizeof(buf), "swap.%d", this->index);
    names.push_back(buf);
    snprintf(buf, sizeof(buf), "%lukb", this->swap);
    values.push_back(buf);
    }

  if (this->disk != 0)
    {
    snprintf(buf, sizeof(buf), "disk.%d", this->index);
    names.push_back(buf);
    snprintf(buf, sizeof(buf), "%lukb", this->disk);
    values.push_back(buf);
    }

  if (this->nodes != 0)
    {
    snprintf(buf, sizeof(buf), "node.%d", this->index);
    names.push_back(buf);
    snprintf(buf, sizeof(buf), "%d", this->nodes);
    values.push_back(buf);
    }

  if (this->socket != 0)
    {
    snprintf(buf, sizeof(buf), "socket.%d", this->index);
    names.push_back(buf);
    snprintf(buf, sizeof(buf), "%d", this->socket);
    values.push_back(buf);
    }

  if (this->numa_chip != 0)
    {
    snprintf(buf, sizeof(buf), "numachip.%d", this->index);
    names.push_back(buf);
    snprintf(buf, sizeof(buf), "%d", this->numa_chip);
    values.push_back(buf);
    }

  if (this->cores != 0)
    {
    snprintf(buf, sizeof(buf), "core.%d", this->index);
    names.push_back(buf);
    snprintf(buf, sizeof(buf), "%d", this->cores);
    values.push_back(buf);
    }

  if (this->threads != 0)
    {
    snprintf(buf, sizeof(buf), "thread.%d", this->index);
    names.push_back(buf);
    snprintf(buf, sizeof(buf), "%d", this->threads);
    values.push_back(buf);
    }

  if (this->gpus != 0)
    {
    snprintf(buf, sizeof(buf), "gpus.%d", this->index);
    names.push_back(buf);
    snprintf(buf, sizeof(buf), "%d", this->gpus);
    values.push_back(buf);

    if (this->gpu_mode.size() != 0)
      {
      snprintf(buf, sizeof(buf), "gpu_mode.%d", this->index);
      names.push_back(buf);
      values.push_back(this->gpu_mode);
      }
    }

  if (this->mics != 0)
    {
    snprintf(buf, sizeof(buf), "mics.%d", this->index);
    names.push_back(buf);
    snprintf(buf, sizeof(buf), "%d", this->mics);
    values.push_back(buf);
    }

  if (this->maxtpn != 0)
    {
    snprintf(buf, sizeof(buf), "maxtpn.%d", this->index);
    names.push_back(buf);
    snprintf(buf, sizeof(buf), "%d", this->maxtpn);
    values.push_back(buf);
    }

  snprintf(buf, sizeof(buf), "thread_usage_policy.%d", this->index);
  names.push_back(buf);
  values.push_back(this->thread_usage_str);

  if (this->req_attr.size() != 0)
    {
    snprintf(buf, sizeof(buf), "reqattr.%d", this->index);
    names.push_back(buf);
    values.push_back(this->req_attr);
    }

  if (this->gres.size() != 0)
    {
    snprintf(buf, sizeof(buf), "gres.%d", this->index);
    names.push_back(buf);
    values.push_back(this->gres);
    }

  if (this->os.size() != 0)
    {
    snprintf(buf, sizeof(buf), "opsys.%d", this->index);
    names.push_back(buf);
    values.push_back(this->os);
    }

  if (this->arch.size() != 0)
    {
    snprintf(buf, sizeof(buf), "arch.%d", this->index);
    names.push_back(buf);
    values.push_back(this->arch);
    }

  if (this->features.size() != 0)
    {
    snprintf(buf, sizeof(buf), "features.%d", this->index);
    names.push_back(buf);
    values.push_back(this->features);
    }

  if (this->single_job_access == true)
    {
    snprintf(buf, sizeof(buf), "single_job_access.%d", this->index);
    names.push_back(buf);
    values.push_back("true");
    }

  if (this->pack == true)
    {
    snprintf(buf, sizeof(buf), "pack.%d", this->index);
    names.push_back(buf);
    values.push_back("true");
    }

  if (this->hostlist.size() != 0)
    {
    snprintf(buf, sizeof(buf), "hostlist.%d", this->index);
    names.push_back(buf);
    values.push_back(this->hostlist);
    }
  } // END get_values() 



char *capture_until_newline_and_advance(

  char **current_ptr)

  {
  char *retval = *current_ptr;
  char *current = *current_ptr;
  current = strchr(current, '\n');

  if (current != NULL)
    {
    *current = '\0';
    current++;
    move_past_whitespace(&current);
    }

  *current_ptr = current;
  return(retval);
  } // END capture_until_newline_and_advance()



/*
 * set_from_string()
 *
 * Sets the values for this req from the string passed in. This string
 * must be in the format produced by toString(), which is below, ignoring
 * whitespace:
 *
 * req[<index>]
 * task count: <task_count>
 * [lprocs: <execution_slots>]
 * [mem: <mem>kb]
 * [swap: <swap>kb]
 * [disk: <swap>kb]
 * [socket: <sockets>]
 * [numa chips: <numa chips>]
 * [gpus: <gpus> [gpu mode: <gpu mode>]]
 * [mics: <mics>]
 * [max tpn: <maxtpn>]
 * thread usage policy: <thread usage policy>
 * placement type: <placement type>
 * [reqattr: <reqattr>]
 * [gres: <gres info>]
 * [os: <os info>]
 * [arch: <arch info>]
 * [hostlist: <hostlist>]
 * [features: <features>]
 * [single job access]
 * [pack]
 */

void req::set_from_string(

  const std::string &req_str)

  {
  char *req = strdup(req_str.c_str());
  char *current = strchr(req, '[');

  if (current == NULL)
    return;

  this->index = strtol(current, &current, 10);

  // read the task count
  current = strchr(current, ':');
  if (current == NULL)
    return;

  current += 2; // move past the ': '
  this->task_count = strtol(current, &current, 10);

  while (is_whitespace(*current))
    current++;

  if (!strncmp(current, "lprocs", 6))
    {
    // found lprocs
    current += 8; // move past 'lprocs: '

    if (*current == 'a')
      {
      this->execution_slots = ALL_EXECUTION_SLOTS;
      current += 3; // move past 'all'
      }
    else
      this->execution_slots = strtol(current, &current, 10);

    move_past_whitespace(&current); 
    }
  
  if (!strncmp(current, "mem", 3))
    {
    // found mem
    current += 5; // move past 'mem: '
    this->mem = strtoll(current, &current, 10);
    current += 2; // move past 'kb'

    move_past_whitespace(&current); 
    }

  if (!strncmp(current, "swap", 4))
    {
    // found swap
    current += 6; // move past 'swap: '
    this->swap = strtoll(current, &current, 10);
    current += 2; // move past kb
    
    move_past_whitespace(&current);
    }

  if (!strncmp(current, "disk", 4))
    {
    current += 6; // move past 'disk: '
    this->disk = strtoll(current, &current, 10);
    current += 2; // move past kb

    move_past_whitespace(&current);
    }

  if (!strncmp(current, "node", 4))
    {
    current += 6; // move past 'node: '
    this->nodes = strtol(current, &current, 10);

    move_past_whitespace(&current);
    }

  if (!strncmp(current, "socket", 6))
    {
    current += 8; // move past 'socket: '
    this->socket = strtol(current, &current, 10);
    
    move_past_whitespace(&current);
    }

  if (!strncmp(current, "numachip", 8))
    {
    current += 10; // move past 'numachip: '
    this->numa_chip = strtol(current, &current, 10);

    move_past_whitespace(&current);
    }

  if (!strncmp(current, "core", 4))
    {
    current += 6; // move past 'core: '
    this->cores = strtol(current, &current, 10);

    move_past_whitespace(&current);
    }

  if ((!strncmp(current, "thread", 6)) &&
      (strncmp(current, "thread usage policy", 19)))
    {
    current += 8; // move past 'thread: '
    this->threads = strtol(current, &current, 10);

    move_past_whitespace(&current);
    }

  if (!strncmp(current, "gpus", 4))
    {
    current += 6; // move past 'gpus: '
    this->gpus = strtol(current, &current, 10);

    move_past_whitespace(&current);

    if (!strncmp(current, "gpu mode", 8))
      {
      current += 10; // move past 'gpu mode: '
      this->gpu_mode = capture_until_newline_and_advance(&current);
      }
    }

  if (!strncmp(current, "mics", 4))
    {
    current += 6; // move past 'gpus: '
    this->mics = strtol(current, &current, 10);

    move_past_whitespace(&current);
    }

  if (!strncmp(current, "max tpn", 7))
    {
    current += 9; // move past 'max tpn: '
    this->maxtpn = strtol(current, &current, 10);
    
    move_past_whitespace(&current);
    }

  if (!strncmp(current, "thread usage policy", 19))
    {
    current += 21; // move past 'thread usage policy: '

    this->thread_usage_str = capture_until_newline_and_advance(&current);

    if (this->thread_usage_str == use_cores)
      this->thread_usage_policy = USE_CORES;
    else if (this->thread_usage_str == use_threads)
      this->thread_usage_policy = USE_THREADS;
    else if (this->thread_usage_str == allow_threads)
      this->thread_usage_policy = ALLOW_THREADS;
    else
      this->thread_usage_policy = USE_FAST_CORES;
    }

  if (!strncmp(current, "placement type", 14))
    {
    current += 16; // move past 'placement type: '
    
    this->placement_str = capture_until_newline_and_advance(&current);

    if (current == NULL)
      {
      free(req);
      return;
      }
    }

  if ((current != NULL) &&
      (!strncmp(current, "reqattr", 7)))
    {
    current += 9; // move past 'reqattr: '
    this->req_attr = capture_until_newline_and_advance(&current);
    }

  if ((current != NULL) &&
      (!strncmp(current, "gres", 4)))
    {
    current += 6; // move past 'gres: '

    char *tmp = current;
    current = strchr(current, '\n');
    *current = '\0';
    current++;
    this->gres = tmp;

    move_past_whitespace(&current);
    }

  if ((current != NULL) &&
      (!strncmp(current, "os", 2)))
    {
    current += 4; // move past 'os: '

    char *tmp = current;
    current = strchr(current, '\n');
    *current = '\0';
    current++;
    this->os = tmp;

    move_past_whitespace(&current);
    }

  if ((current != NULL) &&
      (!strncmp(current, "arch", 4)))
    {
    current += 6; // move past 'arch: '

    char *tmp = current;
    current = strchr(current, '\n');
    *current = '\0';
    current++;
    this->arch = tmp;

    move_past_whitespace(&current);
    }

  if ((current != NULL) &&
      (!strncmp(current, "hostlist", 8)))
    {
    current += 10; // move past 'hostlist: '

    char *tmp = current;
    current = strchr(current, '\n');
    if (current != NULL)
    {
      *current = '\0';
      current++;
      move_past_whitespace(&current);
    }
    this->hostlist = tmp;

    }

  if ((current != NULL) &&
      (!strncmp(current, "features", 8)))
    {
    current += 10; // move past 'features: '

    char *tmp = current;
    current = strchr(current, '\n');
    *current = '\0';
    current++;
    this->features = tmp;

    move_past_whitespace(&current);
    }

  if ((current != NULL) &&
      (!strncmp(current, "single job access", 17)))
    {
    current += 17;
    this->single_job_access = true;

    move_past_whitespace(&current);
    }

  if ((current != NULL) &&
      (!strncmp(current, "pack", 4)))
    {
    this->pack = true;
    }

  free(req);
  } // END set_from_string() 



/*
 * set_value()
 *
 * Sets a value on this class identified by name
 * @param name - the name of the value to be set
 * @param value - the value
 * @return PBSE_NONE if the name is valid or PBSE_BAD_PARAMETER for an invalid name
 */

int req::set_value(

  const char *name,
  const char *value)

  {
  if (!strncmp(name, "index", 5))
    this->index = strtol(value, NULL, 10);
  else if (!strncmp(name, "task_count", 10))
    this->task_count = strtol(value, NULL, 10);
  else if (!strncmp(name, "lprocs", 6))
    {
    if (value[0] == 'a')
      this->execution_slots = ALL_EXECUTION_SLOTS;
    else
      this->execution_slots = strtol(value, NULL, 10);
    }
  else if (!strncmp(name, "memory", 6))
    this->mem = strtoll(value, NULL, 10);
  else if (!strncmp(name, "swap", 4))
    this->swap = strtoll(value, NULL, 10);
  else if (!strncmp(name, "disk", 4))
    this->disk = strtoll(value, NULL, 10);
  else if (!strncmp(name, "nodes", 5))
    this->nodes = strtol(value, NULL, 10);
  else if (!strncmp(name, "socket", 7))
    {
    this->socket = strtol(value, NULL, 10);
    this->placement_str = place_socket;
    }
  else if (!strncmp(name, "numachip", 10))
    {
    this->numa_chip = strtol(value, NULL, 10);
    this->placement_str = place_numa;
    }
  else if (!strncmp(name, "gpus", 4))
    this->gpus = strtol(value, NULL, 10);
  else if (!strncmp(name, "gpu_mode", 8))
    this->gpu_mode += value;
  else if (!strncmp(name, "mics", 4))
    this->mics = strtol(value, NULL, 10);
  else if (!strncmp(name, "maxtpn", 6))
    this->maxtpn = strtol(value, NULL, 10);
  else if (!strncmp(name, "thread_usage_policy", 19))
    this->thread_usage_str = value;
  else if (!strncmp(name, "placement_type", 14))
    this->placement_str = value;
  else if (!strncmp(name, "reqattr", 7))
    this->req_attr = value;
  else if (!strncmp(name, "gres", 4))
    this->gres = value;
  else if (!strncmp(name, "opsys", 5))
    this->os = value;
  else if (!strncmp(name, "arch", 4))
    this->arch = value;
  else if (!strncmp(name, "features", 8))
    this->features = value;
  else if (!strncmp(name, "single_job_access", 17))
    this->single_job_access = true;
  else if (!strncmp(name, "pack", 4))
    this->pack = true;
  else if (!strncmp(name, "hostlist", 8))
    this->hostlist = value;
  else
    return(PBSE_BAD_PARAMETER);

  return(PBSE_NONE);
  } // END set_value()



int req::getExecutionSlots() const

  {
  return(this->execution_slots);
  }

unsigned long req::getMemory() const 

  {
  return(this->mem);
  }

unsigned long req::getSwap() const

  {
  return(this->swap);
  }

unsigned long req::getDisk() const

  {
  return(this->disk);
  }

std::string req::getGres() const

  {
  return(this->gres);
  }

std::string req::getOS() const

  {
  return(this->os);
  }

std::string req::getNodeAccessPolicy() const

  {
  return(this->node_access_policy);
  }

std::string req::getPlacementType() const

  {
  return(this->placement_str);
  }

int req::getTaskCount() const

  {
  return(this->task_count);
  }

std::string req::getHostlist() const

  {
  return(this->hostlist);
  }

std::string req::getFeatures() const

  {
  return(this->features);
  }

std::string req::getThreadUsageString() const

  {
  return(this->thread_usage_str);
  }

void req::set_index(

  int index)

  {
  this->index = index;
  }
    
int req::getMaxtpn() const

  {
  return(this->maxtpn);
  }

std::string req::getGpuMode() const

  {
  return(this->gpu_mode);
  }

std::string req::getReqAttr() const

  {
  return(this->req_attr);
  }

int req::getIndex() const

  {
  return(this->index);
  }

int req::getGpus() const

  {
  return(this->gpus);
  }

int req::getMics() const

  {
  return(this->mics);
  }



/*
 * get_num_tasks_for_host()
 * Based on the hostlist, determines the number of tasks from this req assigned 
 * to this host
 * hostlist is in the format:
 * hostname1/<index range[+hostname2/<index range>[...]]
 * index range is in the format:
 * digit[-digit][,digit][...]
 * We find the ratio of the number of tasks assigned to this host to the number of cores per task
 * That raio is the number of tasks assigned to this host
 *
 * @param host - the hostname for which we're determining the number of tasks
 * @return the number of tasks assigned to this host (can be 0)
 */

int req::get_num_tasks_for_host(

  const std::string &host) const

  {
  int         task_count = 0;
  std::size_t pos = this->hostlist.find(host);
  unsigned int         offset = pos + host.size();

  if (pos != std::string::npos)
    {
    if ((this->execution_slots == ALL_EXECUTION_SLOTS) ||
        (!strncmp(this->placement_str.c_str(), "node", 4)))
      task_count = 1;
    else if ((this->hostlist.size() <= offset) ||
             ((this->hostlist.at(offset) != ':') &&
              (this->hostlist.at(offset) != '/')))
      task_count = 1;
    else
      {
      int num_ppn = 0;

      // handle both :ppn=X and /range
      // + 5 for ':ppn='
      if (this->hostlist.at(pos + offset) == '/')
        {
        char             *range_str = strdup(this->hostlist.substr(pos + offset +1).c_str());
        char             *ptr;
        std::vector<int>  indices;

        // truncate any further entries 
        if ((ptr = strchr(range_str, '+')) != NULL)
          *ptr = '\0';

        translate_range_string_to_vector(range_str, indices);
        num_ppn = indices.size();

        free(range_str);
        }
      else
        {
        std::string  ppn_val = this->hostlist.substr(pos + offset + 5);
        char        *ppn_str = strdup(ppn_val.c_str());
        
        num_ppn = strtol(ppn_str, NULL, 10);

        free(ppn_str);
        }
        
      task_count = num_ppn / this->execution_slots;
      }
    }

  return(task_count);
  } // END get_num_tasks_for_host()



/*
 * get_swap_for_host()
 *
 * @param host - the host whose swap we're calculating
 * @return the number of kb of swap for this host
 */

unsigned long req::get_swap_for_host(

  const std::string &host) const

  {
  int           num_tasks = this->get_num_tasks_for_host(host);
  unsigned long swap = this->swap * num_tasks;

  return(swap);
  } // END get_swap_for_host()



/*
 * get_memory_for_host()
 *
 * @param host - the host whose memory we're calculating
 * @return the number of kb of memory for this host
 */

unsigned long req::get_memory_for_host(

  const std::string &host) const

  {
  int           num_tasks = this->get_num_tasks_for_host(host);
  unsigned long mem = this->mem * num_tasks;

  return(mem);
  } // END get_memory_for_host()



/*
 * set_hostlist()
 */

void req::set_hostlist(

  const char *hostlist)

  {
  this->hostlist = hostlist;
  } // END set_hostlist()
    
void req::set_memory(
    
  unsigned long mem)

  {
  this->mem = mem;
  }

void req::set_execution_slots(
    
  int execution_slots)

  {
  this->execution_slots = execution_slots;
  }

void req::set_task_count(
    
  int task_count)

  {
  this->task_count = task_count;
  }


