#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */ 

#include "pbs_ifl.h" /* attrl, attropl */

int pbs_errno = 0;

char *pbs_geterrmsg(int connect)
  { 
  fprintf(stderr, "The call to pbs_geterrmsg needs to be mocked!!\n");
  exit(1);
  }

struct batch_status *pbs_statserver(int c, struct attrl *attrib, char *extend)
  { 
  fprintf(stderr, "The call to pbs_statserver needs to be mocked!!\n");
  exit(1);
  }

struct batch_status *pbs_statserver_err(int c, struct attrl *attrib, char *extend, int *local_errno)
  { 
  fprintf(stderr, "The call to pbs_statserver_err needs to be mocked!!\n");
  exit(1);
  }

int pbs_disconnect(int connect)
  { 
  fprintf(stderr, "The call to pbs_disconnect needs to be mocked!!\n");
  exit(1);
  }

extern "C"
{
int cnt2server(const char *SpecServer, bool silence)
  { 
  fprintf(stderr, "The call to cnt2server needs to be mocked!!\n");
  exit(1);
  }
}

int pbs_query_max_connections(void)
  { 
  fprintf(stderr, "The call to pbs_query_max_connections needs to be mocked!!\n");
  exit(1);
  }

struct batch_status *pbs_statnode(int c, char *id, struct attrl *attrib, char *extend)
  { 
  fprintf(stderr, "The call to pbs_statnode needs to be mocked!!\n");
  exit(1);
  }

struct batch_status *pbs_statnode_err(int c, char *id, struct attrl *attrib, char *extend, int *local_errno)
  { 
  fprintf(stderr, "The call to pbs_statnode_err needs to be mocked!!\n");
  exit(1);
  }

void pbs_statfree(struct batch_status *bsp)
  { 
  fprintf(stderr, "The call to pbs_statfree needs to be mocked!!\n");
  exit(1);
  }

int pbs_manager(int c, int command, int objtype, char *objname, struct attropl *attrib, char *extend)
  { 
  fprintf(stderr, "The call to pbs_manager needs to be mocked!!\n");
  exit(1);
  }

int pbs_manager_err(int c, int command, int objtype, char *objname, struct attropl *attrib, char *extend, int *local_errno)
  { 
  fprintf(stderr, "The call to pbs_manager_err needs to be mocked!!\n");
  exit(1);
  }
#ifdef __cplusplus
extern "C"
{
#endif
char *pbs_strerror(int err)
  { 
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }
#ifdef __cplusplus
}
#endif

struct batch_status *pbs_statque(int c, char *id, struct attrl *attrib, char *extend)
  { 
  fprintf(stderr, "The call to pbs_statque needs to be mocked!!\n");
  exit(1);
  }

struct batch_status *pbs_statque_err(int c, char *id, struct attrl *attrib, char *extend, int *local_errno)
  { 
  fprintf(stderr, "The call to pbs_statque_err needs to be mocked!!\n");
  exit(1);
  }

void initialize_network_info() {}

