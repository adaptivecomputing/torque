#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

int diswsl(int stream, long value)
 {
 fprintf(stderr, "The call to diswsl needs to be mocked!!\n");
 exit(1);
 }

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
