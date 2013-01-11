#include<stdlib.h>
#include<stdio.h>
#include<pbs_config.h>

#include "license_pbs.h"
#include "list_link.h"
#include "portability.h"

void im_dying()
   {
   fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
   exit(1);
   }

void *get_next(list_link, char *file, int ln)
   {
   fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
   exit(1);
   }

void insert_link(struct list_link *old, struct list_link *new_link, void *pobj, int position)
   {
   fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
   exit(1);
   }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
   {
   fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
   exit(1);
   }

int is_linked(list_link *head, list_link *entry)
   {
   fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
   exit(1);
   }

int is_link_initialized(list_link *head)
   {
   fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
   exit(1);
   }

void *get_prior(list_link pl, char *file, int line)
   {
   fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
   exit(1);
   }

void list_move(tlist_head *from, tlist_head *to)
   {
   fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
   exit(1);
   }

void free_pidlist(struct pidl *pl)
   {
   fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
   exit(1);
   }

void delete_link(struct list_link *old)
   {
   fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
   exit(1);
   }

void swap_link(list_link *pone, list_link *ptwo)
   {
   fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
   exit(1);
   }

