#ifndef _NODE_MANAGER_H
#define _NODE_MANAGER_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job, pbsnodes, prop */
#include "work_task.h" /* work_task */
#include "pbs_ifl.h" /* resource_t */
#include "pbs_nodes.h" /* howl, gpusubn */

struct pbsnode *tfind_addr(const u_long key, uint16_t port, char *job_momname);

void update_node_state(struct pbsnode *np, int newstate);

int check_node_for_job(struct pbsnode *pnode, char *jobid);

job *find_job_by_node(struct pbsnode *pnode, char *jobid);

void *sync_node_jobs(void *vp);

void update_job_data(struct pbsnode *np, char *jobstring_in);

void setup_notification(char *pname);

int is_stat_get(char *node_name, struct tcp_chan *chan);

#ifdef NVIDIA_GPUS
int gpu_has_job(struct pbsnode *pnode, int gpuid);
#endif /* NVIDIA_GPUS */

int is_gpustat_get(struct pbsnode *np, char **str_ptr);

int is_compose(struct tcp_chan *chan, int command);

void stream_eof(int stream, u_long addr, uint16_t port, int ret);

void *check_nodes_work(void *vp);

void check_nodes(struct work_task *ptask);

int svr_is_request(struct tcp_chan *chan, int version);

void *write_node_state_work(void *vp);

void write_node_state(void);

int write_node_note(void);

/* static void free_prop(struct prop *prop); */

void *node_unreserve_work(void *vp);

void node_unreserve(resource_t handle);

int hasprop(struct pbsnode *pnode, struct prop *props);

/* static int hasppn(struct pbsnode *pnode, int node_req, int free); */

/* static void mark(struct pbsnode *pnode, struct prop *props); */

/* static int gpu_count(struct pbsnode *pnode, int freeonly); */

#ifdef NVIDIA_GPUS
int gpu_entry_by_id(struct pbsnode *pnode, char *gpuid, int get_empty);
#endif /* NVIDIA_GPUS */

int search_acceptable(struct pbsnode *pnode, struct prop *glorf, int skip, int vpreq, int gpureq);

int can_reshuffle(struct pbsnode *pnode, struct prop *glorf, int skip, int vpreq, int gpureq, int pass);

/* static int search(struct prop *glorf, int vpreq, int gpureq, int skip, int order, int depth); */

/* static int number(char **ptr, int *num); */

/* static int property(char **ptr, char **prop); */

/* static int proplist(char **str, struct prop **plist, int *node_req, int *gpu_req); */

/* static int listelem(char **str, int order); */

/* static char *mod_spec(char *spec, char *global); */

int MSNPrintF(char **BPtr, int *BSpace, char *Format, ...);

int procs_available(int proc_ct);

#ifdef GEOMETRY_REQUESTS
int get_bitmap(job *pjob, int ProcBMSize, char *ProcBMPtr);

int node_satisfies_request(struct pbsnode *pnode, char *ProcBMStr);

int reserve_node(struct pbsnode *pnode, short newstate, job *pjob, char *ProcBMStr, struct howl **hlistptr);
#endif /* GEOMETRY_REQUESTS */

int add_job_to_node(struct pbsnode *pnode, struct pbssubn *snp, short newstate, job *pjob, int exclusive);

int add_job_to_gpu_subnode(struct pbsnode *pnode, struct gpusubn *gn, job *pjob);

int build_host_list(struct howl **hlistptr, struct pbssubn *snp, struct pbsnode *pnode);

int add_gpu_to_hostlist(struct howl **hlistptr, struct gpusubn *gn, struct pbsnode *pnode);

int set_nodes(job *pjob, char *spec, int procs, char **rtnlist, char **rtnportlist, char *FailHost, char *EMsg);

int procs_requested(char *spec);

int node_avail_complex(char *spec, int *navail, int *nalloc, int *nresvd, int *ndown);

int node_avail(char *spec, int *navail, int *nalloc, int *nresvd, int *ndown);

int node_reserve(char *nspec, resource_t tag);

int is_ts_node(char *nodestr);

char *find_ts_node(void);

void free_nodes(job *pjob);

void set_old_nodes(job *pjob);

job *get_job_from_jobinfo(struct jobinfo *jp, struct pbsnode *pnode);

#endif /* _NODE_MANAGER_H */
