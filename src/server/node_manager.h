#ifndef _NODE_MANAGER_H
#define _NODE_MANAGER_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job, pbsnodes, prop */
#include "work_task.h" /* work_task */
#include "pbs_ifl.h" /* resource_t */
#include "pbs_nodes.h" /* howl, gpusubn */
#include <string>
#include <vector>

void update_node_state(struct pbsnode *np, int newstate);

int check_node_for_job(struct pbsnode *pnode, char *jobid);

job *find_job_by_node(struct pbsnode *pnode, char *jobid);

void update_job_data(struct pbsnode *np, char *jobstring_in);

void setup_notification(char *pname);

int is_stat_get(const char *node_name, struct tcp_chan *chan);

int is_compose(struct tcp_chan *chan, int command);

void stream_eof(int stream, u_long addr, uint16_t port, int ret);

void *check_nodes_work(void *vp);

void check_nodes(struct work_task *ptask);

int svr_is_request(struct tcp_chan *chan, int version, long *args);

void *write_node_state_work(void *vp);

void write_node_state(void);

void *write_node_power_state_work(void *vp);

void write_node_power_state(void);

int write_node_note(void);

void *node_unreserve_work(void *vp);

void node_unreserve(resource_t handle);

int search_acceptable(struct pbsnode *pnode, struct prop *glorf, int skip, int vpreq, int gpureq);

int can_reshuffle(struct pbsnode *pnode, struct prop *glorf, int skip, int vpreq, int gpureq, int pass);

int procs_available(int proc_ct);

#ifdef GEOMETRY_REQUESTS
int get_bitmap(job *pjob, int ProcBMSize, char *ProcBMPtr);

int node_satisfies_request(struct pbsnode *pnode, char *ProcBMStr);
#endif /* GEOMETRY_REQUESTS */

int add_job_to_gpu_subnode(struct pbsnode *pnode, struct gpusubn *gn, job *pjob);

int set_nodes(job *pjob, char *spec, int procs, char **rtnlist, char **rtnportlist, char *FailHost, char *EMsg);

int procs_requested(char *spec);

int node_avail_complex(char *spec, int *navail, int *nalloc, int *nresvd, int *ndown);

int node_avail(char *spec, int *navail, int *nalloc, int *nresvd, int *ndown);

int node_reserve(char *nspec, resource_t tag);

void sync_node_jobs_with_moms(struct pbsnode *np, std::vector<std::string> &job_list);

#endif /* _NODE_MANAGER_H */
