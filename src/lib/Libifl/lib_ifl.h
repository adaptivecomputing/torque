#include "license_pbs.h" /* See here for the software license */

#include <netinet/in.h> /* in_addr_t */
#include "attribute.h" /* attropl, attrl */
#include "libpbs.h" /* job_file */
#include "batch_request.h" /* batch_request */
#include "tm_.h" /* tm_task_id, tm_node_id, tm_event_t */
#include "tm.h" /* tm_roots */
#include "tcp.h" /* tcp_chan */
#include <string>
#include "u_hash_map_structs.h"

/* trq_auth.c */
#define AUTH_TYPE_IFF 1
#define AUTH_TYPE_KEY 2

#define  TRQ_AUTH_CONNECTION        1  /* Authorize a client connection. Used by trqauthd */
#define  TRQ_GET_ACTIVE_SERVER      2  /* Request the name of the currently active pbs_server from trqauthd */
#define  TRQ_VALIDATE_ACTIVE_SERVER 3 /* Request trqauthd to validate which server is active */
#define  TRQ_DOWN_TRQAUTHD          4 /* Terminate trqauthd */
#define  TRQ_PING_SERVER            5 /* just request a response from the trqauthd server */


int parse_request_client(int sock, char **server_name, int *server_port, int *auth_type, char **user, int *user_pid, int *user_sock);
int build_request_svr(int auth_type, char *user, int sock, char **send_message);
int parse_response_svr(int sock, char **msg);
int build_response_client(int code, char *msg, char **send_message);
int get_trq_server_addr(char *server_name, char **server_addr, int *server_addr_len);
void *process_svr_conn(void *sock);
int validate_server(char *active_server_name, int t_server_port, char *ssh_key, char **sign_key);
int set_active_pbs_server(const char *, const int);
int get_active_pbs_server(char **, int *);
int validate_active_pbs_server(char **);
int trq_simple_connect(const char *server_name, int batch_port, int *handle);
int trq_simple_disconnect(int handle);
void send_svr_disconnect(int, const char *);
int set_trqauthd_addr(void);
int validate_user(int sock, const char *user_name, int user_pid, char *msg);

/* PBSD_gpuctrl2.c */
int PBSD_gpu_put(int c, char *node, char *gpuid, int gpumode, int reset_perm, int reset_vol, char *extend);

/* PBSD_manage2.c */
int PBSD_mgr_put(int c, int function, int command, int objtype, const char *objname, struct attropl *aoplp, char *extend); 

/* PBSD_manager_caps.c */
int PBSD_manager(int c, int function, int command, int objtype, const char *objname, struct attropl *aoplp, char *extend, int *);

/* PBSD_msg2.c */
int PBSD_msg_put(int c, char *jobid, int fileopt, char *msg, char *extend);

/* PBSD_rdrpy.c */
struct batch_reply *PBSD_rdrpy(int *, int c); 
void PBSD_FreeReply(struct batch_reply *reply);

/* PBSD_sig2.c */
int PBSD_sig_put(int c, const char *jobid, const char *signal, char *extend);
int PBSD_async_sig_put(int c, const char *jobid, const char *signal, char *extend);

/* PBSD_status.c */
struct batch_status *PBSD_status(int c, int function, int *, char *id, struct attrl *attrib, char *extend); 
struct batch_status *PBSD_status_get(int *, int c); 
/* static struct batch_status * alloc_bs(void); */

/* PBSD_status2.c */
int PBSD_status_put(int c, int function, char *id, struct attrl *attrib, char *extend);

/* PBSD_submit_caps.c */
int PBSD_rdytocmt(int connect, char *jobid);
int PBSD_commit_get_sid(int connect, long *sid, char *jobid); 
int PBSD_commit(int connect, char *jobid); 
int pbs_submit_hash(
  int                socket,
  job_data_container *job_attr,
  job_data_container *res_attr,
  char              *script,
  char              *destination,
  char              *extend,  /* (optional) */
  char              **return_jobid,
  char              **msg);
