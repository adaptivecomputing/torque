#include <stdlib.h>
#include <string.h>

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

const int ALL_EXECUTION_SLOTS = -1;


req::req() : execution_slots(1), mem(0), swap(0), disk(0),
             placement_type(PLACE_NO_PREFERENCE), task_count(1), socket(0), numa_chip(0),
             thread_usage_policy(ALLOW_THREADS), gpus(0), mics(0),
             pack(true), index(0), thread_usage_str("allow threads"), single_job_access(false)

  {
  }
    
req::req(
    
  const req &other) : execution_slots(other.execution_slots), mem(other.mem), swap(other.swap),
                      disk(other.disk), gres(other.gres), os(other.os), arch(other.arch),
                      node_access_policy(other.node_access_policy), placement_type(other.placement_type),
                      task_count(other.task_count), hostlist(other.hostlist), socket(other.socket),
                      numa_chip(other.numa_chip), thread_usage_policy(other.thread_usage_policy),
                      gpus(other.gpus), mics(other.mics), pack(other.pack), index(other.index),
                      thread_usage_str(other.thread_usage_str), placement_str(other.placement_str),
                      features(other.features), single_job_access(other.single_job_access),
                      maxtpn(other.maxtpn), gpu_mode(other.gpu_mode), req_attr(other.req_attr)

  {
  }



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

  if (equals != NULL)
    {
    *equals = '\0';
    numeric_value = equals + 1;
    }

  if (!strcmp(work_str, "node"))
    {
    this->placement_type = PLACE_NODE;
    this->placement_str = "place node";
    }
  else if (!strcmp(work_str, "socket"))
    {
    if (numeric_value != NULL)
      {
      this->socket = strtol(numeric_value, NULL, 10);
      }
      
    this->placement_type = PLACE_SOCKET;
    this->placement_str = "place socket";
    }
  else if (!strcmp(work_str, "numachip"))
    {
    if (numeric_value != NULL)
      this->numa_chip = strtol(numeric_value, NULL, 10);
      
    this->placement_type = PLACE_NUMA_CHIP;
    this->placement_str = "place numa";
    }
  else if (!strcmp(work_str, "core"))
    {
    this->thread_usage_policy = USE_CORES;
    this->thread_usage_str = "use cores";

    if (numeric_value != NULL)
      this->execution_slots = strtol(numeric_value, NULL, 10);
    }
  else if (!strcmp(work_str, "thread"))
    {
    this->thread_usage_policy = USE_THREADS;
    this->thread_usage_str = "use threads";
    
    if (numeric_value != NULL)
      this->execution_slots = strtol(numeric_value, NULL, 10);
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

long long read_mem_value(

  const char *value)

  {
  char      *suffix;
  long long  memval = strtoll((char *)value, &suffix, 10);
  
  switch (*suffix)
    {
    case 't':

      memval *= 1024;
      // fall through

    case 'g':

      memval *= 1024;
      // fall through

    case 'm':

      memval *= 1024;

      break;
    }

  return(memval);
  } // END read_mem_value()



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
  if (!strcmp(name, "lprocs"))
    {
    if (value[0] == 'a')
      this->execution_slots = ALL_EXECUTION_SLOTS;
    else
      this->execution_slots = strtol(value, NULL, 10);
    }
  else if (!strcmp(name, "memory"))
    this->mem = read_mem_value(value);
  else if (!strcmp(name, "gpus"))
    this->gpus = strtol(value, NULL, 10);
  else if (!strcmp(name, "mics"))
    this->mics = strtol(value, NULL, 10);
  else if (!strcmp(name, "place"))
    return(set_place_value(value));
  else if (!strcmp(name, "maxtpn"))
    this->maxtpn = strtol(value, NULL, 10);
  else if (!strcmp(name, "gres"))
    this->gres = value;
  else if (!strcmp(name, "feature"))
    this->features = value;
  else if (!strcmp(name, "disk"))
    this->disk = read_mem_value(value);
  else if (!strcmp(name, "opsys"))
    this->os = value;
  else if (!strcmp(name, "mode"))
    this->gpu_mode = value;
  else if (!strcmp(name, "reqattr"))
    this->req_attr = value;
  else
    return(PBSE_BAD_PARAMETER);

  return(PBSE_NONE);
  } // END set_name_value_pair() 



