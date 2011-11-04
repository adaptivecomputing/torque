#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle */
#include "attribute.h" /* attropl */
#include "u_memmgr.h" /* memmgr */

int pbs_errno = 0;
struct connect_handle connection[10];

int PBSD_jscript(int c, char *script_file, char *jobid)
 {
 fprintf(stderr, "The call to PBSD_jscript needs to be mocked!!\n");
 exit(1);
 }

int PBSD_commit(int connect, char *jobid)
 {
 fprintf(stderr, "The call to PBSD_commit needs to be mocked!!\n");
 exit(1);
 }

int PBSD_rdytocmt(int connect, char *jobid)
 {
 fprintf(stderr, "The call to PBSD_rdytocmt needs to be mocked!!\n");
 exit(1);
 }

int PBSD_QueueJob_hash(int connect, char *jobid, char *destin, memmgr **mm, job_data *job_attr, job_data *res_attr, char *extend, char **job_id, char **msg)
 {
 fprintf(stderr, "The call to PBSD_QueueJob_hash needs to be mocked!!\n");
 exit(1);
 }

char *memmgr_strdup(memmgr **mgr, char *value, int *size)
 {
 fprintf(stderr, "The call to memmgr_strdup needs to be mocked!!\n");
 exit(1);
 }