/* static int PBSD_scbuf(int c, int reqtype, int seq, char *buf, int len, char *jobid, enum job_file which);  */
int PBSD_jscript(int c, const char *script_file, const char *jobid);
int PBSD_jobfile(int c, int req_type, char *path, char *jobid, enum job_file which);
char *PBSD_queuejob(int connect, int *, const char *jobid, const char *destin, struct attropl *attrib, char *extend);
int PBSD_QueueJob_hash(int connect, char *jobid, char *destin, job_data *job_attr, job_data *res_attr, char *extend, char **job_id, char **msg);

/* PBS_attr.c */
int PBS_val_al(struct attrl *alp);
void PBS_free_al(struct attrl *alp);
int PBS_val_aopl(struct attropl *aoplp);
void PBS_free_aopl(struct attropl *aoplp);

/* PBS_data.c */

/* dec_Authen.c */
int decode_DIS_Authen(struct tcp_chan *chan, struct batch_request *preq);
int decode_DIS_AltAuthen(struct tcp_chan *chan, struct batch_request *preq);

/* dec_CpyFil.c */
int decode_DIS_CopyFiles(struct tcp_chan *chan, struct batch_request *preq);

/* dec_Gpu.c */
int decode_DIS_GpuCtrl(struct tcp_chan *chan, struct batch_request *preq);

/* dec_JobCred.c */
int decode_DIS_JobCred(struct tcp_chan *chan, struct batch_request *preq);

/* dec_JobFile.c */
int decode_DIS_JobFile(struct tcp_chan *chan, struct batch_request *preq);

/* dec_JobId.c */
int decode_DIS_JobId(struct tcp_chan *chan, char *jobid);

/* dec_JobObit.c */
int decode_DIS_JobObit(struct tcp_chan *chan, struct batch_request *preq); 

/* dec_Manage.c */
int decode_DIS_Manage(struct tcp_chan *chan, struct batch_request *preq);

/* dec_MoveJob.c */
int decode_DIS_MoveJob(struct tcp_chan *chan, struct batch_request *preq);

/* dec_MsgJob.c */
int decode_DIS_MessageJob(struct tcp_chan *chan, struct batch_request *preq);

/* dec_QueueJob.c */
int decode_DIS_QueueJob(struct tcp_chan *chan, struct batch_request *preq);

/* dec_Reg.c */
int decode_DIS_Register(struct tcp_chan *chan, struct batch_request *preq);

/* dec_ReqExt.c */
int decode_DIS_ReqExtend(struct tcp_chan *chan, struct batch_request *preq);

/* dec_ReqHdr.c */
int decode_DIS_ReqHdr(struct tcp_chan *chan, struct batch_request *preq, int *proto_type, int *proto_ver);

/* dec_Resc.c */
int decode_DIS_Rescl(struct tcp_chan *chan, struct batch_request *preq);

/* dec_ReturnFile.c */
int decode_DIS_ReturnFiles(struct tcp_chan *chan, struct batch_request *preq);

/* dec_RunJob.c */
int decode_DIS_RunJob(struct tcp_chan *chan, struct batch_request *preq);

/* dec_Shut.c */
int decode_DIS_ShutDown(struct tcp_chan *chan, struct batch_request *preq);

/* dec_Sig.c */
int decode_DIS_SignalJob(struct tcp_chan *chan, struct batch_request *preq);

/* dec_Status.c */
int decode_DIS_Status(struct tcp_chan *chan, struct batch_request *preq);

/* dec_Track.c */
int decode_DIS_TrackJob(struct tcp_chan *chan, struct batch_request *preq);

/* dec_attrl.c */
int decode_DIS_attrl(struct tcp_chan *chan, struct attrl **ppatt);

/* dec_attropl.c */
int decode_DIS_attropl(struct tcp_chan *chan, struct attropl **ppatt);

/* dec_rpyc.c */
int decode_DIS_replyCmd(struct tcp_chan *chan, struct batch_reply *reply);

/* dec_rpys.c */
int decode_DIS_replySvr(struct tcp_chan *chan, struct batch_reply *reply); 

/* dec_svrattrl.c */
int decode_DIS_svrattrl(struct tcp_chan *chan, tlist_head *phead);

