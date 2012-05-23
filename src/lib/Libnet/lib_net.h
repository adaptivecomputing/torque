#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <time.h> /* time_t */
#include <netinet/in.h> /* sockaddr_in */
#include <sys/select.h> /* fd_set */
#include <netdb.h> /* struct addrinfo */

#include "net_connect.h" /* pbs_net_t,conn_type */
#include "md5.h" /* MD5_CTX */
#include "port_forwarding.h" /* pfwdsock */


/* from file conn_table.c */
int get_connection_entry(int *conn_pos);

/* from file get_hostaddr.c */
char *PAddrToString(pbs_net_t *Addr);
pbs_net_t get_hostaddr(int *, char *hostname);
int get_hostaddr_hostent_af(int *, char *hostname, unsigned short *af_family, char **host_addr, int *host_addr_len);

/* from file get_hostname.c */
int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg);

/* from file md5.c */
void MD5Init(MD5_CTX *mdContext);
void MD5Update(MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen);
void MD5Final(MD5_CTX *mdContext);
/* static void Transform(UINT4 *buf, UINT4 *in); */

/* from file net_common.c */
unsigned availBytesOnDescriptor(int pLocalSocket);
int socket_avail_bytes_on_descriptor(int socket);
int socket_get_tcp();
int get_listen_socket(struct addrinfo *);
int get_random_reserved_port();
int socket_get_tcp_priv();
int socket_connect(int *local_socket, char *dest_addr, int dest_addr_len, int dest_port, int family, int is_privileged, char **err_msg);
int socket_connect_addr(int *local_socket, struct sockaddr *remote, size_t remote_size, int is_privileged, char **err_msg);
int socket_wait_for_write(int socket);
int socket_wait_for_xbytes(int socket, int len);
int socket_wait_for_read(int socket);
void socket_read_flush(int socket);
int socket_write(int socket, char *data, int data_len);
int socket_read_force(int socket, char *the_str, long long avail_bytes, long long *byte_count);
int socket_read(int socket, char **the_str, long long *str_len);
int socket_read_one_byte(int socket, char *one_char);
int socket_read_num(int socket, long long *the_num);
int socket_read_str(int socket, char **the_str, long long *str_len);
int socket_close(int socket);
int get_addr_info(char *name, struct sockaddr_in *sa_info, int retry);

/* from file server_core.c */
int start_listener(char *server_ip, int server_port, void *(*process_meth)(void *));
int start_listener_addrinfo(char *host_name, int server_port, void *(*process_meth)(void *));

/* from file net_client.c */
#ifdef __APPLE__
int bindresvport(int sd, struct sockaddr_in *sin);
#endif
int get_max_num_descriptors(void);
int get_fdset_size(void);
/* static int await_connect(long timeout, int sockd); */
int client_to_svr(pbs_net_t hostaddr, unsigned int port, int local_port, char *EMsg);

/* from file net_server.c */
void global_sock_add(int new_sock);
void global_sock_rem(int new_sock);
fd_set *global_sock_getlist();
int global_sock_verify();
void netcounter_incr(void);
int get_num_connections();
void netcounter_get(int netrates[]);
int init_network(unsigned int port, void *(*readfunc)(void *));
int check_network_port(unsigned int port);
int thread_func(int active_sockets, fd_set *select_set);
int wait_request(time_t waittime, long *SState);
/* static void accept_conn(void *new_conn); */
void globalset_add_sock(int sock);
void globalset_del_sock(int sock);
int add_conn(int, enum conn_type, pbs_net_t, unsigned int, unsigned int, void *(*func)(void *));
int add_scheduler_conn(int, enum conn_type, pbs_net_t, unsigned int, unsigned int, void *(*func)(void *));
void close_conn(int sd, int has_mutex);
void net_close(int but);
pbs_net_t get_connectaddr(int sock, int mutex);
int get_connecthost(int sock, char *namebuf, int size);
char *netaddr_pbs_net_t(pbs_net_t ipadd);

/* from file net_set_clse.c */
void net_add_close_func(int, void (*func)(int));

/* from file port_forwarding.c */
void port_forwarder(struct pfwdsock *socks, int (*connfunc)(char *, int, char *), char *phost, int pport, char *EMsg);
void set_nodelay(int fd);
int connect_local_xsocket(u_int dnr);
int x11_connect_display(char *display, int alsounused, char *EMsg);

/* from file rm.c */
/* static int addrm(int stream); */
int openrm(char *host, unsigned int port);
/* static int delrm(int stream); */
/* static struct out *findout(int stream); */
/* static int startcom(int stream, int com);  */
/* static int simplecom(int stream, int com); */
/* static int simpleget(int stream); */
int closerm(int stream);
int downrm(int stream);
int configrm(int stream, char *file);
/* static int doreq(struct out *op, char *line); */
int addreq(int stream, char *line);
int allreq(char *line);
char *getreq(int stream);
int flushreq(void);
int activereq(void);
void fullresp(int flag);

