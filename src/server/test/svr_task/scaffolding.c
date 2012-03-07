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
  fprintf(stderr, "The call to enqueue_threadpool_request to be mocked!!\n");
  exit(1);
  }

void check_nodes(struct work_task *ptask)
  {
  fprintf(stderr, "The call to check_nodes to be mocked!!\n");
  exit(1);
  }

resizable_array *initialize_resizable_array(int size)
  {
  fprintf(stderr, "The call to initialize_resizable_array to be mocked!!\n");
  exit(1);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  fprintf(stderr, "The call to next_thing to be mocked!!\n");
  exit(1);
  }

int insert_thing_after(resizable_array *ra, void *thing, int index)
  {
  fprintf(stderr, "The call to insert_thing_after to be mocked!!\n");
  exit(1);
  }

int get_index(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to get_index to be mocked!!\n");
  exit(1);
  }
