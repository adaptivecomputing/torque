#include <license_pbs.h> /* See here for the software license */
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
char *disrcs(int stream, int rpp, size_t *nchars, int *retval);
double disrd(int stream, int rpp, int *retval);
float disrf(int stream, int rpp, int *retval);
int disrfcs(int stream,int rpp,size_t *nchars,size_t achars, char *value);
int disrfst(int stream, int rpp, size_t achars, char *value);
dis_long_double_t disrl(int stream, int rpp, int *retval);
int disrl_(int stream, int rpp, dis_long_double_t *ldval, unsigned *ndigs,
    unsigned *nskips, unsigned sigd, unsigned count);
signed char disrsc( int stream, int rpp, int *retval);
int disrsi(int stream, int rpp, int *retval);
int disrsi_(int stream, int rpp, int *negate, unsigned *value, unsigned count);
long disrsl(int stream, int rpp, int *retval);
int disrsl_(int stream, int rpp, int *negate, unsigned long *value,
    unsigned long count);
short disrss(int stream, int rpp, int *retval);
char *disrst(int stream, int rpp, int *retval);
unsigned char disruc(int stream, int rpp, int *retval);
unsigned disrui(int stream, int rpp, int *retval);
unsigned long disrul(int stream, int rpp, int *retval);
unsigned short disrus(int stream, int rpp, int *retval);
int diswcs(int stream, int rpp, const char *value, size_t nchars);
int diswf(int stream, int rpp, double value);
int diswl_(int stream, int rpp, dis_long_double_t value, unsigned ndigs);
int diswsi(int stream, int rpp, int value);
int diswsl(int stream, int rpp, long value);
int diswui(int stream, int rpp, unsigned value);
int diswui_(int stream, int rpp, unsigned value);
int diswul(int stream, int rpp, unsigned long value);

extern unsigned dis_dmx10;
extern double *dis_dp10;
extern double *dis_dn10;

extern unsigned dis_lmx10;
extern dis_long_double_t *dis_lp10;
extern dis_long_double_t *dis_ln10;

/*extern char dis_buffer[DIS_BUFSIZ];*/
extern char *dis_umax;
extern unsigned dis_umaxd;
