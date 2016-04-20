#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "server.h" /* server */
#include "attribute.h" /* pbs_attribute, attribute_def */
#include "dynamic_string.h" /* dynamic_string */

char *path_priv = NULL;
const char *msg_svdbopen = "Unable to open server data base";
char *path_svrdb = NULL;
struct server server;
attribute_def svr_attr_def[10];
int disable_timeout_check;
time_t pbs_tcp_timeout;
time_t pbs_incoming_tcp_timeout;



ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
 {
 fprintf(stderr, "The call to read_nonblocking_socket to be mocked!!\n");
 exit(1);
 }

int get_parent_and_child(char *start, char **parent, char **child, char **end)
 {
 fprintf(stderr, "The call to get_parent_and_child to be mocked!!\n");
 exit(1);
 }

int write_buffer(char *buf, int len, int fds)
 {
 fprintf(stderr, "The call to write_buffer to be mocked!!\n");
 exit(1);
 }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
 {
 fprintf(stderr, "The call to clear_attr to be mocked!!\n");
 exit(1);
 }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
 {
 fprintf(stderr, "The call to write_nonblocking_socket to be mocked!!\n");
 exit(1);
 }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
 {
 fprintf(stderr, "The call to find_attr to be mocked!!\n");
 exit(1);
 }

int decode_resc(struct pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
 {
 fprintf(stderr, "The call to decode_resc to be mocked!!\n");
 exit(1);
 }

int unescape_xml(char *in, char *out, int size)
 {
 fprintf(stderr, "The call to unescape_xml to be mocked!!\n");
 exit(1);
 }

int recov_attr(int fd, void *parent, struct attribute_def *padef, struct pbs_attribute *pattr, int limit, int unknown, int do_actions)
 {
 fprintf(stderr, "The call to recov_attr to be mocked!!\n");
 exit(1);
 }

void *get_next(list_link pl, char *file, int line)
 {
 fprintf(stderr, "The call to get_next to be mocked!!\n");
 exit(1);
 }

int decode_arst(struct pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
 {
 fprintf(stderr, "The call to decode_arst to be mocked!!\n");
 exit(1);
 }

int save_attr_xml(struct attribute_def *padef, struct pbs_attribute *pattr, int numattr, int fds)
 {
 fprintf(stderr, "The call to save_attr_xml to be mocked!!\n");
 exit(1);
 }

int escape_xml(char *in, char *out, int size)
 {
 fprintf(stderr, "The call to escape_xml to be mocked!!\n");
 exit(1);
 }

int append_dynamic_string(dynamic_string *ds, const char *to_append)
 {
 fprintf(stderr, "The call to append_dynamic_string to be mocked!!\n");
 exit(1);
 }

int size_to_dynamic_string(dynamic_string *ds, struct size_value *szv)
 {
 fprintf(stderr, "The call to size_to_dynamic_string to be mocked!!\n");
 exit(1);
 }

int append_dynamic_string_xml(dynamic_string *ds, const char *str)
 {
 fprintf(stderr, "The call to append_dynamic_string_xml to be mocked!!\n");
 exit(1);
 }

int unlock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

int lock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int type, const char *func_name, const char *msg, int o) {}

int str_to_attr(

  const char           *name,   /* I */
  char                 *val,    /* I */
  pbs_attribute        *attr,   /* O */
  struct attribute_def *padef,  /* I */
  int                   limit)  /* I */

  {
  return(0);
  }
