#include <grp.h>

#include "acl_special.hpp"
#include "restricted_host.hpp"

acl_special::acl_special() : ug_acls()

  {
  }



/*
 * parse_qmgr_input()
 *
 * Receives input in the form ident@host and parses it
 * @param qmgr_input (I) - the raw input
 * @param ident (O) - where the identity is stored
 * @param hostname (O) - where the hostname is stored
 */

void acl_special::parse_qmgr_input(

  const std::string &qmgr_input,
  std::string       &ident,
  std::string       &hostname) const

  {
  std::size_t pos = qmgr_input.find("@");

  if (pos != std::string::npos)
    {
    ident = qmgr_input.substr(0, pos);
    hostname = qmgr_input.substr(pos + 1);
    }
  else
    {
    ident.clear();
    hostname.clear();
    }

  } // END parse_qmgr_input()



void acl_special::add_user_configuration(

  const std::string &qmgr_input)

  {
  std::string user;
  std::string host;

  parse_qmgr_input(qmgr_input, user, host);

  std::map<std::string, restricted_host>::iterator it = ug_acls.find(host);

  if (it == ug_acls.end())
    {
    // Add the new restricted host
    restricted_host rh(host, user, "");
    ug_acls[host] = rh;
    }
  else
    ug_acls[host].add_user(user);

  } // END add_user_configuration()



void acl_special::add_group_configuration(

  const std::string &qmgr_input)

  {
  std::string group;
  std::string host;

  parse_qmgr_input(qmgr_input, group, host);

  std::map<std::string, restricted_host>::iterator it = ug_acls.find(host);

  if (it == ug_acls.end())
    {
    // Add the new restricted host
    restricted_host rh(host, "", group);
    ug_acls[host] = rh;
    }
  else
    ug_acls[host].add_group(group);

  } // END add_group_configuration()



void acl_special::remove_user_configuration(

  const std::string &qmgr_input)

  {
  std::string user;
  std::string host;

  parse_qmgr_input(qmgr_input, user, host);

  std::map<std::string, restricted_host>::iterator it = ug_acls.find(host);

  if (it != ug_acls.end())
    {
    ug_acls[host].remove_user(user);
    }
  } // END remove_user_configuration()



void acl_special::remove_group_configuration(

  const std::string &qmgr_input)

  {
  std::string group;
  std::string host;

  parse_qmgr_input(qmgr_input, group, host);

  std::map<std::string, restricted_host>::iterator it = ug_acls.find(host);

  if (it != ug_acls.end())
    {
    ug_acls[host].remove_group(group);
    }
  } // END remove_group_configuration()



void acl_special::clear_users()

  {
  std::map<std::string, restricted_host>::iterator it;

  for (it = this->ug_acls.begin(); it != this->ug_acls.end(); it++)
    it->second.clear_users();
  }



void acl_special::clear_groups()

  {
  std::map<std::string, restricted_host>::iterator it;

  for (it = this->ug_acls.begin(); it != this->ug_acls.end(); it++)
    it->second.clear_groups();
  }



bool acl_special::is_authorized(

  const std::string &host,
  const std::string &user) const

  {
  std::map<std::string, restricted_host>::const_iterator it = this->ug_acls.find(host);
  bool                                                   authorized = false;

  if (it != this->ug_acls.end())
    {
    if (it->second.has_user(user) == true)
      authorized = true;
    else
      {
      authorized = it->second.authorize_by_group(user);
      }
    }

  return(authorized);
  } // END is_authorized()


