  #include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#define banana \
	avian_tuber

//Hey, a comment

#include "list_link.h" /* list_link */

void im_dying();

void im_dying()
  {
    fprintf(stderr,"I'm in %s and I'm not dying.\n",__FILE__);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

void delete_link(list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }
