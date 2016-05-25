#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle */
#include "attribute.h" /* attropl */

int pbs_errno = 0;
struct connect_handle connection[10];

int PBSD_jscript(int c, const char *script_file, const char *jobid)
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

int PBSD_QueueJob_hash(int connect, char *jobid, char *destin, job_data_container *job_attr, job_data_container *res_attr, char *extend, char **job_id, char **msg)
 {
 fprintf(stderr, "The call to PBSD_QueueJob_hash needs to be mocked!!\n");
 exit(1);
 }

