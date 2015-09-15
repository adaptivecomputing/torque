#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"

int diswcs(tcp_chan *chan, const char *value, size_t nchars)
 {
 fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
 exit(1);
 }

int diswsi(tcp_chan *chan, int value)
 {
 fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
 exit(1);
 }
