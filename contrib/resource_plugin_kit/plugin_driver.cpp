
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "trq_plugin_api.h"

// plugin_driver.cpp
//
// Can be built against the plugin library for testing purposes. It will echo
// the output of the plugin library so that it may be manually checked for 
// correctness.
//


void test_usage_information()

  {
  std::map<std::string, std::string> usage_information;
  std::string                        jid("1.roshar");
  std::set<pid_t>                    job_pids;
  pid_t                              max_pid_id = 0;
  pid_t                              random_pid = 0;
  std::ifstream                      file;

  file.open("/proc/sys/kernel/pid_max");
  if (file.good())
    {
    file >> max_pid_id;
    file.close();
    }

  if (max_pid_id == 0)
    max_pid_id = 32768;

  max_pid_id--;

  struct timeval now;
  gettimeofday(&now, 0);
  srand(now.tv_usec);

  do
    {
    random_pid = rand() % max_pid_id + 1; // add 1 to avoid process 0
    } while (kill(random_pid, 0) != 0);

  job_pids.insert(random_pid);
  report_job_resources(jid, job_pids, usage_information);

  printf("Your plugin reported the following for pid %d:\n", random_pid);
  for (std::map<std::string, std::string>::iterator it = usage_information.begin();
       it != usage_information.end();
       it++)
    printf("%s = %s\n", it->first.c_str(), it->second.c_str());
  } // END test_usage_information()



void test_node_info_plugin()

  {
  std::map<std::string, unsigned int> gres_map;
  std::map<std::string, double>       gmetric_map;
  std::map<std::string, std::string>  varattr_map;
  std::set<std::string>               features;

  report_node_generic_resources(gres_map);
  report_node_generic_metrics(gmetric_map);
  report_node_varattrs(varattr_map);
  report_node_features(features);

  printf("Your plugin reports the following for this host:\n");

  printf("\tGRES:\n");

  for (std::map<std::string, unsigned int>::iterator it = gres_map.begin();
       it != gres_map.end();
       it++)
    printf("\t\t%s = %u\n", it->first.c_str(), it->second);

  printf("\n\tGMETRICS:\n");

  for (std::map<std::string, double>::iterator it = gmetric_map.begin();
       it != gmetric_map.end();
       it++)
    printf("\t\t%s = %.2f\n", it->first.c_str(), it->second);

  printf("\n\tVARATTRS:\n");

  for (std::map<std::string, std::string>::iterator it = varattr_map.begin();
       it != varattr_map.end();
       it++)
    printf("\t\t%s = %s\n", it->first.c_str(), it->second.c_str());

  printf("\n\tFEATURES:");

  for (std::set<std::string>::iterator it = features.begin(); it != features.end(); it++)
    printf(" %s", it->c_str());

  printf("\n");
  } // END test_node_info_plugin()



int main(
    
  int   argc,
  char *argv[])

  {
  test_usage_information();

  test_node_info_plugin();
  } // END main()

