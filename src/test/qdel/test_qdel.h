#include "license_pbs.h" /* See here for the software license */
#ifndef _QDEL_CT_H
#define _QDEL_CT_H
#include <check.h>

#define QDEL_SUITE 1
Suite *qdel_suite();
#define METH_2 2
Suite *meth_2_suite();

int qdel_main(

  int    argc,
  char **argv,
  char **envp);

#endif /* _QDEL_CT_H */
