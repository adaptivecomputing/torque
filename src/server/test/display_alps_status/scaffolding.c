#include <stdlib.h>
#include <stdio.h>

#include "pbs_nodes.h"
#include "batch_request.h"
#include "list_link.h"

int num_nodes = 10;

struct pbsnode *next_host(all_nodes *an, int *iter, struct pbsnode *pnode)
  {

  if (*iter == -1)
    *iter = 1;
  else if (*iter == num_nodes)
    return(NULL);
  else
    *iter += 1;

  return((struct pbsnode *)iter);
  }

int unlock_node(struct pbsnode *the_node, const char *method_name, char *msg, int logging)
  {
  return(0);
  }

int status_node(struct pbsnode *pnode, struct batch_request *preq, int *bad, tlist_head *pstathd)
  {
  int *count = (int *)pstathd->ll_struct;
  *count += 1;

  if (num_nodes == -1)
    return(-1);

  return(0);
  }

