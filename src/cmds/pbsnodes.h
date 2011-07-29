#include "license_pbs.h" /* See here for the software license */

/* static int set_note(int con, char *name, char *msg); */

/* static void prt_node_attr(struct batch_status *pbs, int IsVerbose);  */

/* static char *get_nstate(struct batch_status *pbs);  */

/* static char *get_note(struct batch_status *pbs);  */

/* static int marknode(int con, char *name, char *state1, enum batch_op op1, char *state2, enum batch_op op2); */

struct batch_status *statnode(int con, char *nodearg);

void addxmlnode(mxml_t *DE, struct batch_status *pbstat);

int filterbystate(struct batch_status *pbstat, enum NStateEnum ListType, char *S);

/* main */
