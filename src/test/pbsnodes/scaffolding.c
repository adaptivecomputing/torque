#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "mcom.h" /* mxml_ti, MDataFormatEnumi, mbool_t */
#include "pbs_ifl.h" /* batch_status, attrl */


int pbs_errno = 0;


char *pbs_geterrmsg(int connect)
  { 
  fprintf(stderr, "The call to pbs_geterrmsg needs to be mocked!!\n");
  exit(1);
  }

int TShowAbout_exit(void)
  { 
  fprintf(stderr, "The call to TShowAbout_exit needs to be mocked!!\n");
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

int MXMLToXString(mxml_t *E, char **Buf, int *BufSize, int MaxBufSize, char **Tail, mbool_t IsRootElement)
  { 
  fprintf(stderr, "The call to MXMLToXString needs to be mocked!!\n");
  exit(1);
  }

extern "C"
{
int cnt2server(const char *SpecServer, bool silence)
  { 
  fprintf(stderr, "The call to cnt2server needs to be mocked!!\n");
  exit(1);
  }

char *pbs_strerror(int err)
  { 
  fprintf(stderr, "The call to pbs_strerror needs to be mocked!!\n");
  exit(1);
  }
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

int MXMLCreateE(mxml_t **E, const char *Name)
  { 
  fprintf(stderr, "The call to MXMLCreateE needs to be mocked!!\n");
  exit(1);
  }

int pbs_modify_node_err(int c, int command, int objtype, char *objname, struct attropl *attrib,
  char *extend, int *local_errno)
  {
  return(0);
  }

char *pbs_default(void)
  { 
  fprintf(stderr, "The call to pbs_default needs to be mocked!!\n");
  exit(1);
  }



