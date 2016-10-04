#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <list>

#include "list_link.h"
#include "pbs_job.h"
#include "u_tree.h"
#include "log.h"
#include "json/json.h"

#define LOG_BUF_SIZE        16384
#define MAXLINE            1024

int encode_used_ctr;
int encode_flagged_attrs_ctr;
int LOGLEVEL = 10;
int            mom_server_count = 0;
time_t time_now;
tlist_head mom_varattrs; /* variable attributes */
char           *path_log;
long     *log_event_mask;
int   cphosts_num = 0;
long MaxConnectTimeout = 5000000; /* in microseconds */
int    internal_state = 0;
AvlTree okclients = NULL;
tlist_head svr_alljobs; /* all jobs under MOM's control */
char log_buffer[LOG_BUF_SIZE];
int MOMJobDirStickySet = FALSE;
char PBSNodeMsgBuf[MAXLINE];
std::list<job *>    alljobs_list;

void free_pwnam(struct passwd *pwdp, char *buf)
  {}

void encode_used(

  job               *pjob,   /* I */
  int                perm,   /* I */
  Json::Value       *list,   /* O */
  tlist_head        *phead)  /* O */

  {
  }

void encode_flagged_attrs(

  job               *pjob,   /* I */
  int                perm,   /* I */
  Json::Value       *list,   /* O */
  tlist_head        *phead)  /* O */

  {
  }

unsigned long mom_checkpoint_set_checkpoint_run_exe_name(
    
  const char *value)  /* I */
  
  {
  return(0);
  }

unsigned long mom_checkpoint_set_restart_script(
    
  const char *value)  /* I */

  {
  return(0);
  }

int mom_server_add(

  const char *value)

  {
  return(0);
  }

int chk_file_sec(

  const char *path,   /* path to check */
  int   isdir,   /* 1 = path is directory, 0 = file */
  int   sticky,   /* allow write on directory if sticky set */
  int   disallow, /* perm bits to disallow */
  int   fullpath, /* recursively check full path */
  char *SEMsg)    /* O (optional,minsize=1024) */

  {
  return(0);
  }

void log_record(

  int         eventtype,  /* I */
  int         objclass,   /* I */
  const char *objname,    /* I */
  const char *text)       /* I */

  {
  }

char *csv_nth(const char *csv_str, int n)

  {
  return(NULL);
  }

char *pbs_get_server_list(void)

  {
  return(NULL);
  }

void append_link(

  tlist_head *head, /* ptr to head of list */
  list_link  *new_link,  /* ptr to new entry */
  void       *pobj) /* ptr to object to link in */

  {
  }

int log_init(

  const char *suffix,    /* I (optional) */
  const char *hostname)  /* I (optional) */

  {
  return(0);
  }

void rmnl(char *str) {}

void *get_next(

  list_link  pl,   /* I */
  char     *file, /* I */
  int      line) /* I */

  {
  return(NULL);
  }

ssize_t read_ac_socket(

  int     fd,
  void   *buf,
  ssize_t count)

  {
  return(0);
  }

int csv_length(const char *csv_str)

  {
  return(0);
  }

const char *getuname(void)

  {
  return(NULL);
  }

bool am_i_mother_superior(job const &pjob)
  {
  return(true);
  }

void DIS_tcp_settimeout(long timeout) {}

struct passwd *getpwnam_ext( 

  char **user_buf,
  char *user_name) /* I */
  
  {
  return(NULL);
  }

AvlTree AVL_insert( u_long key, uint16_t port, struct pbsnode *node, AvlTree tree )
  
  {
  return(NULL);
  }

unsigned long mom_checkpoint_set_checkpoint_interval(const char *value)  /* I */

  {
  return(0);
  }

char *skipwhite(

  char *str)
  
  {
  return(NULL);
  }

unsigned long mom_checkpoint_set_checkpoint_script(
    
  const char *value)  /* I */

  {
  return(0);
  }

void memcheck(const char *buf) {}

void log_err(int errnum, const char *routine, const char *text) {}

char *conf_res(char *resline, struct rm_attribute *attr)
  {
  if ((resline != NULL) && (*resline == '!'))
    return(strdup("scriptoutput"));

  return(resline);
  }
