#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>

#include "pbs_nodes.h"

pbsnode::pbsnode() {}
pbsnode::~pbsnode() {}

const char *pbsnode::get_name() const
  
  {
  return(this->nd_name.c_str());
  }

void pbsnode::change_name(

  const char *name)

  {
  this->nd_name = name;
  }

pbsnode *find_nodebyname(const char *node_name)
  {
  static pbsnode pnode;

  if ((!strcmp(node_name, "nalthis")) ||
      (!strcmp(node_name, "nalthis2")) ||
      (!strcmp(node_name, "roshar")) ||
      (!strcmp(node_name, "roshar2")) ||
      (!strcmp(node_name, "scadrial")) ||
      (!strcmp(node_name, "scadrial2")) ||
      (!strcmp(node_name, "elantris")) ||
      (!strcmp(node_name, "elantris2")))
    {
    pnode.change_name(node_name);
    return(&pnode);
    }

  return(NULL);
  }

