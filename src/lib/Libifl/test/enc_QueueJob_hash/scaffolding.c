#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"
#include "attribute.h" /* attropl */
#include "u_hash_map_structs.h" /* job_data */

int diswcs(tcp_chan *chan, const char *value, size_t nchars)
 {
 fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
 exit(1);
 }

int encode_DIS_attropl_hash(tcp_chan *chan, memmgr **mm, job_data *job_attr, job_data *res_attr)
 {
 fprintf(stderr, "The call to encode_DIS_attropl needs to be mocked!!\n");
 exit(1);
 }


