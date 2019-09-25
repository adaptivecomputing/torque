#ifndef _ACCOUNTING_H
#define _ACCOUNTING_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */

/* static char *acct_job(job *pjob, char **Buf, unsigned int *BufSize); */

void account_record(int acctype, job *pjob, const char *text, boost::shared_ptr<mutex_mgr>& job_mutex);

void account_jobstr(job *pjob, boost::shared_ptr<mutex_mgr>& job_mutex);

void account_jobend(job *pjob, char *used, boost::shared_ptr<mutex_mgr>& job_mutex);

void acct_cleanup(long days_to_keep);

#endif /* _ACCOUNTING_H */
