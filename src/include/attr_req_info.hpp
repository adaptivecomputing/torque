#ifndef __ATTR_REQ_INFO__
#define __ATTR_REQ_INFO__

#include "license_pbs.h"
#include <vector>
#include <string>
#include "attribute.h"

class attr_req_info
  {
    /* queue and server resource limits */
    /* QA_ATR_ReqInformationMax */
    int max_lprocs;
    unsigned long max_mem;
    unsigned long max_swap;
    unsigned long max_disk;
    int max_nodes;
    int max_sockets;
    int max_cores;
    int max_numa_nodes;
    int max_threads;
    struct size_value max_mem_value;
    struct size_value max_swap_value;
    struct size_value max_disk_value;

    /* QA_ATR_ReqInformationMin */
    int min_lprocs;
    unsigned long min_mem;
    unsigned long min_swap;
    unsigned long min_disk;
    int min_nodes;
    int min_sockets;
    int min_cores;
    int min_numa_nodes;
    int min_threads;
    struct size_value min_mem_value;
    struct size_value min_swap_value;
    struct size_value min_disk_value;

    /* QA_ATR_ReqInformationDefault */
    int default_lprocs;
    unsigned long default_mem;
    unsigned long default_swap;
    unsigned long default_disk;
    int default_nodes;
    int default_sockets;
    int default_numa_nodes;
    struct size_value default_mem_value;
    struct size_value default_swap_value;
    struct size_value default_disk_value;

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
  unsigned int get_unsigned_min_limit_value(const char *rescn, unsigned long& value);
  unsigned int get_unsigned_max_limit_value(const char *rescn, unsigned long& value);
  unsigned int get_unsigned_default_limit_value(const char *rescn, unsigned long& value);
  int get_min_values(std::vector<std::string>& names, std::vector<std::string>& values);
  int get_max_values(std::vector<std::string>& names, std::vector<std::string>& values);
  int get_default_values(std::vector<std::string>& names, std::vector<std::string>& values);
  int check_max_values(std::vector<std::string>& names, std::vector<std::string>& values);
  int check_min_values(std::vector<std::string>& names, std::vector<std::string>& values);
  int add_default_values(std::vector<std::string>&, std::vector<std::string>&, std::vector<std::string>&, std::vector<std::string>&);
  };

#endif /* ifndef __ATTR_REQ_INFO__ */
