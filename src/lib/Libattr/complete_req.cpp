
#include <stdio.h>

#include "complete_req.hpp"
#include "pbs_error.h"



complete_req::complete_req() : reqs()
  {
  }



complete_req::complete_req(
    
  const complete_req &other) : reqs(other.reqs)

  {
  }



complete_req &complete_req::operator =(

  const complete_req &other)

  {
  this->reqs = other.reqs;
  }



/*
 * add_req()
 *
 * adds a req to this complete req's list of reqs after setting its index
 *
 * @param r - the req to add
 */

void complete_req::add_req(

  req &r)

  {
  // start indices at 0
  r.set_index(reqs.size());
  reqs.push_back(r);
  }



/*
 * set_from_string()
 *
 * initializes this object from a string exactly
 * matching the output produced by toString()
 *
 * @param obj_string - the string produced by a call to toString()
 */

void complete_req::set_from_string(

  const std::string &obj_string)

  {
  static std::string req_start("req[");
  std::size_t curr_pos = obj_string.find(req_start, 1);
  std::size_t next_req = obj_string.find(req_start, curr_pos + 1);

  while (curr_pos != std::string::npos)
    {
    std::string one_req(obj_string.substr(curr_pos, next_req - curr_pos));
    req r;

    r.set_from_string(one_req);

    add_req(r);

    curr_pos = next_req;

    if (curr_pos != std::string::npos)
      next_req = obj_string.find(req_start, curr_pos + 1);
    }

  } // END set_from_string() 



void complete_req::toString(

  std::string &output) const

  {
  output.clear();

  for (unsigned int i = 0; i < this->reqs.size(); i++)
    {
    if (i > 0)
      output += '\n';

    this->reqs[i].toString(output);
    }
  } // END toString() 



/*
 * req_count()
 *
 * @return - the number of reqs in this complete req class
 */

int complete_req::req_count() const

  {
  return(this->reqs.size());
  } // END req_count()



/*
 * set_value()
 *
 * Sets a value for the req at the specified index, creating the index
 * if it doesn't already exist
 *
 * @param index - the index of the req to set this particular value
 * @param name - the name of the value to be set
 * @param value - the value to be set
 * @return PBSE_NONE if the name is valid and the index is >= 0, PBSE_BAD_PARAMETER otherwise
 *
 */

int complete_req::set_value(

  int         index,
  const char *name,
  const char *value)

  {
  if (index < 0)
    return(PBSE_BAD_PARAMETER);

  while (this->reqs.size() <= (unsigned int)index)
    {
    req r;
    r.set_index(this->reqs.size());
    this->reqs.push_back(r);
    }

  return(this->reqs[index].set_value(name, value));
  } // END set_value()



/*
 * get_values()
 *
 * Gets the name and value of each attribute set for each req held
 *
 * @param names - the vector where each name is stored
 * @param values - the vector where each value is stored
 */

void complete_req::get_values(

  std::vector<std::string> &names,
  std::vector<std::string> &values) const

  {
  for (unsigned int i = 0; i < this->reqs.size(); i++)
    this->reqs[i].get_values(names, values);
  } // END get_values()
    
/* 
 * get_swap_memory_for_this_host()
 */

unsigned long complete_req::get_swap_memory_for_this_host(const std::string &hostname) const

  {
  unsigned long mem = 0;
  
  for (unsigned int i = 0; i < this->reqs.size(); i++)
    mem += this->reqs[i].get_swap_for_host(hostname);

  return(mem);
  } // END get_memory_for_host()



/* 
 * get_memory_for_this_host()
 */

unsigned long complete_req::get_memory_for_this_host(const std::string &hostname) const

  {
  unsigned long mem = 0;
  
  for (unsigned int i = 0; i < this->reqs.size(); i++)
    mem += this->reqs[i].get_memory_for_host(hostname);

  return(mem);
  } // END get_memory_for_host()
