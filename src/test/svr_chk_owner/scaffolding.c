#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "pbs_job.h" /* job */
#include "attribute.h" /* pbs_attribute */
#include "batch_request.h" /* batch_request */
#include "server.h" /* server */

const char *pbs_o_host = "PBS_O_HOST";
const char *msg_permlog = "Unauthorized Request, request type: %d, Object: %s, Name: %s, request from: %s@%s";
const char *PJobState[] = {"hi", "hello"};
struct server server;
char *server_host;
char server_localhost[PBS_MAXHOSTNAME + 1];
int LOGLEVEL;


char *site_map_user(char *uname, char *host)
  {
  fprintf(stderr, "The call to site_map_user to be mocked!!\n");
  exit(1);
  }

char *get_variable(job *pjob, const char *variable)
  {
  fprintf(stderr, "The call to get_variable to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

int acl_check(pbs_attribute *pattr, char *name, int type)
  {
  fprintf(stderr, "The call to acl_check to be mocked!!\n");
  exit(1);
  }

int site_allow_u(char *user, char *host)
  {
  fprintf(stderr, "The call to site_allow_u to be mocked!!\n");
  exit(1);
  }

void get_jobowner(char *from, char *to)
  {
  fprintf(stderr, "The call to get_jobowner to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int get_svr_attr_arst(int index, struct array_strings **arst)
  {
  return(0);
  }

int acl_check_my_array_string(struct array_strings *pas, char *name, int type)
  {
  return(0);
  }

char *get_cached_fullhostname(

  char               *hostname,
  struct sockaddr_in *sai)

  {
  return(NULL);
  }

int insert_addr_name_info(
    
  char               *hostname,
  char               *full_hostname,
  struct sockaddr_in *sai)

  {
  return(0);
  }
  
struct sockaddr_in *get_cached_addrinfo(
    
  char               *hostname)
  {
  return(NULL);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

int pbs_getaddrinfo(const char *pNode, struct addrinfo *pHints, struct addrinfo **ppAddrInfoOut)
  {
  return(0);
  }

struct sockaddr_in *get_cached_addrinfo(const char *hostname)
  {
  return(NULL);
  }

struct addrinfo * insert_addr_name_info(struct addrinfo *pAddrInfo, const char *host)
  {
  return(NULL);
  }

pbs_net_t get_hostaddr(

  int  *local_errno, /* O */    
  const char *hostname)    /* I */

  {
  return(0);
  }

char *get_cached_fullhostname(const char *hostname,const struct sockaddr_in *sai)
  {
  return(NULL);
  }
