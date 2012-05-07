#ifndef _MOM_SERVER_H
#define _MOM_SERVER_H
#include "license_pbs.h" /* See here for the software license */

#include "mom_server.h" /* mom_server */
#include "pbs_job.h" /* job */
#include "mom_hierarchy.h" /* node_comm_t */
#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "tcp.h" /* tcp_chan */
#if defined(NVIDIA_GPUS) && defined(NVML_API)
#include "nvml.h"
#endif  /* NVIDIA_GPUS and NVML_API */

void mom_server_init(mom_server *pms);

void mom_server_all_init(void);

mom_server *mom_server_find_by_name(char *name);

mom_server *mom_server_find_by_ip(u_long search_ipaddr);

mom_server *mom_server_find_empty_slot(void);

int mom_server_add(char *value);

void mom_server_stream_error(int socket, char *name, const char *id, char *message);

int mom_server_flush_io(struct tcp_chan *chan, const char *id, char *message);

int is_compose(struct tcp_chan *chan, char *server_name, int command);

void gen_size(char *name, char **BPtr, int *BSpace);

void gen_arch(char *name, char **BPtr, int *BSpace);

void gen_opsys(char *name, char **BPtr, int *BSpace);

void gen_jdata(char *name, char **BPtr, int *BSpace);

void gen_gres(char *name, char **BPtr, int *BSpace);

void gen_gen(char *name, char **BPtr, int *BSpace);

#if defined(NVIDIA_GPUS) && defined(NVML_API)
void log_nvml_error(nvmlReturn_t rc, char* gpuid, const char* id);

int init_nvidia_nvml();

int shut_nvidia_nvml();

nvmlDevice_t get_nvml_device_handle(char *gpuid);
#endif /* NVIDIA_GPUS and NVML_API */

#ifdef NVIDIA_GPUS
/* static int check_nvidia_path(); */

/* static char *gpus(char *buffer, int buffer_size); */

/* static int gpumodes(int buffer[], int buffer_size); */

int setgpumode(char *gpuid, int gpumode);

int resetgpuecc(char *gpuid, int reset_perm, int reset_vol);

int setup_gpus_for_job(job *pjob);

void generate_server_gpustatus_smi(dynamic_string *gpu_status);

void mom_server_update_gpustat(mom_server *pms, char *status_strings);
#endif /* NVIDIA_GPUS */

int generate_server_status(char *buffer, int buffer_size);

#ifdef NVML_API
void generate_server_gpustatus_nvml(dynamic_string *gpu_status);
#endif /* NVML_API */

int write_update_header(struct tcp_chan *chan, const char *id, char *name);

int write_my_server_status(struct tcp_chan *chan, const char *id, char *status_strings, void *dest, int mode);

int write_cached_statuses(struct tcp_chan *chan, const char *id, void *dest, int mode);

void mom_server_update_stat(mom_server *pms, char *status_strings);

void node_comm_error(node_comm_t *nc, char *message);

int write_status_strings(char *stat_str, node_comm_t *nc);

int send_update();

void mom_server_all_update_stat(void);

long power(register int x, register int n);

int calculate_retry_seconds(int count);

void mom_server_diag(mom_server *pms, int sindex, char **BPtr, int *BSpace);

void mom_server_all_diag(char **BPtr, int *BSpace);

void mom_server_update_receive_time(int stream, char *command_name);

void mom_server_update_receive_time_by_ip(u_long ipaddr, char *command_name);

mom_server *mom_server_valid_message_source(struct tcp_chan *chan, char **err_msg);

void pass_along_hellos(int hello_count);

void mom_is_request(struct tcp_chan *chan, int version, int *cmdp);

float compute_load_threshold(char *config, int numvnodes, float threshold);

void check_busy(double mla);

void check_state(int Force);

void state_to_server(int ServerIndex, int force);
void shutdown_to_server(int ServerIndex);

void mom_server_all_send_state(void);

int mom_open_socket_to_jobs_server(job *pjob, const char *caller_id, void *(*message_handler)(void *));

void clear_down_mom_servers(void);

int is_mom_server_down(pbs_net_t server_address);

int no_mom_servers_down(void);

void set_mom_server_down(pbs_net_t server_address);

#endif /* _MOM_SERVER_H */
