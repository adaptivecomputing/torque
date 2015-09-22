#ifndef JOB_COMPLETED_MAP_H
#define JOB_COMPLETED_MAP_H

#include <pthread.h>
#include <map>
#include <string>

#include "work_task.h"

class completed_jobs_map_class {
    std::map< std::string, time_t > completed_jobs_map;
    pthread_mutex_t completed_jobs_map_mutex;
    bool delete_job_unlocked(std::string);
  public:
    completed_jobs_map_class();
    ~completed_jobs_map_class();
    bool add_job(const char*, time_t);
    bool delete_job(const char*);
    bool is_job_id_in_map(const char*);
    int cleanup_completed_jobs(void);
    void print_map(void);
};

void add_to_completed_jobs(work_task *wt);

#endif
