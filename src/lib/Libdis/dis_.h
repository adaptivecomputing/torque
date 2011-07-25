#include "license_pbs.h" /* See here for the software license */
#include <limits.h>
#include <stddef.h>

#include "dis.h"
#include "dis_init.h"

#define DIS_BUFSIZ (CHAR_BIT * sizeof(ULONG_MAX))

char *discui_(char *cp, unsigned value, unsigned *ndigs);
char *discul_(char *cp, unsigned long value, unsigned *ndigs);
void disi10d_();
void disi10l_();
void disiui_(void);
double disp10d_(int expon);
dis_long_double_t disp10l_(int expon);
char *disrcs(int stream, size_t *nchars, int *retval);
double disrd(int stream, int *retval);
float disrf(int stream, int *retval);
int disrfcs(int stream, size_t *nchars,size_t achars, char *value);
int disrfst(int stream, size_t achars, char *value);
dis_long_double_t disrl(int stream, int *retval);
int disrl_(int stream, dis_long_double_t *ldval, unsigned *ndigs,
    unsigned *nskips, unsigned sigd, unsigned count);
signed char disrsc( int stream, int *retval);
int disrsi(int stream, int *retval);
int disrsi_(int stream, int *negate, unsigned *value, unsigned count);
long disrsl(int stream, int *retval);
int disrsl_(int stream, int *negate, unsigned long *value,
    unsigned long count);
short disrss(int stream, int *retval);
char *disrst(int stream, int *retval);
unsigned char disruc(int stream, int *retval);
unsigned disrui(int stream, int *retval);
unsigned long disrul(int stream, int *retval);
unsigned short disrus(int stream, int *retval);
int diswcs(int stream, const char *value, size_t nchars);
int diswf(int stream, double value);
int diswl_(int stream, dis_long_double_t value, unsigned ndigs);
int diswsi(int stream, int value);
int diswsl(int stream, long value);
int diswui(int stream, unsigned value);
int diswui_(int stream, unsigned value);
int diswul(int stream, unsigned long value);

extern unsigned dis_dmx10;
extern double *dis_dp10;
extern double *dis_dn10;

extern unsigned dis_lmx10;
extern dis_long_double_t *dis_lp10;
extern dis_long_double_t *dis_ln10;

/*extern char dis_buffer[DIS_BUFSIZ];*/
extern char *dis_umax;
extern unsigned dis_umaxd;
