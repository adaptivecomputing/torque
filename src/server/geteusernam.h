#ifndef _GETEUSERNAM_H
#define _GETEUSERNAM_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "attribute.h" /* attribute */

/* static char *geteusernam(job *pjob, attribute *pattr); */

/* static char *getegroup(job *pjob, attribute *pattr); */
 
int set_jobexid(job *pjob, attribute *attrry, char *EMsg);

#endif /* _GETEUSERNAM_H */
