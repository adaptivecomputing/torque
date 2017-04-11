#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "server.h" /* server */
#include "pbs_nodes.h" /* pbs_node */
#include "pbs_job.h" /* job */

char *server_host;
struct server server;
int    LOGLEVEL = 10;

const char  *pbs_o_host = "PBS_O_HOST";
const char  *msg_orighost = "Job missing PBS_O_HOST value";
bool         exists = true;
std::string  long_name("roshar.cosmere");
std::string  short_name("threnody");
char        *var = NULL;
acl_special  limited_acls;

char *get_variable(job *pjob, const char *variable)
  { 
  return(var);
  }


struct pbsnode *find_nodebyname(const char *nodename)
  { 
  fprintf(stderr, "The call to find_nodebyname needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text) 
  { 
  }

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }


bool acl_special::is_authorized(const std::string &host, const std::string &user) const

  {
  return(false);
  }

int get_svr_attr_l(

  int   index,
  long *l)

  {
  *l = 1;
  return(0);
  }

int get_svr_attr_arst(

  int  index,
  struct array_strings **arst_ptr)

  {
  size_t need = sizeof(struct array_strings) + 3 * sizeof(char *);
  struct array_strings *arst = (struct array_strings *)calloc(1, need);
  char *buf = (char *)calloc(100, sizeof(char));
  strcpy(buf, "napali");
  strcpy(buf + 7, "waimea");
  arst->as_buf = buf;
  arst->as_usedptr = 2;
  arst->as_bufsize = 100;
  arst->as_next = arst->as_buf + 14;
  arst->as_string[0] = arst->as_buf;
  arst->as_string[1] = arst->as_buf + 7;

  *arst_ptr = arst;

  return(0);
  }

bool node_exists(const char *node_name)
  {
  if (exists == false)
    {
    if (long_name == node_name)
      return(true);
    else if (short_name == node_name)
      return(true);
    }

  return(exists);
  }

acl_special::acl_special() {}
