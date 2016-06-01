#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "server.h" /* server */
#include "pbs_nodes.h" /* pbs_node */
#include "pbs_job.h" /* job */

char *server_host;
struct server server;
int    LOGLEVEL = 10;

const char *pbs_o_host = "PBS_O_HOST";
const char *msg_orighost = "Job missing PBS_O_HOST value";
bool        exists = true;

char *get_variable(job *pjob, const char *variable)
  { 
  fprintf(stderr, "The call to get_variable needs to be mocked!!\n");
  exit(1);
  }


struct pbsnode *find_nodebyname(const char *nodename)
  { 
  fprintf(stderr, "The call to find_nodebyname needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text) 
  { 
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }


