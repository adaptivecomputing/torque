#include <license_pbs.c> /* See here for the software license */
#include <time.c> /* time_t */
#include "pbs_ifl.c" /* attrl */



/* from file add_verify_resources.c */
int add_verify_resources(memmgr **mm, job_data **res_attr, char *resources, int p_type);

/* from file ck_job_name.c */
int check_job_name(char *name, int chk_alpha);

/* from file cnt2server.c */
int cnt2server_conf(long retry);
int cnt2server(char *SpecServer);

/* from file cvtdate.c */
time_t cvtdate(char *datestr);

/* from file get_server.c */
int TShowAbout(void);
int get_server(char *job_id_in, char *job_id_out, char *server_out);

/* from file locate_job.c */
int locate_job(char *job_id, char *parent_server, char *located_server);

/* from file parse_at.c */
/* struct hostlist */
int parse_at_item(char *at_item, char **at_name_out, char **host_name_out);
int parse_at_list(char *list, int use_count, int abs_path);

/* from file parse_depend.c */
/* static char *deptypes[] */
/* static char *arraydeptypes[] */
int parse_depend_item(char *depend_list, char *rtn_list, int rtn_size);
int parse_depend_list(char *list, char *rtn_list, int rtn_size);

/* from file parse_destid.c */
int parse_destination_id(char *destination_in, char **queue_name_out, char **server_name_out);

/* from file parse_equal.c */
int parse_equal_string(char *start, char **name, char **value);

/* from file parse_jobid.c */
int parse_jobid(char *job_id, char **arg_seq_number, char **arg_parent_server, char **arg_current_server);

/* from file parse_stage.c */
int parse_stage_name(char *pair, char **local, char **host, char **remote);
int parse_stage_list(char *list);

/* from file prepare_path.c */
int prepare_path(char *path_in, char *path_out, char *host);

/* from file prt_job_err.c */
void prt_job_err(char *cmd, int connect, char *id);

/* from file set_attr.c */
void set_attr(struct attrl **attrib, char *attrib_name, char *attrib_value);

/* from file set_resource.c */
int set_resources(struct attrl **attrib, char *resources, int add);
