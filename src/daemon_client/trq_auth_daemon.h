#include "license_pbs.h" /* See here for the software license */
#ifndef _TRQ_AUTH_DAEMON_H
#define _TRQ_AUTH_DAEMON_H

int load_config(char **ip, int *t_port, int *d_port);

int load_ssh_key(char **ssh_key);

extern "C"
{
int trq_main(int argc, char **argv, char **envp);
}

void clean_log_init_mutex(void);

void initialize_globals_for_log(const char *server_port);

int termiante_trqauthd(void);

int load_trqauthd_config(char **, int *, char **);

#endif /* _TRQ_AUTH_DAEMON_H */
