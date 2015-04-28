#ifndef __ATTR_REQ_INFO__
#define __ATTR_REQ_INFO__

#include "license_pbs.h"
#include <vector>
#include <string>

class attr_req_info
  {
    /* queue and server resource limits */
    /* QA_ATR_ReqInformationMax */
    int max_lprocs;
    unsigned int max_mem;
    unsigned int max_swap;
    unsigned int max_disk;
    int max_nodes;
    int max_sockets;
    int max_cores;
    int max_numa_chips;
    int max_threads;

    /* QA_ATR_ReqInformationMin */
    int min_lprocs;
    unsigned int min_mem;
    unsigned int min_swap;
    unsigned int min_disk;
    int min_nodes;
    int min_sockets;
    int min_cores;
    int min_numa_chips;
    int min_threads;

    /* QA_ATR_ReqInformationDefault */
    int default_lprocs;
    unsigned int default_mem;
    unsigned int default_swap;
    unsigned int default_disk;
    int default_nodes;
    int default_sockets;
    int default_cores;
    int default_numa_chips;
    int default_threads;

  public:
  attr_req_info& operator=(const attr_req_info& newattr_req_info);
  attr_req_info();
  ~attr_req_info();
  int set_min_limit_value(const char *rescn, const char *val);
  int set_max_limit_value(const char *rescn, const char *val);
  int set_default_limit_value(const char *rescn, const char *val);
  int get_signed_min_limit_value(const char *rescn, int& value);
  int get_signed_max_limit_value(const char *rescn, int& value);
  int get_signed_default_limit_value(const char *rescn, int& value);
  unsigned int get_unsigned_min_limit_value(const char *rescn, unsigned int& value);
  unsigned int get_unsigned_max_limit_value(const char *rescn, unsigned int& value);
  unsigned int get_unsigned_default_limit_value(const char *rescn, unsigned int& value);
  int get_min_values(std::vector<std::string>& names, std::vector<std::string>& values);
  int get_max_values(std::vector<std::string>& names, std::vector<std::string>& values);
  int get_default_values(std::vector<std::string>& names, std::vector<std::string>& values);
  int check_max_values(std::vector<std::string>& names, std::vector<std::string>& values);
  int check_min_values(std::vector<std::string>& names, std::vector<std::string>& values);
  int add_default_values(std::vector<std::string>&, std::vector<std::string>&, std::vector<std::string>&, std::vector<std::string>&);
  };

#endif /* ifndef __ATTR_REQ_INFO__ */
