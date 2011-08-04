#include "license_pbs.h" /* See here for the software license */
#ifndef _GET_SERVER_CT_H
#define _GET_SERVER_CT_H
#include <check.h>

#define PBS_DEFAULT_FILE "test_dir"

#define GET_SERVER_SUITE 1
Suite *get_server_suite();
#define METH_2 2
Suite *meth_2_suite();

#endif /* _GET_SERVER_CT_H */
