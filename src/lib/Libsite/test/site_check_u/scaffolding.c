#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "server.h" /* server */
#include "pbs_nodes.h" /* pbs_node */
#include "pbs_job.h" /* job */

char *server_host;
struct server server;

char *pbs_o_host = "PBS_O_HOST";
char *msg_orighost = "Job missing PBS_O_HOST value";

char *get_variable(job *pjob, char *variable) 
  { 
  fprintf(stderr, "The call to get_variable needs to be mocked!!\n");
  exit(1);
  }


struct pbsnode *find_nodebyname( char *nodename) 
  { 
  fprintf(stderr, "The call to find_nodebyname needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, char *objname, char *text) 
  { 
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }
