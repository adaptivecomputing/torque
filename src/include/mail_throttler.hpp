
#ifndef MAIL_THROTTLER_HPP
#define MAIL_THROTTLER_HPP

#include <map>
#include <string>
#include <vector>
#include <pthread.h>

#include "pbs_job.h"
#include "resource.h"

class mail_info
  {
  public:
  std::string           mailto;
  std::string           exec_host;
  std::string           jobid;
  std::string           jobname;
  std::string           text; // additional optional text
  std::string           errFile;
  std::string           outFile;
  int                   mail_point;
  std::string           queue_name;
  std::string           owner;
  std::string           working_directory;
  std::vector<resource> resources_requested;
  std::vector<resource> resources_used;

  mail_info();
  mail_info(job *pjob);
  mail_info(const mail_info &other);
  mail_info &operator =(const mail_info &other);
  };



class mail_throttler
  {
  std::map<std::string, std::vector<mail_info> > pending;
  pthread_mutex_t                                mt_mutex;

  public:
    mail_throttler();
    int  get_email_list(const std::string &addressee, std::vector<mail_info> &list);
    bool add_email_entry(mail_info &mi);
  };

#endif

