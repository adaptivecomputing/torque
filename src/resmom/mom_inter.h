#ifndef _MOM_INTER_H
#define _MOM_INTER_H
#include "license_pbs.h" /* See here for the software license */

/* static int read_net(int sock, char *buf, int amt); */

char *rcvttype(int sock);

void set_termcc(int fd);

int rcvwinsize(int sock);

int setwinsize(int pty);

int mom_reader(int s, int ptc);

int mom_writer(int s, int ptc);

int x11_create_display(int x11_use_localhost, char *display, char *phost, int pport, char *homedir, char *x11authstr);

#endif /* _MOM_INTER_H */
