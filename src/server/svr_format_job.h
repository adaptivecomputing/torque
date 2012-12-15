#ifndef _SVR_FORMAT_JOB_H
#define _SVR_FORMAT_JOB_H
#include "license_pbs.h" /* See here for the software license */
#include <stdio.h> /* FILE */
#include "server.h" /* mail_info */

void svr_format_job(FILE *fh, mail_info *mi, const char *fmt);

#endif /* _SVR_FORMAT_JOB_H */
