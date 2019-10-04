#ifndef _SVR_MAIL_H
#define _SVR_MAIL_H
#include "license_pbs.h" /* See here for the software license */

#include "server.h" /* mail_info */
#include "pbs_job.h" /* job */

void svr_mailowner(job *pjob, int mailpoint, int force, const char *text, boost::shared_ptr<mutex_mgr>& job_mutex);

#endif /* _SVR_MAIL_H */
