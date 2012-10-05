#ifndef _PBS_LOG_H
#define _PBS_LOG_H
#include "license_pbs.h" /* See here for the software license */

enum getter_setter { GETV, SETV };
typedef enum getter_setter SGetter;

int log_init(char *suffix, char *hostname); 

int log_open(char *filename, char *directory); 

int job_log_open(char *filename, char *directory); 

void log_err(int errnum, const char *routine, char *text); 

void log_ext(int errnum, const char *routine, char *text, int severity); 

int log_job_record(char *buf);

void log_record(int eventtype, int objclass, const char *objname, char *text); 

void log_close(int msg);

void job_log_close(int msg);

int log_remove_old(char *DirPath, unsigned long ExpireTime); 

void log_roll(int max_depth);

long log_size(void);

long job_log_size(void);

void print_trace(int socknum);

void log_get_set_eventclass(int *objclass, SGetter action);

void log_format_trq_timestamp(char *time_formatted_str, unsigned int buflen);

#endif /* _PBS_LOG_H */
