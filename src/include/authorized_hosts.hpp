
#ifndef __AUTHORIZED_HOSTS_HPP__
#define __AUTHORIZED_HOSTS_HPP__
#include <map>
#include <string>

#include "pbs_nodes.h"


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
