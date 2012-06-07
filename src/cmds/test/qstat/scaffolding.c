#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */ 

#include "pbs_ifl.h" /* attopl, attrl */
#include "mcom.h" /* MDataFormatEnum */
int pbs_errno = 0;
char *pbs_server = NULL;

char *pbs_geterrmsg(int connect)
  { 
  fprintf(stderr, "The call to pbs_geterrmsg needs to be mocked!!\n");
  exit(1);
  }

struct batch_status * pbs_selstat(int c, struct attropl *attrib, char *extend)
  { 
  fprintf(stderr, "The call to pbs_selstat needs to be mocked!!\n");
  exit(1);
  }

struct batch_status * pbs_selstat_err(int c, struct attropl *attrib, char *extend, int *any_failed)
  { 
  fprintf(stderr, "The call to pbs_selstat_err needs to be mocked!!\n");
  exit(1);
  }

int TShowAbout_exit(void)
  { 
  fprintf(stderr, "The call to TShowAbout_exit needs to be mocked!!\n");
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

int MXMLSetVal(mxml_t *E, void *V, enum MDataFormatEnum Format)
  { 
  fprintf(stderr, "The call to MXMLSetVal needs to be mocked!!\n");
  exit(1);
  }

int pbs_disconnect(int connect)
  { 
  fprintf(stderr, "The call to pbs_disconnect needs to be mocked!!\n");
  exit(1);
  }

int parse_destination_id(char *destination_in, char **queue_name_out, char **server_name_out)
  { 
  fprintf(stderr, "The call to parse_destination_id needs to be mocked!!\n");
  exit(1);
  }

int locate_job(char *job_id, char *parent_server, char *located_server)
  { 
  fprintf(stderr, "The call to locate_job needs to be mocked!!\n");
  exit(1);
  }

int MXMLToXString(mxml_t *E, char **Buf, int *BufSize, int MaxBufSize, char **Tail, mbool_t IsRootElement)
  { 
  fprintf(stderr, "The call to MXMLToXString needs to be mocked!!\n");
  exit(1);
  }

int cnt2server(char *SpecServer)
  { 
  fprintf(stderr, "The call to cnt2server needs to be mocked!!\n");
  exit(1);
  }

struct batch_status *pbs_statjob(int c, char *id, struct attrl *attrib, char *extend)
  { 
  fprintf(stderr, "The call to pbs_statjob needs to be mocked!!\n");
  exit(1);
  }

struct batch_status *pbs_statjob_err(int c, char *id, struct attrl *attrib, char *extend, int *local_errno)
  { 
  fprintf(stderr, "The call to pbs_statjob_err needs to be mocked!!\n");
  exit(1);
  }

void pbs_statfree(struct batch_status *bsp)
  { 
  fprintf(stderr, "The call to pbs_statfree needs to be mocked!!\n");
  exit(1);
  }

int MXMLDestroyE(mxml_t **EP)
  { 
  fprintf(stderr, "The call to MXMLDestroyE needs to be mocked!!\n");
  exit(1);
  }

int MXMLAddE(mxml_t *E, mxml_t *C)
  { 
  fprintf(stderr, "The call to MXMLAddE needs to be mocked!!\n");
  exit(1);
  }

int MXMLCreateE(mxml_t **E, char *Name)
  { 
  fprintf(stderr, "The call to MXMLCreateE needs to be mocked!!\n");
  exit(1);
  }

int get_server(char *job_id_in, char *job_id_out, char *server_out)
  { 
  fprintf(stderr, "The call to get_server needs to be mocked!!\n");
  exit(1);
  }

void prt_job_err(char *cmd, int connect, char *id)
  { 
  fprintf(stderr, "The call to prt_job_err needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  { 
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }

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

char *pbs_default(void)
  { 
  fprintf(stderr, "The call to pbs_default needs to be mocked!!\n");
  exit(1);
  }

void initialize_network_info() {}

