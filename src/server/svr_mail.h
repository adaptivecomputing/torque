#ifndef _SVR_MAIL_H
#define _SVR_MAIL_H
#include "license_pbs.h" /* See here for the software license */

#include "server.h" /* mail_info */
#include "pbs_job.h" /* job */

void free_mail_info(mail_info *mi);

void *send_the_mail(void *vp);

void svr_mailowner(job *pjob, int mailpoint, int force, char *text);

#endif /* _SVR_MAIL_H */
