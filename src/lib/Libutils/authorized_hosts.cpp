
#include <stdio.h>

#include "authorized_hosts.hpp"
#include "log.h"



authorized_hosts::authorized_hosts() : auth_map()

  {
  pthread_mutex_init(&this->auth_mutex, NULL);
  } // END constructor



/*
 * add_authorized_address()
 *
 * Adds an authorized host with it's information
 * @param addr - the address of the host
 * @param port - the port of the host
 * @param host - the hostname of the host
 */

void authorized_hosts::add_authorized_address(
    
  unsigned long      addr, 
  unsigned short     port,
  const std::string &host)

  {
  pthread_mutex_lock(&this->auth_mutex);

  if (this->auth_map.find(addr) == this->auth_map.end())
    {
    std::map<unsigned short, std::string> port_host_map;
    port_host_map[port] = host;
    this->auth_map[addr] = port_host_map;
    }
  else
    this->auth_map[addr][port] = host;
  pthread_mutex_unlock(&this->auth_mutex);
  } // END add_authorized_address()



/*
 * clear()
 *
 * Clears the current authorized host information
 */

void authorized_hosts::clear()

  {
  pthread_mutex_lock(&this->auth_mutex);
  this->auth_map.clear();
  pthread_mutex_unlock(&this->auth_mutex);
  }



/*
 * is_authorized()
 *
 * Determines if the address in question is authorized.
 * @param addr - the addr that we want to know if is authorized or not
 */

bool authorized_hosts::is_authorized(
    
  unsigned long addr)

  {
  bool legit = false;

  pthread_mutex_lock(&this->auth_mutex);
  if (this->auth_map.find(addr) != this->auth_map.end())
    legit = true;
  pthread_mutex_unlock(&this->auth_mutex);

  return(legit);
  } // END is_authorized()



/*
 * is_authorized()
 *
 * Determines if the address and port in question is authorized.
 * @param addr - the addr that we want to know if is authorized or not
 * @param port - the port that we want to know if is authorized or not
 */

bool authorized_hosts::is_authorized(
    
  unsigned long  addr,
  unsigned short port)

  {
  bool legit = false;

  pthread_mutex_lock(&this->auth_mutex);
  if (this->auth_map.find(addr) != this->auth_map.end())
    {
    if (this->auth_map[addr].find(port) != this->auth_map[addr].end())
      legit = true;
    }
  pthread_mutex_unlock(&this->auth_mutex);

  return(legit);
  } // END is_authorized()



/*
 * get_authorized_node()
 *
 * Retrieves the node associated with the addr, if there is an authorized node associated with
 * this address.
 *
 * @param addr - the address in question
 * @return the node associated with the address
 */

pbsnode *authorized_hosts::get_authorized_node(
    
  unsigned long addr)

  {
  std::string  hostname;
  pbsnode     *pnode = NULL;

  pthread_mutex_lock(&this->auth_mutex);
  if (this->auth_map.find(addr) != this->auth_map.end())
    hostname = this->auth_map[addr].begin()->second;
  pthread_mutex_unlock(&this->auth_mutex);

  if (hostname.size() != 0)
    pnode = find_nodebyname(hostname.c_str());

  return(pnode);
  } // END get_authorized_node()



/*
 * get_authorized_node()
 *
 * Retrieves the node associated with the addr and port, if there is an authorized node associated
 * with this address.
 *
 * @param addr - the address in question
 * @param port - the port in question
 * @return the node associated with the address
 */

pbsnode *authorized_hosts::get_authorized_node(
    
  unsigned long addr,
  unsigned short port)

  {
  std::string  hostname;
  pbsnode     *pnode = NULL;

  pthread_mutex_lock(&this->auth_mutex);
  if (this->auth_map.find(addr) != this->auth_map.end())
    {
    if (this->auth_map[addr].find(port) != this->auth_map[addr].end())
      hostname = this->auth_map[addr][port];
    }
  pthread_mutex_unlock(&this->auth_mutex);

  if (hostname.size() != 0)
    pnode = find_nodebyname(hostname.c_str());

  return(pnode);
  } // END get_authorized_node()



/*
 * list_authorized_hosts()
 *
 * Outputs the authorized hosts into a list
 */

void authorized_hosts::list_authorized_hosts(
    
  std::string &output)

  {
  char buf[MAXLINE];

  output.clear();

  pthread_mutex_lock(&this->auth_mutex);

  for (std::map<unsigned long, std::map<unsigned short, std::string> >::iterator it = this->auth_map.begin();
       it != this->auth_map.end();
       it++)
    {
    for (std::map<unsigned short, std::string>::iterator map_it = it->second.begin();
         map_it != it->second.end();
         map_it++)
      {
      sprintf(buf, "%ld.%ld.%ld.%ld:%d",
        (it->first & 0xFF000000) >> 24,
        (it->first & 0x00FF0000) >> 16,
        (it->first & 0x0000FF00) >> 8,
        (it->first & 0x000000FF),
        map_it->first);

      if (output.size() != 0)
        output += ",";

      output += buf;
      }
    }
  pthread_mutex_unlock(&this->auth_mutex);
  } // END list_authorized_hosts()



/*
 * remove_address()
 * Removes the address and port from the authorized hosts
 *
 * @param host - the host that should be removed
 * @param port - the accompanying port
 * @return true if the node was removed, false if it wasn't present and therefore wasn't removed
 */

bool authorized_hosts::remove_address(

  unsigned long  addr,
  unsigned short port)

  {
  bool removed = false;

  pthread_mutex_lock(&this->auth_mutex);
  std::map<unsigned long, std::map<unsigned short, std::string> >::iterator it = this->auth_map.find(addr);

  std::map<unsigned short, std::string>::iterator map_it = it->second.find(port);
  if (map_it != it->second.end())
    {
    removed = true;
    it->second.erase(map_it);

    // If there are no authorized ports left, remove this address completely
    if (it->second.size() == 0)
      {
      this->auth_map.erase(it);
      }
    }
  
  pthread_mutex_unlock(&this->auth_mutex);

  return(removed);
  } // END remove_address()

