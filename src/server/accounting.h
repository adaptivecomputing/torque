#ifndef _ACCOUNTING_H
#define _ACCOUNTING_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */

/* static char *acct_job(job *pjob, char **Buf, unsigned int *BufSize); */

int acct_open(char *filename);

void acct_close(void);

void account_record(int acctype, job *pjob, char *text);

void account_jobstr(job *pjob);

void account_jobend(job *pjob, char *used);

void acct_cleanup(long days_to_keep);

 int AdjustAcctBufSize(char **Buf, unsigned int *BufSize, int newStringLen, job *pjob);

#endif /* _ACCOUNTING_H */
