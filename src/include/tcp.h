#include "license_pbs.h" /* See here for the software license */
#ifndef TCP_PBS_H
#define TCP_PBS_H

struct tcpdisbuf
  {
  unsigned long tdis_bufsize;
  char *tdis_leadp;
  char *tdis_trailp;
  char *tdis_eod;
  char  *tdis_thebuf;
  };

struct tcp_chan
  {

  struct tcpdisbuf readbuf;

  struct tcpdisbuf writebuf;

  int              IsTimeout;  /* (boolean)  1 - true */
  int              ReadErrno;
  int              SelectErrno;
  int              sock;
  };


int tcp_getc(struct tcp_chan *chan);
int tcp_gets(struct tcp_chan *chan, char *, size_t);
int tcp_puts(struct tcp_chan *chan, const char *, size_t);
int tcp_rcommit(struct tcp_chan *chan, int);
int tcp_wcommit(struct tcp_chan *chan, int);
int tcp_rskip(struct tcp_chan *chan,size_t);
int tcp_chan_has_data(struct tcp_chan *chan);

#endif /* TCP_PBS_H */
