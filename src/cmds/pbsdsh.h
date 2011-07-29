#include "license_pbs.h" /* See here for the software license */

char *get_ecname(int rc);

void bailout(int sig);

int obit_submit(int c);

void mom_reconnect(void);

void getstdout(void);

void wait_for_task(int *nspawned);

char *gethostnames(tm_node_id *nodelist);

int findtargethost(char *allnodes, char *targethost);

int uniquehostlist(tm_node_id *nodelist, char *allnodes);

/* static int build_listener(int *port); */

/* main */
