#include "license_pbs.h" /* See here for the software license */
#ifndef _U_MU_CT_H
#define _U_MU_CT_H
#include <check.h>

char *threadsafe_tokenizer(char **str, char *delims);

#define U_MU_SUITE 1
Suite *u_mu_suite();
#define METH_2 2
Suite *meth_2_suite();

#endif /* _U_MU_CT_H */
