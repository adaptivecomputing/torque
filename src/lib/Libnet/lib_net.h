#include "license_pbs.h" /* See here for the software license */


/* from file get_hostaddr.c */
char *PAddrToString(pbs_net_t *Addr);;
pbs_net_t get_hostaddr(char *hostname);;

/* from file get_hostname.c */
int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg); 

/* from file md5.c */
void MD5Init(MD5_CTX *mdContext);
void MD5Update(MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen);
void MD5Final(MD5_CTX *mdContext);
/* static void Transform(UINT4 *buf, UINT4 *in); */

/* from file net_client.c */
#ifdef __APPLE__
int bindresvport(int sd, struct sockaddr_in *sin);;
#endif
int get_max_num_descriptors(void);
int get_fdset_size(void);
/* static int await_connect(long timeout, int sockd); */
int client_to_svr(pbs_net_t hostaddr, unsigned int port, int local_port, char *EMsg);

/* from file net_server.c */
void netcounter_incr(void);
int get_num_connections();
int *netcounter_get(void);
int init_network(unsigned int port, void (*readfunc)());
int wait_request(time_t waittime, long *SState); 
static void accept_conn(int sd); 
void add_conn(int sock, enum conn_type type, pbs_net_t addr, unsigned int port, unsigned int socktype, void (*func);(int)) 
void close_conn(int sd); 
void net_close(int but); 
pbs_net_t get_connectaddr(int sock, int mutex); 
int find_conn(pbs_net_t addr); 
int get_connecthost(int sock, char *namebuf, int size); 
char *netaddr_pbs_net_t(pbs_net_t ipadd);

/* from file net_set_clse.c */
void net_add_close_func(int sd, void (*func);(int))
 
/* from file port_forwarding.c */
void port_forwarder(struct pfwdsock *socks, int (*connfunc)(char *, int, char *), char *phost, int pport, char *EMsg)
void set_nodelay(int fd);
int connect_local_xsocket(u_int dnr);
int x11_connect_display(char *display, int alsounused, char *EMsg);

/* from file rm.c */
static int addrm(int stream);
int openrm(char *host, unsigned int port);
static int delrm(int stream);
static struct out *findout(int stream);
static int startcom(int stream, int com); 
static int simplecom(int stream, int com);
static int simpleget(int stream);
int closerm(int stream);
int downrm(int stream); 
int configrm(int stream, char *file); 
static int doreq(struct out *op, char *line);
int addreq(int stream, char *line);
int allreq(char *line);
char *getreq(int stream); 
int flushreq(void);
int activereq(void);
void fullresp(int flag);