/* enc_CpyFil.c */
int encode_DIS_CopyFiles(struct tcp_chan *chan, struct batch_request *preq);

/* enc_Gpu.c */
int encode_DIS_GpuCtrl(struct tcp_chan *chan, char *node, char *gpuid, int gpumode, int reset_perm, int reset_vol);

/* enc_JobCred.c */
int encode_DIS_JobCred(struct tcp_chan *chan, int type, char *cred, int len);

/* enc_JobFile.c */
int encode_DIS_JobFile(struct tcp_chan *chan, int seq, char *buf, int len, const char *jobid, int which);

/* enc_JobId.c */
int encode_DIS_JobId(struct tcp_chan *chan, char *jobid);

/* enc_JobObit.c */
int encode_DIS_JobObit(struct tcp_chan *chan, struct batch_request *preq); 

/* enc_Manage.c */
int encode_DIS_Manage(struct tcp_chan *chan, int command, int objtype, const char *objname, struct attropl *aoplp);

/* enc_MoveJob.c */
int encode_DIS_MoveJob(struct tcp_chan *chan, char *jobid, char *destin); 

/* enc_MsgJob.c */
int encode_DIS_MessageJob(struct tcp_chan *chan, char *jobid, int fileopt, char *msg);

/* enc_QueueJob.c */
int encode_DIS_QueueJob(struct tcp_chan *chan, const char *jobid, const char *destin, struct attropl *aoplp);

/* enc_Reg.c */
int encode_DIS_Register(struct tcp_chan *chan, struct batch_request *preq);

/* enc_ReqExt.c */
int encode_DIS_ReqExtend(struct tcp_chan *chan, char *extend);

/* enc_ReqHdr.c */
int encode_DIS_ReqHdr(struct tcp_chan *chan, int reqt, char *user);

/* enc_ReturnFile.c */
int encode_DIS_ReturnFiles(struct tcp_chan *chan, struct batch_request *preq);

/* enc_RunJob.c */
int encode_DIS_RunJob(struct tcp_chan *chan, char *jobid, char *where, unsigned int resch); 

/* enc_Shut.c */
int encode_DIS_ShutDown(struct tcp_chan *chan, int manner); 

/* enc_Sig.c */
int encode_DIS_SignalJob(struct tcp_chan *chan, const char *jobid, const char *signal);

/* enc_Status.c */
int encode_DIS_Status(struct tcp_chan *chan, char *objid, struct attrl *pattrl);

/* enc_Track.c */
int encode_DIS_TrackJob(struct tcp_chan *chan, struct batch_request *preq);

/* enc_attrl.c */
int encode_DIS_attrl(struct tcp_chan *chan, struct attrl *pattrl);

/* enc_attropl.c */
int encode_DIS_attropl(struct tcp_chan *chan, struct attropl *pattropl);

/* enc_reply.c */
int encode_DIS_reply(struct tcp_chan *chan, struct batch_reply *reply);

/* enc_svrattrl.c */
int encode_DIS_svrattrl(struct tcp_chan *chan, svrattrl *psattl);

/* list_link.c */
void insert_link(struct list_link *old, struct list_link *new_link, void *pobj, int position); 
void append_link(tlist_head *head, list_link *new_link, void *pobj); 
void delete_link(struct list_link *old);
void swap_link(list_link *pone, list_link *ptwo);
int is_linked(list_link *head, list_link *entry);
int is_link_initialized(list_link *head);
#ifndef NDEBUG
void *get_next(list_link pl, char *file, int line); 
void *get_prior(list_link pl, char *file, int line);
#endif
void list_move(tlist_head *from, tlist_head *to);
void free_pidlist(struct pidl *pl);

/* nonblock.c */
/* ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count);  */
/* ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count); */
extern ssize_t write_ac_socket(int, const void *, ssize_t);
extern ssize_t read_ac_socket(int, void *, ssize_t);

ssize_t read_blocking_socket(int fd, void *buf, ssize_t count);

