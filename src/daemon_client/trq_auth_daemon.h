#include "license_pbs.h" /* See here for the software license */
#ifndef _TRQ_AUTH_DAEMON_H
#define _TRQ_AUTH_DAEMON_H

int load_config(char **ip, int *t_port, int *d_port);

int load_ssh_key(char **ssh_key);

int trq_main(int argc, char **argv, char **envp);

#endif /* _TRQ_AUTH_DAEMON_H */
