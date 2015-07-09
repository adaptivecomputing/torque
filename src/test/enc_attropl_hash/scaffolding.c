#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"
#include "u_hash_map_structs.h" /* job_data */

bool exit_called = false;

int diswcs(tcp_chan *chan, const char *value, size_t nchars)
 {
 fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
 exit(1);
 }

int diswui(tcp_chan *chan, unsigned value)
 {
 fprintf(stderr, "The call to diswui needs to be mocked!!\n");
 exit(1);
 }

int hash_count(job_data_container *head)
 {
 fprintf(stderr, "The call to hash_count needs to be mocked!!\n");
 exit(1);
 }

int hash_del_item(job_data_container *head, const char *name)
 {
 fprintf(stderr, "The call to hash_del_item needs to be mocked!!\n");
 exit(1);
 }

