#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "resource.h" /* resource_def */
#include "server.h" /* server */
#include "list_link.h" /* list_link, tlist_head */
#include "attribute.h" /* pbs_attribute, svrattrl */

int svr_resc_size = 0;
resource_def *svr_resc_def;
struct server server;


svrattrl *attrlist_create(const char *aname, const char *rname, int vsize)
  {
  fprintf(stderr, "The call to attrlist_create to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry to be mocked!!\n");
  exit(1);
  }
