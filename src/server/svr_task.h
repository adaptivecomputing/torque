#ifndef _SVR_TASK_H
#define _SVR_TASK_H
#include "license_pbs.h" /* See here for the software license */

#include "work_task.h" /* work_type, work_task, all_tasks */

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock);

int task_is_in_threadpool(struct work_task *ptask);

void dispatch_task(struct work_task *ptask);

void delete_task(struct work_task *ptask);

void initialize_all_tasks_array(all_tasks *at);

work_task *next_task(all_tasks *at, int *iter);

int insert_task(all_tasks *at, work_task *wt, int object);

int insert_task_before(all_tasks *at, work_task *before, work_task *after);

int insert_task_first(all_tasks *at, work_task *wt);

int has_task(all_tasks *at);

int remove_task(all_tasks *at, work_task *wt);

#endif /* _SVR_TASK_H */
