#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "list_link.h" /* list_link */
#include "resource.h" /* resource_def */
int svr_resc_size = 0;
resource_def *svr_resc_def;


void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  } 

void insert_link(struct list_link *old, struct list_link *new, void *pobj, int position) 
  {
  fprintf(stderr, "The call to insert_link needs to be mocked!!\n");
  exit(1);
  } 

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  } 

void append_link(tlist_head *head, list_link *new, void *pobj)
 {
 fprintf(stderr, "The call to append_link needs to be mocked!!\n");
 exit(1);
 }

