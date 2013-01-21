#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "work_task.h" /* all_tasks, work_task */
#include "resizable_array.h" /* resizable_array */

all_tasks task_list_timed;
all_tasks task_list_event;
task_recycler           tr;

int insert_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to insert_thing to be mocked!!\n");
  exit(1);
  }

int insert_thing_before(resizable_array *ra, void *thing, int index)
  {
  fprintf(stderr, "The call to insert_thing_before to be mocked!!\n");
  exit(1);
  }

int remove_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to remove_thing to be mocked!!\n");
  exit(1);
  }

int enqueue_threadpool_request(void *(*func)(void *),void *arg)
  {
  return 5;
  }

void check_nodes(struct work_task *ptask)
  {
  fprintf(stderr, "The call to check_nodes to be mocked!!\n");
  exit(1);
  }


void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int type, const char *func_name, const char *msg, int o) {}
