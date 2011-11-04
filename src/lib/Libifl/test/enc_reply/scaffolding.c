#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "list_link.h" /* list_link */
#include "attribute.h" /* svrattrl */

int diswcs(int stream, const char *value, size_t nchars)
 {
 fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
 exit(1);
 }

void *get_next(list_link pl, char *file, int line)
 {
 fprintf(stderr, "The call to log_event needs to be mocked!!\n");
 exit(1);
 }

int diswui(int stream, unsigned value)
 {
 fprintf(stderr, "The call to log_event needs to be mocked!!\n");
 exit(1);
 }

int encode_DIS_svrattrl(int sock, svrattrl *psattl)
 {
 fprintf(stderr, "The call to log_event needs to be mocked!!\n");
 exit(1);
 }

int diswsi(int stream, int value)
 {
 fprintf(stderr, "The call to log_event needs to be mocked!!\n");
 exit(1);
 }

