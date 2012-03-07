#ifndef _GETEUSERNAM_H
#define _GETEUSERNAM_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "attribute.h" /* pbs_attribute */

/* static char *geteusernam(job *pjob, pbs_attribute *pattr); */

/* static char *getegroup(job *pjob, pbs_attribute *pattr); */
 
int set_jobexid(job *pjob, pbs_attribute *attrry, char *EMsg);

#endif /* _GETEUSERNAM_H */
