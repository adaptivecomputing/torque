#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */ 
#include <vector>
#include <string>

#include "pbs_ifl.h" /* attopl, attrl */
#include "mcom.h" /* MDataFormatEnum */
#include "cmds.h"
#include "allocation.hpp"

#define ISNAMECHAR(x) ( (isgraph(x)) && ((x) != '#') && ( (x) != '@') )

int pbs_errno = PBSE_NONE;
char *pbs_server = NULL;
const char* default_err_msg = "err message for test qstat";

bool connect_success = true;

char *pbs_geterrmsg(int connect)
  {
  return((pbs_errno == PBSE_NONE)? NULL : strdup(default_err_msg));
  }

struct batch_status * pbs_selstat(int c, struct attropl *attrib, char *extend)
  { 
  fprintf(stderr, "The call to pbs_selstat needs to be mocked!!\n");
  return(PBSE_NONE);
  }

struct batch_status * pbs_selstat_err(int c, struct attropl *attrib, char *extend, int *any_failed)
  { 
  *any_failed = 0;
  return(PBSE_NONE);
  }

int TShowAbout_exit(void)
  { 
  fprintf(stderr, "The call to TShowAbout_exit needs to be mocked!!\n");
  return(PBSE_NONE);
  }

struct batch_status *pbs_statserver(int c, struct attrl *attrib, char *extend)
  { 
  fprintf(stderr, "The call to pbs_statserver needs to be mocked!!\n");
  return(PBSE_NONE);
  }

struct batch_status *pbs_statserver_err(int c, struct attrl *attrib, char *extend, int *local_errno)
  { 
  static struct batch_status server_status;
  return(&server_status);
  }

int MXMLSetVal(mxml_t *E, void *V, enum MDataFormatEnum Format)
  { 
  fprintf(stderr, "The call to MXMLSetVal needs to be mocked!!\n");
  return(PBSE_NONE);
  }

int pbs_disconnect(int connect)
  { 
  return(PBSE_NONE);
  }

int parse_destination_id(

  char *destination_in,     /* I */
  char **queue_name_out,    /* O */
  char **server_name_out)   /* O */

  {
  /* FORMAT:  <QUEUE>@<HOST>:<PORT> */

  int i;
  char *c;
  char queue_name[PBS_MAXQUEUENAME+1];
  int q_pos;
  char server_name[MAXSERVERNAME];
  int c_pos;


  /* initialize static data for this parsing call */

  for (i = 0; i < PBS_MAXQUEUENAME + 1; i++) queue_name[i] = '\0';

  q_pos = 0;

  for (i = 0; i < MAXSERVERNAME; i++) server_name[i] = '\0';

  c_pos = 0;

  /* Begin the parse */
  c = destination_in;

  while (isspace(*c)) c++;

  /* Looking for a queue */
  while (*c != '\0')
    {
    if (ISNAMECHAR(*c))
      {
      if (q_pos >= PBS_MAXQUEUENAME) return 1;

      queue_name[q_pos++] = *c;
      }
    else
      break;

    c++;
    }

  /* Looking for a server */
  if (*c == '@')
    {
    c++;

    while (*c != '\0')
      {
      if (ISNAMECHAR(*c))
        {
        if (c_pos >= MAXSERVERNAME) return 1;

        server_name[c_pos++] = *c;
        }
      else
        break;

      c++;
      }

    if (c_pos == 0) return 1;
    }

  if (*c != '\0') return 1;

  /* set char * pointers to static data, to arguments */
  if (queue_name_out != NULL) *queue_name_out = queue_name;

  if (server_name_out != NULL) *server_name_out = server_name;

  return 0;
  }


int locate_job(char *job_id, char *parent_server, char *located_server)
  { 
  fprintf(stderr, "The call to locate_job needs to be mocked!!\n");
  return(PBSE_NONE);
  }

int MXMLToXString(mxml_t *E, char **Buf, int *BufSize, int MaxBufSize, char **Tail, mbool_t IsRootElement)
  { 
  fprintf(stderr, "The call to MXMLToXString needs to be mocked!!\n");
  return(PBSE_NONE);
  }

extern "C"
{
int cnt2server(const char *SpecServer, bool silence)
  { 
  if (connect_success == false)
    return(-1);

  return(10);
  }
}

struct batch_status *pbs_statjob(int c, char *id, struct attrl *attrib, char *extend)
  { 
  fprintf(stderr, "The call to pbs_statjob needs to be mocked!!\n");
  return(PBSE_NONE);
  }

struct batch_status *pbs_statjob_err(int c, char *id, struct attrl *attrib, char *extend, int *local_errno)
  { 
  static struct batch_status job_status;
  *local_errno = PBSE_NONE;
  return(&job_status);
  }

void pbs_statfree(struct batch_status *bsp)
  { 
  return;
  }

int MXMLDestroyE(mxml_t **EP)
  { 
  fprintf(stderr, "The call to MXMLDestroyE needs to be mocked!!\n");
  return(PBSE_NONE);
  }

int MXMLAddE(mxml_t *E, mxml_t *C)
  { 
  fprintf(stderr, "The call to MXMLAddE needs to be mocked!!\n");
  return(PBSE_NONE);
  }

