#include "license_pbs.h" /* See here for the software license */

/* PBSD_gpuctrl2.c */
int PBSD_gpu_put(int c, char *node, char *gpuid, int gpumode, int reset_perm, int reset_vol, char *extend);

/* PBSD_gpuctrl2.c */
int PBSD_gpu_put(int c, char *node, char *gpuid, int gpumode, int reset_perm, int reset_vol, char *extend);

/* PBSD_manage2.c */
int PBSD_mgr_put(int c, int function, int command, int objtype, char *objname, struct attropl *aoplp, char *extend); 

/* PBSD_manager_caps.c */
int PBSD_manager(int c, int function, int command, int objtype, char *objname, struct attropl *aoplp, char *extend);

/* PBSD_msg2.c */
int PBSD_msg_put(int c, char *jobid, int fileopt, char *msg, char *extend);

/* PBSD_rdrpy.c */
struct batch_reply *PBSD_rdrpy(int c); 
void PBSD_FreeReply(struct batch_reply *reply);

/* PBSD_sig2.c */
int PBSD_sig_put(int c, char *jobid, char *signal, char *extend);
int PBSD_async_sig_put(int c, char *jobid, char *signal, char *extend);

/* PBSD_status.c */
struct batch_status *PBSD_status(int c, int function, char *id, struct attrl *attrib, char *extend); 
struct batch_status *PBSD_status_get(int c); 
/* static struct batch_status * alloc_bs(void); */

/* PBSD_status2.c */
int PBSD_status_put(int c, int function, char *id, struct attrl *attrib, char *extend);

/* PBSD_submit_caps.c */
int PBSD_rdytocmt(int connect, char *jobid);
int PBSD_commit_get_sid(int connect, long *sid, char *jobid); 
int PBSD_commit(int connect, char *jobid); 
/* static int PBSD_scbuf(int c, int reqtype, int seq, char *buf, int len, char *jobid, enum job_file which);  */
int PBSD_jscript(int c, char *script_file, char *jobid);
int PBSD_jobfile(int c, int req_type, char *path, char *jobid, enum job_file which);
char *PBSD_queuejob(int connect, char *jobid, char *destin, struct attropl *attrib, char *extend);
char *PBSD_QueueJob_hash(int connect, char *jobid, char *destin, memmgr **mm, job_data *job_attr, job_data *res_attr, char *extend);

/* PBSD_submit_caps.c */
int PBSD_rdytocmt(int connect, char *jobid);
int PBSD_commit_get_sid(int connect, long *sid, char *jobid); 
int PBSD_commit(int connect, char *jobid); 
/* static int PBSD_scbuf(int c, int reqtype, int seq, char *buf, int len, char *jobid, enum job_file which);  */
int PBSD_jscript(int c, char *script_file, char *jobid);
int PBSD_jobfile(int c, int req_type, char *path, char *jobid, enum job_file which);
char *PBSD_queuejob(int connect, char *jobid, char *destin, struct attropl *attrib, char *extend);
char *PBSD_QueueJob_hash(int connect, char *jobid, char *destin, memmgr **mm, job_data *job_attr, job_data *res_attr, char *extend);

/* PBS_attr.c */
int PBS_val_al(struct attrl *alp);
void PBS_free_al(struct attrl *alp);
int PBS_val_aopl(struct attropl *aoplp);
void PBS_free_aopl(struct attropl *aoplp);

/* PBS_data.c */

/* dec_Authen.c */
int decode_DIS_Authen(int sock, struct batch_request *preq);
int decode_DIS_AltAuthen(int sock, struct batch_request *preq);

/* dec_CpyFil.c */
int decode_DIS_CopyFiles(int sock, struct batch_request *preq);

/* dec_Gpu.c */
int decode_DIS_GpuCtrl(int sock, struct batch_request *preq);

/* dec_JobCred.c */
int decode_DIS_JobCred(int sock, struct batch_request *preq);

/* dec_JobFile.c */
int decode_DIS_JobFile(int sock, struct batch_request *preq);

/* dec_JobId.c */
int decode_DIS_JobId(int sock, char *jobid);

/* dec_JobObit.c */
int decode_DIS_JobObit(int sock, struct batch_request *preq); 

/* dec_Manage.c */
int decode_DIS_Manage(int sock, struct batch_request *preq);

