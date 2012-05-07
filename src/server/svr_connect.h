#ifndef _SVR_CONNECT_H
#define _SVR_CONNECT_H
#include "license_pbs.h" /* See here for the software license */

#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "pbs_nodes.h" /* pbsnode */
#include "net_connect.h" /* conn_type */

void connection_clear(int con_pos);
int svr_connect(pbs_net_t hostaddr, unsigned int port, int *local_errno, struct pbsnode *pnode, void *(*func)(void *), enum conn_type cntype);
void svr_disconnect_sock(int handle);
void svr_disconnect(int handle);
int get_connection_entry(int *conn_pos);
char *parse_servername(char *name, unsigned int *service);
#endif /* _SVR_CONNECT_H */
