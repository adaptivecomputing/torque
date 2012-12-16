
#include <string.h>
#include "pbs_nodes.h"
#include "batch_request.h"
#include "list_link.h"
#include "../lib/Libutils/u_lock_ctl.h"


int status_node(struct pbsnode *, struct batch_request *, int *, tlist_head *);


int get_alps_statuses(

  struct pbsnode       *parent,
  struct batch_request *preq,
  int                  *bad,
  tlist_head           *pstathd)

  {
  struct pbsnode *alps_node;
  int             iter = -1;
  int             rc = PBSE_NONE;

  while ((alps_node = next_host(&(parent->alps_subnodes), &iter, NULL)) != NULL)
    {
    rc = status_node(alps_node, preq, bad, pstathd);
    unlock_node(alps_node, __func__, NULL, 0);

    if (rc != PBSE_NONE)
      break;
    }

  return(rc);
  } /* END get_alps_statuses() */


