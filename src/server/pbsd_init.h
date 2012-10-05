#ifndef _PBSD_INIT_H
#define _PBSD_INIT_H
#include "license_pbs.h" /* See here for the software license */

/*
 * dynamic array, with utility functions for easy appending
 */

typedef struct darray_t
  {
  int Length;
  void **Data;
  int AppendIndex;
  } darray_t;

int DArrayInit(darray_t *Array, int InitialSize);

int DArrayFree(darray_t *Array);

int DArrayAppend(darray_t *Array, void *Item);

/* static int SortPrioAscend(const void *A, const void *B); */

void update_default_np();

void add_server_names_to_acl_hosts(void);

int pbsd_init(int type);

/* static char *build_path(char *parent, char *name, char *suffix); */

/* static int pbsd_init_job(job *pjob, int type); */

/* static void pbsd_init_reque(job *pjob, int change_state); */

/* static void catch_abort(int sig); */

/* static void change_logs(int sig); */

/* static void change_log_level(int sig); */

/* static void stop_me(int sig); */

/* static int chk_save_file(char *filename); */

/* static void resume_net_move(struct work_task *ptask); */

/* static void rm_files(char *dirname); */

/* static void init_abt_job(job *pjob); */

int recov_svr_attr(int type);

#endif /* _PBSD_INIT_H */