/*
 * set_attribute()
 *
 * Sets an attribute that doesn't require a value. It has to be
 * in this format:
 * 
 * [:{usecores|usethreads|allowthreads|usefastcores}][:pack]
 *
 * @param str - the value in the format specified above.
 * @return PBSE_NONE if correct, PBSE_BAD_PARAMETER otherwise.
 */
int req::set_attribute(

  const char *str)

  {
  if (!strcmp(str, "usecores"))
    {
    this->thread_usage_policy = USE_CORES;
    this->thread_usage_str = "use cores";
    }
  else if (!strcmp(str, "usethreads"))
    {
    this->thread_usage_policy = USE_THREADS;
    this->thread_usage_str = "use threads";
    }
  else if (!strcmp(str, "allowthreads"))
    {
    this->thread_usage_policy = ALLOW_THREADS;
    this->thread_usage_str = "allow threads";
    }
  else if (!strcmp(str, "usefastcores"))
    {
    this->thread_usage_policy = USE_FAST_CORES;
    this->thread_usage_str = "use fast cores";
    }
  else if (!strcmp(str, "pack"))
    {
    this->pack = true;
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
 * set_from_submission_string()
 *
 * initializes req from a string in the format:
 *
 * tasks=#[:lprocs=#|all][:memory=#][:place={node|socket|numachip|core|thread}[=#]]
 * [:{usecores|usethreads|allowthreads|usefastcores}][:pack][:maxtpn=#]
 * [:gpus=#][:mics=#][:gres=xxx[=#]][:feature=xxx]
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

  this->task_count = strtol(submission_str, &current, 10);

  if (*current == ':')
    current++;

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
    
  const std::string &resource_request) : execution_slots(1), mem(0), swap(0),
                                         disk(0), placement_type(PLACE_NO_PREFERENCE),
                                         task_count(1), socket(0), numa_chip(0),
                                         thread_usage_policy(ALLOW_THREADS), gpus(0), mics(0),
                                         index(-1)

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
  this->placement_type = other.placement_type;
  this->req_attr = other.req_attr;
  this->task_count = other.task_count;
  this->pack = other.pack;
  this->single_job_access = other.single_job_access;
  this->index = other.index;
  this->hostlist = other.hostlist;

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

  if (this->socket != 0)
    {
    snprintf(buf, sizeof(buf), "      sockets: %d\n", this->socket);
    str += buf;
    }

  if (this->numa_chip != 0)
    {
    snprintf(buf, sizeof(buf), "      numa chips: %d\n", this->numa_chip);
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

  if (this->placement_type != PLACE_NO_PREFERENCE)
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
 * [sockets: <sockets>]
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

  if (!strncmp(current, "sockets", 7))
    {
    current += 9; // move past 'sockets: '
    this->socket = strtol(current, &current, 10);
    
    move_past_whitespace(&current);
    }

  if (!strncmp(current, "numa chips", 10))
    {
    current += 12; // move past 'numa chips: '
    this->numa_chip = strtol(current, &current, 10);

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

    if (this->thread_usage_str == "use cores")
      this->thread_usage_policy = USE_CORES;
    else if (this->thread_usage_str == "use threads")
      this->thread_usage_policy = USE_THREADS;
    else if (this->thread_usage_str == "allow threads")
      this->thread_usage_policy = ALLOW_THREADS;
    else
      this->thread_usage_policy = USE_FAST_CORES;
    }

  if (!strncmp(current, "placement type", 14))
    {
    current += 16; // move past 'placement type: '
    
    this->placement_str = capture_until_newline_and_advance(&current);

    if (this->placement_str == "place socket")
      this->placement_type = PLACE_SOCKET;
    else if (this->placement_str == "place numa")
      this->placement_type = PLACE_NUMA_CHIP;
    else
      this->placement_type = PLACE_NODE;

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
    *current = '\0';
    current++;
    this->hostlist = tmp;

    move_past_whitespace(&current);
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

int req::getPlacementTypeInt() const

  {
  return(this->placement_type);
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
