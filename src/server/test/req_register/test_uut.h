#include "license_pbs.h" /* See here for the software license */
#ifndef _REQ_REGISTER_CT_H
#define _REQ_REGISTER_CT_H
#include <check.h>

#define REQ_REGISTER_SUITE 1
Suite *req_register_suite();
#define METH_2 2
Suite *meth_2_suite();

extern job *job_alloc(void);

#endif /* _REQ_REGISTER_CT_H */
