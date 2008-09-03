#ifndef PORT_FORWARDING_H
#define PORT_FORWARDING_H 1

#define BUF_SIZE 1024
#define NUM_SOCKS 100
#define MAX_DISPLAYS 500
#define X11OFFSET 50


/* derived from XF4/xc/lib/dps/Xlibnet.h */
#ifndef X_UNIX_PATH
#  ifdef __hpux
#    define X_UNIX_PATH "/var/spool/sockets/X11/%u"
#  else
#    define X_UNIX_PATH "/tmp/.X11-unix/X%u"
#  endif
#endif /* X_UNIX_PATH */

#ifndef NI_MAXSERV
# define NI_MAXSERV 32
#endif /* !NI_MAXSERV */


struct pfwdsock
  {
  int sock;
  int listening;
  int remotesock;
  int bufavail;
  int bufwritten;
  int active;
  int peer;
  char buff[BUF_SIZE];
  };

void port_forwarder(struct pfwdsock *, int(*connfunc)(char *phost, int pport, char *), char*, int, char *);
void set_nodelay(int);
int connect_local_xsocket(u_int);
int x11_connect_display(char *, int, char *);


#endif /* END PORT_FORWARDING_H */
