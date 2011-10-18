#ifndef _PRINTSERVERDB_H
#define _PRINTSERVERDB_H

#include "server.h" /* server */

void prt_server_struct(struct server *pserver);

int read_attr(int fd);

void dumpdb(char *file);

#endif /* _PRINTSERVERDB_H */
