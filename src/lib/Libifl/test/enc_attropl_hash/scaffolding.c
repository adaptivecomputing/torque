#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "u_memmgr.h" /* memmgr */
#include "u_hash_map_structs.h" /* job_data */

int diswcs(int stream, const char *value, size_t nchars)
 {
 fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
 exit(1);
 }

int diswui(int stream, unsigned value)
 {
 fprintf(stderr, "The call to diswui needs to be mocked!!\n");
 exit(1);
 }

void memmgr_destroy(memmgr **mgr)
 {
 fprintf(stderr, "The call to memmgr_destroy needs to be mocked!!\n");
 exit(1);
 }

int hash_count(job_data *head)
 {
 fprintf(stderr, "The call to hash_count needs to be mocked!!\n");
 exit(1);
 }

int hash_del_item(memmgr **mm, job_data **head, char *name)
 {
 fprintf(stderr, "The call to hash_del_item needs to be mocked!!\n");
 exit(1);
 }

void *memmgr_realloc(memmgr **mgr, void *ptr, int new_size)
 {
 fprintf(stderr, "The call to memmgr_realloc needs to be mocked!!\n");
 exit(1);
 }

int memmgr_init(memmgr **mgr, int mgr_size) 
 {
 fprintf(stderr, "The call to memmgr_init needs to be mocked!!\n");
 exit(1);
 }