/* dec_MoveJob.c */
int decode_DIS_MoveJob(int sock, struct batch_request *preq);

/* dec_MsgJob.c */
int decode_DIS_MessageJob(int sock, struct batch_request *preq);

/* dec_QueueJob.c */
int decode_DIS_QueueJob(int sock, struct batch_request *preq);

/* dec_Reg.c */
int decode_DIS_Register(int sock, struct batch_request *preq);

/* dec_ReqExt.c */
int decode_DIS_ReqExtend(int sock, struct batch_request *preq);

/* dec_ReqHdr.c */
int decode_DIS_ReqHdr(int sock, struct batch_request *preq, int *proto_type, int *proto_ver);

/* dec_Resc.c */
int decode_DIS_Rescl(int sock, struct batch_request *preq);

/* dec_ReturnFile.c */
int decode_DIS_ReturnFiles(int sock, struct batch_request *preq);

/* dec_RunJob.c */
int decode_DIS_RunJob(int sock, struct batch_request *preq);

/* dec_Shut.c */
int decode_DIS_ShutDown(int sock, struct batch_request *preq);

/* dec_Sig.c */
int decode_DIS_SignalJob(int sock, struct batch_request *preq);

/* dec_Status.c */
int decode_DIS_Status(int sock, struct batch_request *preq);

/* dec_Track.c */
int decode_DIS_TrackJob(int sock, struct batch_request *preq);

/* dec_attrl.c */
int decode_DIS_attrl(int sock, struct attrl **ppatt);

/* dec_attropl.c */
int decode_DIS_attropl(int sock, struct attropl **ppatt);

/* dec_rpyc.c */
int decode_DIS_replyCmd(int sock, struct batch_reply *reply);

/* dec_rpys.c */
int decode_DIS_replySvr(int sock, struct batch_reply *reply); 

/* dec_svrattrl.c */
int decode_DIS_svrattrl(int sock, tlist_head *phead);

/* enc_CpyFil.c */
int encode_DIS_CopyFiles(int sock, struct batch_request *preq);

/* enc_Gpu.c */
int encode_DIS_GpuCtrl(int sock, char *node, char *gpuid, int gpumode, int reset_perm, int reset_vol);

/* enc_JobCred.c */
int encode_DIS_JobCred(int sock, int type, char *cred, int len);

/* enc_JobFile.c */
int encode_DIS_JobFile(int sock, int seq, char *buf, int len, char *jobid, int which);

/* enc_JobId.c */
int encode_DIS_JobId(int sock, char *jobid);

/* enc_JobObit.c */
int encode_DIS_JobObit(int sock, struct batch_request *preq); 

/* enc_Manage.c */
int encode_DIS_Manage(int sock, int command, int objtype, char *objname, struct attropl *aoplp);

/* enc_MoveJob.c */
int encode_DIS_MoveJob(int sock, char *jobid, char *destin); 

/* enc_MsgJob.c */
int encode_DIS_MessageJob(int sock, char *jobid, int fileopt, char *msg);

/* enc_QueueJob.c */
int encode_DIS_QueueJob(int sock, char *jobid, char *destin, struct attropl *aoplp);

/* enc_Reg.c */
int encode_DIS_Register(int sock, struct batch_request *preq);

/* enc_ReqExt.c */
int encode_DIS_ReqExtend(int sock, char *extend);

/* enc_ReqHdr.c */
int encode_DIS_ReqHdr(int sock, int reqt, char *user);

/* enc_ReturnFile.c */
int encode_DIS_ReturnFiles(int sock, struct batch_request *preq);

/* enc_RunJob.c */
int encode_DIS_RunJob(int sock, char *jobid, char *where, unsigned int resch); 

/* enc_Shut.c */
int encode_DIS_ShutDown(int sock, int manner); 

/* enc_Sig.c */
int encode_DIS_SignalJob(int sock, char *jobid, char *signal);

/* enc_Status.c */
int encode_DIS_Status(int sock, char *objid, struct attrl *pattrl);

/* enc_Track.c */
int encode_DIS_TrackJob(int sock, struct batch_request *preq);

/* enc_attrl.c */
int encode_DIS_attrl(int sock, struct attrl *pattrl);

/* enc_attropl.c */
int encode_DIS_attropl(int sock, struct attropl *pattropl);

