#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "mcom.h" /* mxml_t */

enum NStateEnum
  {
  tnsNONE = 0, /* default behavior - show down, offline, and unknown nodes */
  tnsActive,   /* one or more jobs running on node */
  tnsAll,      /* list all nodes */
  tnsBusy,     /* node cannot accept additional workload */
  tnsDown,     /* node is down */
  tnsFree,     /* node is idle/free */
  tnsOffline,  /* node is offline */
  tnsUnknown,  /* node is unknown - no contact recieved */
  tnsUp,       /* node is healthy */
  tnsLAST             
  };

/* static int set_note(int con, char *name, char *msg); */

/* static void prt_node_attr(struct batch_status *pbs, int IsVerbose);  */

/* static char *get_nstate(struct batch_status *pbs);  */

/* static char *get_note(struct batch_status *pbs);  */

/* static int marknode(int con, char *name, char *state1, enum batch_op op1, char *state2, enum batch_op op2); */

struct batch_status *statnode(int con, char *nodearg);

void addxmlnode(mxml_t *DE, struct batch_status *pbstat);

int filterbystate(struct batch_status *pbstat, enum NStateEnum ListType, char *S);

/* main */
