#include "license_pbs.h" /* See here for the software license */

#include "pbs_ifl.h" /* batch_status */
#include <string>

int isjobid(const char *string);

int istrue(const char *string); 

int timestring_to_int(const char *, int *);

int process_commandline_opts(int, char **, int *, int *);

void get_ct(const char *, int *, int *);
 
int run_job_mode(bool, const char *, int *, char *, char *, char *, char *, char *, std::string&);

int run_queue_mode(bool, const char *, char *, char *, char *, std::string&);

int run_server_mode(bool, const char *, char *, std::string&);

/* static void states(char *string, char *q, char *r, char *h, char *w, char *t, char *e, int len); */

/* void prt_attr(char *n, char *r, char *v); */

/* static char *findattrl(struct attrl *pattrl, char *name, char *resc);*/

/* static void prt_nodes(char *nodes); */

/* static char *cnv_size(char *value, int opt);*/

/* static void altdsp_statjob(struct batch_status *pstat, struct batch_status *prtheader, int alt_opt); */

/* static void get_ct(char *str, int *jque, int *jrun);*/

/* static void altdsp_statque(char *serv, struct batch_status *pstat, int opt);*/

/* static void add_atropl(struct attropl **list, char *name, char *resc, char *value, enum batch_op op);*/

void display_statjob(struct batch_status *status, int prtheader, int full); 

void display_statque(struct batch_status *status, int prtheader, int full);

void display_statserver(struct batch_status *status, int prtheader, int full);

char *attrlist(struct attrl *ap);

void tcl_init(void);

void tcl_addarg(char *name, char *arg);

int tcl_stat(char *type, struct batch_status *bs, int f_opt);

void tcl_run(int f_opt);

std::string get_err_msg(
  int   any_failed,
  const char *mode,
  int   connect,
  char *id);
/* main */