/* enc_reply.c */
int encode_DIS_reply(int sock, struct batch_reply *reply);

/* enc_svrattrl.c */
int encode_DIS_svrattrl(int sock, svrattrl *psattl);

/* get_svrport.c */
unsigned int get_svrport(char *service_name, char *ptype, unsigned int pdefault); 

/* list_link.c */
void insert_link(struct list_link *old, struct list_link *new, void *pobj, int position); 
void append_link(tlist_head *head, list_link *new, void *pobj); 
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
ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count); 
ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count);
ssize_t read_blocking_socket(int fd, void *buf, ssize_t count);

/* pbsD_alterjo.c */
int pbs_alterjob_asyncflag(int c, char *jobid, struct attrl *attrib, char *extend, int asyncFlag); 
int pbs_alterjob_async(int c, char *jobid, struct attrl *attrib, char *extend); 
int pbs_alterjob(int c, char *jobid, struct attrl *attrib, char *extend); 

/* pbsD_asyrun.c */
int pbs_asyrunjob(int c, char *jobid, char *location, char *extend);

/* pbsD_chkptjob.c */
int pbs_checkpointjob(int c, char *jobid, char *extend);

/* pbsD_connect.c */
void empty_alarm_handler(int signo);
char *pbs_get_server_list(void);
char *pbs_default(void);
char *pbs_fbserver(void);
/* static char *PBS_get_server(char *server, unsigned int *port);  */
#ifdef MUNGE_AUTH
int PBSD_munge_authenticate(int psock, int handle); 
#endif 
#ifndef MUNGE_AUTH
int PBSD_authenticate(int psock); 
#endif 
#ifdef ENABLE_UNIX_SOCKETS
ssize_t send_unix_creds(int sd);
#endif 
int pbs_original_connect(char *server); 
int pbs_disconnect(int connect); 
int pbs_connect(char *server_name_ptr); 
int pbs_connect_with_retry(char *server_name_ptr, int retry_seconds); 
int pbs_query_max_connections(void);
void initialize_connections_table();

/* pbsD_deljob.c */
int pbs_deljob(int c, char *jobid, char *extend);

/* pbsD_deljob.c */
int pbs_deljob(int c, char *jobid, char *extend); 

/* pbsD_gpuctrl.c */
int pbs_gpumode(int c, char *node, char *gpuid, int gpumode);
int pbs_gpureset(int c, char *node, char *gpuid, int permanent, int vol);

/* pbsD_holdjob.c */
int pbs_holdjob(int c, char *jobid, char *holdtype, char *extend);

/* pbsD_locjob.c */
char * pbs_locjob(int c, char *jobid, char *extend);

/* pbsD_manager.c */
int pbs_manager(int c, int command, int objtype, char *objname, struct attropl *attrib, char *extend);

/* pbsD_movejob.c */
int pbs_movejob(int c, char *jobid, char *destin, char *extend); 

/* pbsD_msgjob.c */
int pbs_msgjob(int c, char *jobid, int fileopt, char *msg, char *extend);

/* pbsD_orderjo.c */
int pbs_orderjob(int c, char *job1, char *job2, char *extend);

/* pbsD_rerunjo.c */
int pbs_rerunjob(int c, char *jobid, char *extend);

/* pbsD_resc.c */
/* static int encode_DIS_Resc(int sock, char **rlist, int ct, resource_t rh); */
/* static int PBS_resc(int c, int reqtype, char **rescl, int ct, resource_t rh); */
int pbs_rescquery(int c, char **resclist, int num_resc, int *available, int *allocated, int *reserved, int *down); 
int pbs_rescreserve(int c, char **rl, int num_resc, resource_t *prh); 
int pbs_rescrelease(int c, resource_t rh);
int totpool(int con, int update); 
int usepool(int con, int update);
char *avail(int con, char *resc);

/* pbsD_rlsjob.c */
int pbs_rlsjob(int c, char *jobid, char *holdtype, char *extend);

/* pbsD_runjob.c */
int pbs_runjob(int c, char *jobid, char *location, char *extend);

/* pbsD_selectj.c */
char ** pbs_selectjob(int c, struct attropl *attrib, char *extend);
struct batch_status * pbs_selstat(int c, struct attropl *attrib, char *extend);
/* static int PBSD_select_put(int c, int type, struct attropl *attrib, char *extend); */
/* static char **PBSD_select_get(int c); */

