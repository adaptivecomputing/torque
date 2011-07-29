#include "license_pbs.h" /* See here for the software license */

int log_init(char *suffix, char *hostname); 

int log_open(char *filename, char *directory); 

int job_log_open(char *filename, char *directory); 

void log_err(int errnum, char *routine, char *text); 

void log_ext(int errnum, char *routine, char *text, int severity); 

int log_job_record(char *buf);

void log_record(int eventtype, int objclass, char *objname, char *text); 

void log_close(int msg);

void job_log_close(int msg);

int log_remove_old(char *DirPath, unsigned long ExpireTime); 

void log_roll(int max_depth);

long log_size(void);

long job_log_size(void);

