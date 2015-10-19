#include <sys/types.h>
#include <grp.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "utils.h"
#include "restricted_host.hpp"



restricted_host::restricted_host(

  const std::string &host,
  const std::string &user,
  const std::string &group) : hostname(host), users(), groups()

  {
  if (user.size() != 0)
    this->users.insert(user);

  if (group.size() != 0)
    this->groups.insert(group);
  }
  


restricted_host::restricted_host() : hostname(), users(), groups()

  {
  }



bool restricted_host::operator ==(

  const restricted_host &other) const

  {
  return(other.hostname == this->hostname);
  }



void restricted_host::add_user(

  const std::string &user)

  {
  this->users.insert(user);
  }



void restricted_host::add_group(

  const std::string &group)

  {
  this->groups.insert(group);
  }



void restricted_host::remove_user(

  const std::string &user)

  {
  this->users.erase(user);
  }



void restricted_host::remove_group(

  const std::string &group)

  {
  this->groups.erase(group);
  }



bool restricted_host::has_user(

  const std::string &user) const

  {
  return(this->users.find(user) != this->users.end());
  }



bool restricted_host::has_group(

  const std::string &group) const

  {
  return(this->groups.find(group) != this->groups.end());
  }



void restricted_host::clear_users()

  {
  this->users.clear();
  }



void restricted_host::clear_groups()

  {
  this->groups.clear();
  }


bool restricted_host::authorize_by_group(

  const std::string &user) const

  {
  std::set<std::string>::iterator  it;
  struct group                     grp;
  struct group                    *ptr = NULL;
  int                              bufsize;
  char                            *buf;
  bool                             authorized = false;

  memset(&grp, 0, sizeof(grp));
  bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
  buf = (char *)calloc(1, bufsize);

  for (it = this->groups.begin(); it != this->groups.end() && authorized == false; it++)
    {
    int rc = getgrnam_r((*it).c_str(), &grp, buf, bufsize, &ptr);

    while ((rc != 0) &&
           (errno == ERANGE))
      {
      bufsize *= 2;
      free(buf);
      buf = (char *)calloc(1, bufsize);
      rc = getgrnam_r((*it).c_str(), &grp, buf, bufsize, &ptr);
      }

    if ((rc == 0) && 
        (ptr != NULL))
      {
      for (int i = 0; grp.gr_mem[i] != NULL; i++)
        {
        if (!strcmp(grp.gr_mem[i], user.c_str()))
          {
          authorized = true;
          break;
          }
        }
      }
    
    memset(buf, 0, bufsize);
    }

  free(buf);

  return(authorized);
  } // END authorize_by_group()

