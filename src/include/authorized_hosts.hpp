
#ifndef __AUTHORIZED_HOSTS_HPP__
#define __AUTHORIZED_HOSTS_HPP__
#include <map>
#include <string>

#include "pbs_nodes.h"

class authorized_host_info
  {
  friend class authorized_hosts;

  std::string              hostname;
  std::set<unsigned short> ports;

  public:
    authorized_host_info() : hostname(), ports() {}
    authorized_host_info(const std::string &host, unsigned short port) : hostname(host)
      {
      ports.insert(port);
      }
    
    authorized_host_info(
      const authorized_host_info &other) : hostname(other.hostname), ports(other.ports) {}
  };


class authorized_hosts
  {
  pthread_mutex_t                               auth_mutex;
  // Map key is the ip address, value is the host information
  std::map<unsigned long, std::map<unsigned short, std::string> > auth_map;

  public:
    authorized_hosts();

    void add_authorized_address(unsigned long addr, unsigned short port, const std::string &host);
    void clear();

    bool     is_authorized(unsigned long addr);
    bool     is_authorized(unsigned long addr, unsigned short port);
    pbsnode *get_authorized_node(unsigned long addr);
    pbsnode *get_authorized_node(unsigned long addr, unsigned short port);
    void     list_authorized_hosts(std::string &output);
    bool     remove_address(unsigned long addr, unsigned short port);
  };

extern authorized_hosts auth_hosts;

#endif