/* pbsD_sigjob.c */
int pbs_sigjob(int c, char *jobid, char *signal, char *extend);
int pbs_sigjobasync(int c, char *jobid, char *signal, char *extend);

/* pbsD_stagein.c */
int pbs_stagein(int c, char *jobid, char *location, char *extend);

/* pbsD_statjob.c */
struct batch_status *pbs_statjob(int c, char *id, struct attrl *attrib, char *extend); 

/* pbsD_statnode.c */
struct batch_status *pbs_statnode(int c, char *id, struct attrl *attrib, char *extend); 

/* pbsD_statque.c */
struct batch_status *pbs_statque(int c, char *id, struct attrl *attrib, char *extend); 

/* pbsD_statsrv.c */
struct batch_status *pbs_statserver(int c, struct attrl *attrib, char *extend); 

/* pbsD_submit.c */
char *pbs_submit(int c, struct attropl *attrib, char *script, char *destination, char *extend); 

/* pbsD_termin.c */
int pbs_terminate(int c, int manner, char *extend); 

/* pbs_geterrmg.c */
char *pbs_geterrmsg(int connect); 

/* pbs_statfree.c */
void pbs_statfree(struct batch_status *bsp);

/* rpp.c */
void blog_init(char *s, int len);
int blog_write(char *s);
void blog_out(char *filename);
/* static int next_seq(int *seq); */
char * netaddr(struct sockaddr_in *ap);
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
void rpp_terminate(void);
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
int DIS_tcp_istimeout(int sock);
/* static void tcp_pack_buff(struct tcpdisbuf *tp); */
int tcp_read(int fd); 
int DIS_tcp_wflush(int fd); 
/* static void DIS_tcp_clear(struct tcpdisbuf *tp); */
void DIS_tcp_reset(int fd, int i);
int tcp_rskip(int fd, size_t ct);
int tcp_getc(int fd);
int tcp_gets(int fd, char *str, size_t ct);
int PConnTimeout(int sock); 
int TConnGetReadErrno(int sock); 
int TConnGetSelectErrno(int sock); 
int tcp_puts(int fd, const char *str, size_t ct); 
int tcp_rcommit(int fd, int commit_flag); 
int tcp_wcommit(int fd, int commit_flag);
int lock_all_channels();
int unlock_all_channels(); 
int resize_tcp_array_if_needed(int fd);
void DIS_tcp_setup(int fd);

/* tm.c */
/* static event_info * find_event(tm_event_t x); */
/* static void del_event(event_info *ep); */
/* static tm_event_t new_event(void); */
/* static void add_event(tm_event_t event, tm_node_id node, int type, void *info); */
/* static task_info * find_task(tm_task_id x); */
/* static tm_task_id new_task(char *jobid, tm_node_id node, tm_task_id task); */
/* static int localmom(void); */
/* static int startcom(int com, tm_event_t event); */
int tm_init(void *info, struct tm_roots *roots); 
int tm_nodeinfo(tm_node_id **list, int *nnodes);
int tm_spawn(int argc, char **argv, char **envp, tm_node_id where, tm_task_id *tid, tm_event_t *event); 
int tm_kill(tm_task_id tid, int sig, tm_event_t *event);
int tm_obit(tm_task_id tid, int *obitval, tm_event_t *event);
int tm_taskinfo(tm_node_id node, tm_task_id *tid_list, int list_size, int *ntasks, tm_event_t *event);
int tm_atnode(tm_task_id tid, tm_node_id *node);
int tm_rescinfo(tm_node_id node, char *resource, int len, tm_event_t *event);
int tm_publish(char *name, void *info, int len, tm_event_t *event);
int tm_subscribe(tm_task_id tid, char *name, void *info, int len, int *info_len, tm_event_t *event);
int tm_finalize(void);
int tm_notify(int tm_signal); 
int tm_alloc(char *resources, tm_event_t *event); 
int tm_dealloc(tm_node_id node, tm_event_t *event);
int tm_create_event(tm_event_t *event);
int tm_destroy_event(tm_event_t *event);
int tm_register(tm_whattodo_t *what, tm_event_t *event);
int tm_poll(tm_event_t poll_event, tm_event_t *result_event, int wait, int *tm_errno);
int tm_adopt(char *id, int adoptCmd, pid_t pid);
