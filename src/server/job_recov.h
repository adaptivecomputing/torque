#ifndef _JOB_RECOV_H
#define _JOB_RECOV_H
#include "license_pbs.h" /* See here for the software license */
#include "job_recovery.h"



int job_save(job *pjob, int updatetype, int mom_port);

job *job_recov(const char *);

void   add_fix_fields(xmlNodePtr *rnode, const job *pjob);
void   add_union_fields(xmlNodePtr *rnode, const job *pjob);
int    saveJobToXML(job *pjob, const char *filename);

#endif /* _JOB_RECOV_H */
