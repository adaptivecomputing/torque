#ifndef _PBS_DEMUX_H
#define _PBS_DEMUX_H
#include "license_pbs.h" /* See here for the software license */


enum rwhere {invalid, new_out, new_err, old_out, old_err};

struct routem
  {
  enum rwhere r_where;
  short  r_nl;
  };

void readit(int sock, struct routem *prm);

/* #ifdef ENABLE_BLCR */
/* static int demux_callback(void* arg); */
/* #endif ENABLE_BLCR */

/* int main(int argc, char *argv[]); */


#endif /* PBS_DEMUX_H */