int MXMLCreateE(mxml_t **E, const char *Name)
  { 
  fprintf(stderr, "The call to MXMLCreateE needs to be mocked!!\n");
  return(PBSE_NONE);
  }

int parse_jobid(

  const char *job_id,
  char **arg_seq_number,
  char **arg_parent_server,
  char **arg_current_server)

  {
  int i;
  const char *c;
  char seq_number[PBS_MAXSEQNUM+PBS_MAXJOBARRAYLEN+1];
  int s_pos;
  char parent_server[MAXSERVERNAME];
  int p_pos;
  char current_server[MAXSERVERNAME];
  int c_pos;


  /* initialize static data for this parsing call */

  for (i = 0; i < PBS_MAXSEQNUM + PBS_MAXJOBARRAYLEN + 1; i++) seq_number[i] = '\0';

  s_pos = 0;

  for (i = 0; i < MAXSERVERNAME; i++) parent_server[i] = '\0';

  p_pos = 0;

  for (i = 0; i < MAXSERVERNAME; i++) current_server[i] = '\0';

  c_pos = 0;

  /* Begin the parse */
  c = job_id;

  while (isspace(*c)) c++;

  /* Looking for a seq_number */
  while (*c != '\0')
    {
    /* look for a digit or array brackets */
    if (isdigit(*c) || *c == '[' || *c == ']')
      {
      if (s_pos >= PBS_MAXSEQNUM + PBS_MAXJOBARRAYLEN) return 3;

      seq_number[s_pos++] = *c;
      }
    else
      break;

    c++;
    }

  if (s_pos == 0) return 1;

  /* Looking for a parent_server */
  if (*c == '.')
    {
    c++;

    while (*c != '\0')
      {
      if (ISNAMECHAR(*c))
        {
        if (p_pos >= MAXSERVERNAME) return 3;

        parent_server[p_pos++] = *c;
        }
      else
        break;

      c++;
      }

    if (p_pos == 0) return 1;
    }

  /* Looking for a current_server */
  if (*c == '@')
    {
    c++;

    while (*c != '\0')
      {
      if (ISNAMECHAR(*c))
        {
        if (c_pos >= MAXSERVERNAME) return 3;

        current_server[c_pos++] = *c;
        }
      else
        break;

      c++;
      }

    if (c_pos == 0) return 1;
    }

    if (*c != '\0') return 2;

    /* set char * pointers to static data, to arguments */
    if (arg_seq_number != NULL) *arg_seq_number = seq_number;

    if (arg_parent_server != NULL) *arg_parent_server = parent_server;

    if (arg_current_server != NULL) *arg_current_server = current_server;

    return 0;
    }



int get_server(const char *job_id_in, char *job_id_out, int jobid_size, char *server_out, int server_size)
  { 
  char *seq_number = NULL;
  char *parent_server = NULL;
  char *current_server = NULL;
  int  rc;

  rc = parse_jobid(job_id_in, &seq_number, &parent_server, &current_server);
  if (rc != PBSE_NONE)
    {
    fprintf(stderr, "something is wrong in parse_jobid");
    return(rc);
    }

  if ((current_server != NULL) && (current_server[0] != '\0'))
    {
    sprintf(server_out, "%s", current_server);
    }
  else if ((parent_server != NULL) && (parent_server[0] != '\0'))
    {
    sprintf(server_out, "%s", parent_server);
    }
  else
    {
    server_out[0] = '\0';
    }
  
  return(PBSE_NONE);
  }

void prt_job_err(const char *cmd, int connect, const char *id)
  { 
  fprintf(stderr, "The call to prt_job_err needs to be mocked!!\n");
  return;
  }

struct batch_status *pbs_statque(int c, char *id, struct attrl *attrib, char *extend)
  { 
  fprintf(stderr, "The call to pbs_statque needs to be mocked!!\n");
  return(PBSE_NONE);
  }

struct batch_status *pbs_statque_err(int c, char *id, struct attrl *attrib, char *extend, int *local_errno)
  { 
  static struct batch_status queue_status;


  return(&queue_status);
  }

char *pbs_default(void)
  { 
  fprintf(stderr, "The call to pbs_default needs to be mocked!!\n");
  return(PBSE_NONE);
  }

void initialize_network_info() {}

#ifdef __cplusplus
extern "C"
{
#endif
void set_attr(
  struct attrl **attrib,        /* I */
  const char   *attrib_name,   /* I */
  const char   *attrib_value)  /* I */ 
  {
  }

char *pbs_strerror(int err)
  { 
  return((err == PBSE_NONE)? NULL : strdup(default_err_msg));
  }
#ifdef __cplusplus
}
#endif


struct batch_status * pbs_selstatattr_err(

  int             c,
  struct attropl *attropl,
  struct attrl   *attrib,
  char           *extend,
  int            *local_errno)
  {
  *local_errno = PBSE_NONE;
  return(NULL);
  }

void translate_vector_to_range_string(std::string &out, const std::vector<int> &v) {}

allocation::allocation() {}

void allocation::initialize_from_string(const std::string &init) {}
      
int get_server_and_job_ids(
    
  const char *job_id,
  std::vector<std::string> &id_list,
  std::string &server_name)

  {
  id_list.push_back(job_id);
  return(0);
  }
