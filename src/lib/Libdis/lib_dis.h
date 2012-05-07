#include "license_pbs.h" /* See here for the software license */
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>

#include "dis.h"
#include "dis_init.h"
#include "tcp.h" /* tcp_chan */

#define DIS_BUFSIZ (CHAR_BIT * sizeof(ULONG_MAX))

char *discui_(char *cp, unsigned value, unsigned *ndigs);
char *discul_(char *cp, unsigned long value, unsigned *ndigs);
void disi10d_();
void disi10l_();
void disiui_(void);
double disp10d_(int expon);
dis_long_double_t disp10l_(int expon);
char *disrcs(struct tcp_chan *chan, size_t *nchars, int *retval);
/* double disrd(struct_tcp_chan *chan, int *retval); */
/* float disrf(struct tcp_chan *chan, int *retval); */
int disrfcs(struct tcp_chan *chan, size_t *nchars,size_t achars, char *value);
int disrfst(struct tcp_chan *chan, size_t achars, char *value);
dis_long_double_t disrl(struct tcp_chan *chan, int *retval);
int disrl_(struct tcp_chan *chan, dis_long_double_t *ldval, unsigned *ndigs,
    unsigned *nskips, unsigned sigd, unsigned count);
/* signed char disrsc(struct tcp_chan *chan, int *retval); */
/* int disrsi(struct tcp_chan *chan, int *retval); */
int disrsi_(struct tcp_chan *chan, int *negate, unsigned *value, unsigned count);
long disrsl(struct tcp_chan *chan, int *retval);
int disrsl_(struct tcp_chan *chan, int *negate, unsigned long *value,
    unsigned long count);
/* short disrss(struct tcp_chan *chan, int *retval); */
char *disrst(struct tcp_chan *chan, int *retval);
/* unsigned char disruc(struct tcp_chan *chan, int *retval); */
/* unsigned disrui(struct tcp_chan *chan, int *retval); */
unsigned long disrul(struct tcp_chan *chan, int *retval);
/* unsigned short disrus(struct tcp_chan *chan, int *retval); */
int diswcs(struct tcp_chan *chan, const char *value, size_t nchars);
/* int diswf(struct tcp_chan *chan, double value); */
int diswl_(struct tcp_chan *chan, dis_long_double_t value, unsigned ndigs);
/* int diswsi(struct tcp_chan *chan, int value); */
int diswsl(struct tcp_chan *chan, long value);
/* int diswui(struct tcp_chan *chan, unsigned value); */
int diswui_(struct tcp_chan *chan, unsigned value);
int diswul(struct tcp_chan *chan, unsigned long value);

extern unsigned dis_dmx10;
extern double *dis_dp10;
extern double *dis_dn10;

extern unsigned dis_lmx10;
extern dis_long_double_t *dis_lp10;
extern dis_long_double_t *dis_ln10;

/*extern char dis_buffer[DIS_BUFSIZ];*/
extern char *dis_umax;
extern unsigned dis_umaxd;