/* pbsD_alterjo.c */
int pbs_alterjob_asyncflag(int c, char *jobid, struct attrl *attrib, char *extend, int asyncFlag, int *); 
int pbs_alterjob_async_err(int c, char *jobid, struct attrl *attrib, char *extend, int *); 
int pbs_alterjob_err(int c, char *jobid, struct attrl *attrib, char *extend, int *); 

/* pbsD_asyrun.c */
int pbs_asyrunjob(int c, char *jobid, char *location, char *extend);
int pbs_asyrunjob_err(int c, char *jobid, char *location, char *extend, int *);

/* pbsD_chkptjob.c */
int pbs_checkpointjob_err(int c, char *jobid, char *extend, int *);

/* pbsD_connect.c */
void empty_alarm_handler(int signo);
char *PBS_get_server(const char *server, unsigned int *port);
void get_port_from_server_name_file(unsigned int *server_name_file_port);
#ifdef MUNGE_AUTH
int PBSD_munge_authenticate(int psock, int handle); 
#endif 
#ifndef MUNGE_AUTH
int parse_svr_response(long long code, long long len, char *buf);
int validate_socket(int psock);
int parse_daemon_response(long long code, long long len, char *buf);
#endif 
#ifdef ENABLE_UNIX_SOCKETS
ssize_t send_unix_creds(int sd);
#endif 
int pbs_original_connect(char *server); 
int pbs_disconnect_socket(int socket);
int pbs_connect_with_retry(char *server_name_ptr, int retry_seconds); 
void initialize_connections_table();
int parse_daemon_response(long long code, long long len, char *buf);

/* pbsD_deljob.c */
int pbs_deljob_err(int c, const char *jobid, char *extend, int *);

/* pbsD_gpuctrl.c */
int pbs_gpumode_err(int c, char *node, char *gpuid, int gpumode, int *);
int pbs_gpureset(int c, char *node, char *gpuid, int permanent, int vol);
int pbs_gpureset_err(int c, char *node, char *gpuid, int permanent, int vol, int *);

/* pbsD_holdjob.c */
int pbs_holdjob_err(int c, const char *jobid, const char *holdtype, char *extend, int *);

/* pbsD_locjob.c */
char * pbs_locjob_err(int c, char *jobid, char *extend, int *);

/* pbsD_manager.c */
int pbs_manager_err(int c, int command, int objtype, char *objname, struct attropl *attrib, char *extend, int *);

int pbs_modify_node_err(int c, int command, int objtype, char *objname, struct attropl *attrib, char *extend, int *);

/* pbsD_movejob.c */
int pbs_movejob_err(int c, char *jobid, char *destin, char *extend, int *); 

/* pbsD_msgjob.c */
int pbs_msgjob_err(int c, char *jobid, int fileopt, char *msg, char *extend, int *);

/* pbsD_orderjo.c */
int pbs_orderjob_err(int c, char *job1, char *job2, char *extend, int *);

/* pbsD_rerunjo.c */
int pbs_rerunjob_err(int c, char *jobid, char *extend, int *);

/* pbsD_resc.c */
/* static int encode_DIS_Resc(int sock, char **rlist, int ct, resource_t rh); */
/* static int PBS_resc(int c, int reqtype, char **rescl, int ct, resource_t rh); */
char *avail(int con, char *resc);

/* pbsD_rlsjob.c */
int pbs_rlsjob_err(int c, const char *jobid, const char *holdtype, char *extend, int *);

/* pbsD_runjob.c */
int pbs_runjob_err(int c, char *jobid, char *location, char *extend, int *);

/* pbsD_selectj.c */
char ** pbs_selectjob_err(int c, struct attropl *attrib, char *extend, int *);
struct batch_status * pbs_selstat_err(int c, struct attropl *attrib, char *extend, int *);
struct batch_status * pbs_selstatattr_err(int c, struct attropl *attropl, struct attrl *attrib, char *extend, int *);
/* static int PBSD_select_put(int c, int type, struct attropl *attrib, char *extend); */
/* static char **PBSD_select_get(int c); */

/* pbsD_sigjob.c */
int pbs_sigjob_err(int c, const char *jobid, const char *signal, char *extend, int *);
int pbs_sigjobasync_err(int c, const char *jobid, const char *signal, char *extend, int *);

/* pbsD_statjob.c */
struct batch_status *pbs_statjob_err(int c, char *id, struct attrl *attrib, char *extend, int *); 

/* pbsD_statnode.c */
struct batch_status *pbs_statnode_err(int c, char *id, struct attrl *attrib, char *extend, int *);

/* pbsD_statque.c */
struct batch_status *pbs_statque_err(int c, char *id, struct attrl *attrib, char *extend, int *); 

/* pbsD_statsrv.c */
struct batch_status *pbs_statserver_err(int c, struct attrl *attrib, char *extend, int *); 

/* pbsD_submit.c */
char *pbs_submit_err(int c, struct attropl *attrib, char *script, char *destination, char *extend, int *); 

/* pbsD_termin.c */
int pbs_terminate_err(int c, int manner, char *extend, int *);


/* rpp.c */
void blog_init(char *s, int len);
int blog_write(char *s);
void blog_out(char *filename);
/* static int next_seq(int *seq); */
void set_rpp_throttle_sleep_time(long sleep_time);
/* static void rpp_form_pkt(int index, int type, int seq, u_char *buf, int len); */
/* static struct stream *rpp_check_pkt(int index, struct sockaddr_in *addrp); */
/* static void rpp_send_out(void); */
/* static int rpp_create_sp(void); */
/* static struct hostent *rpp_get_cname(struct sockaddr_in *addr);  */
/* static void rpp_alist(struct hostent *hp, struct stream *sp); */
/* static int rpp_send_ack(struct stream *sp, int seq); */
/* static void dqueue(struct send_packet *pp); */
/* static void clear_send(struct stream *sp); */
/* static void clear_stream(struct stream *sp); */
/* static int rpp_recv_pkt(int fd);  */
/* static int rpp_recv_all(void);  */
/* static void rpp_stale(struct stream *sp);  */
/* static int rpp_dopending(int index, int flag);  */
int rpp_flush(int index);
int rpp_bind(uint port);
int rpp_open(char *name, uint port, char *EMsg); 
struct sockaddr_in *rpp_getaddr(int index);
void rpp_shutdown_on_exit(int foo, void *bar);
void rpp_shutdown(void);
int rpp_close(int index);
int rpp_write(int index, void *buf, int len);
/* static int rpp_attention(int index);  */
/* static int rpp_okay(int index);  */
int rpp_read(int index, void *buf, int len); 
int rpp_rcommit(int index, int flag); 
int rpp_eom(int index); 
int rpp_wcommit(int index, int flag);
int rpp_skip(int index, int len);
void started_servicing(int index);
void done_servicing(int index);
int being_serviced(int index);
int rpp_poll(void); 
int rpp_io(void); 
int rpp_getc(int index);
int rpp_putc(int index, int c);
int RPPConfigure(int SRPPTimeOut, int SRPPRetry); 
int RPPReset(void);
int rpp_get_stream_state(int index);

/* tcp_dis.c */
void DIS_tcp_settimeout(long timeout); 
/* static void tcp_pack_buff(struct tcpdisbuf *tp); */
int tcp_read(struct tcp_chan *chan, long long *read_len, long long *avail_len, unsigned int timeout);
int DIS_tcp_wflush(struct tcp_chan *chan); 
/* static void DIS_tcp_clear(struct tcpdisbuf *tp); */
void DIS_tcp_reset(struct tcp_chan *chan, int i);
int tcp_rskip(struct tcp_chan *chan, size_t ct);
int tcp_getc(struct tcp_chan *chan);
int tcp_gets(struct tcp_chan *chan, char *str, size_t ct);
int tcp_puts(struct tcp_chan *chan, const char *str, size_t ct); 
int tcp_rcommit(struct tcp_chan *chan, int commit_flag); 
int tcp_wcommit(struct tcp_chan *chan, int commit_flag);
int lock_all_channels();
int unlock_all_channels(); 
struct tcp_chan * DIS_tcp_setup(int fd);
void DIS_tcp_cleanup(struct tcp_chan *chan);


