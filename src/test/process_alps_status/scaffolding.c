#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <pbs_config.h>

#include "utils.h"
#include "batch_request.h"
#include "work_task.h"
#include "attribute.h"
#include "u_tree.h"
#include "threadpool.h"
#include "resource.h"
#include "track_alps_reservations.hpp"

#define ATR_DFLAG_SSET  (ATR_DFLAG_SvWR | ATR_DFLAG_SvRD)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define mix(a,b,c) \
  { \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
  }

#define final(a,b,c) \
  { \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
  }

int removed_reservation = 0;
int issued_request = 0;
int state_updated = 0;

threadpool_t *task_pool;
bool exit_called = false;
char log_buffer[LOG_BUF_SIZE];
int mgr_count;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
all_nodes allnodes;
struct node_state
  {
  short  bit;
  const char  *name;
  } ns[] =

{
    {INUSE_UNKNOWN, ND_state_unknown},
    {INUSE_DOWN,    ND_down},
    {INUSE_OFFLINE, ND_offline},
    {INUSE_RESERVE, ND_reserve},
    {INUSE_JOB,     ND_job_exclusive},
    {INUSE_BUSY,    ND_busy},
    {0,             NULL}
};

void log_record(int eventtype, int objclass, const char *objname, const char *msg) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_err(int errnum, const char *routine, const char *text) {}
int lock_node(struct pbsnode *pnode, const char *method, const char *msg, int log_level) {return(0);}
int unlock_node(struct pbsnode *pnode, const char *method, const char *msg, int log_level) {return(0);}


/* nothing needed for this function */
int save_node_status(struct pbsnode *np, pbs_attribute  *temp)

  {
  return(0);
  } /* END save_node_status() */


void update_node_state(

  struct pbsnode *np,         /* I (modified) */
  int             newstate)   /* I (one of INUSE_*) */

  {
  state_updated++;

  if (newstate & INUSE_DOWN)
    {
    if (!(np->nd_state & INUSE_DOWN))
      {
      np->nd_state |= INUSE_DOWN;
      np->nd_state &= ~INUSE_UNKNOWN;
      }
    
    /* ignoring the obvious possibility of a "down,busy" node */
    }  /* END if (newstate & INUSE_DOWN) */
  else if (newstate & INUSE_BUSY)
    {
    np->nd_state |= INUSE_BUSY;
    
    np->nd_state &= ~INUSE_UNKNOWN;
    
    if (np->nd_state & INUSE_DOWN)
      {
      np->nd_state &= ~INUSE_DOWN;

      }
    }  /* END else if (newstate & INUSE_BUSY) */
  else if (newstate == INUSE_FREE)
    {
    np->nd_state &= ~INUSE_BUSY;

    np->nd_state &= ~INUSE_UNKNOWN;

    if (np->nd_state & INUSE_DOWN)
      {
      np->nd_state &= ~INUSE_DOWN;
      }
    }    /* END else if (newstate == INUSE_FREE) */

  if (newstate & INUSE_UNKNOWN)
    {
    np->nd_state |= INUSE_UNKNOWN;
    }

  return;
  }  /* END update_node_state() */




struct pbsnode *find_nodebyname(

  const char *nodename) /* I */

  {
  static struct pbsnode pnode;
  static int    initialized = 0;

  if (initialized == 0)
    {
    memset(&pnode, 0, sizeof(struct pbsnode));
    pnode.nd_name = strdup("george");
    pnode.alps_subnodes = new all_nodes();
    }

  initialized++;

  return(&pnode);
  }  /* END find_nodebyname() */


void free_prop_list(
        
  struct prop *prop)
  
  {  
  struct prop *pp;
  
  while (prop)
    {
    pp = prop->next;
    free(prop);
    prop = pp;
    }
  } /* END free_prop_list() */



struct prop *init_prop(

  char *pname) /* I */

  {
  struct prop *pp;
  
  if ((pp = (struct prop *)calloc(1, sizeof(struct prop))) != NULL)
    {
    pp->name    = pname;
    pp->mark    = 0;
    pp->next    = 0;
    }
  
  return(pp);
  }  /* END init_prop() */




int PNodeStateToString(

  int   SBM,     /* I (state bitmap) */
  char *Buf,     /* O */
  int   BufSize) /* I */

  {
  int len;

  if (Buf == NULL)
    {
    return(-1);
    }

  BufSize--;

  Buf[0] = '\0';

  if (SBM & (INUSE_DOWN))
    {
    len = strlen(ND_down);

    if (len < BufSize)
      {
      strcat(Buf, ND_down);
      BufSize -= len;
      }
    }

  if (SBM & (INUSE_OFFLINE))
    {
    len = strlen(ND_offline) + 1;

    if (len < BufSize)
      {
      if (Buf[0] != '\0')
        strcat(Buf, ",");
      else
        len--;

      strcat(Buf, ND_offline);
      BufSize -= len;
      }
    }

  if (SBM & (INUSE_JOB))
    {
    len = strlen(ND_job_exclusive) + 1;

    if (len < BufSize)
      {
      if (Buf[0] != '\0')
        strcat(Buf, ",");
      else
        len--;

      strcat(Buf, ND_job_exclusive);
      BufSize -= len;
      }

    }

  if (SBM & (INUSE_BUSY))
    {
    len = strlen(ND_busy) + 1;

    if (len < BufSize)
      {
      if (Buf[0] != '\0')
        strcat(Buf, ",");
      else
        len--;

      strcat(Buf, ND_busy);
      BufSize -= len;
      }
    }

  if (SBM & (INUSE_RESERVE))
    {
    len = strlen(ND_reserve) + 1;

    if (len < BufSize)
      {
      if (Buf[0] != '\0')
        strcat(Buf, ",");
      else
        len--;

      strcat(Buf, ND_reserve);
      BufSize -= len;
      }
    }

  if (SBM & (INUSE_UNKNOWN))
    {
    len = strlen(ND_state_unknown) + 1;

    if (len < BufSize)
      {
      if (Buf[0] != '\0')
        strcat(Buf, ",");
      else
        len--;

      strcat(Buf, ND_state_unknown);
      BufSize -= len;
      }
    }

  if (Buf[0] == '\0')
    {
    snprintf(Buf, BufSize, "%s", ND_free);
    }

  return(0);
  } /* END PNodeStateToString() */




int mgr_set_node_attr(

  struct pbsnode *pnode,  /* do some kind of "set" action on this node */
  attribute_def  *pdef,   /* pbsnode doesn't have attributes but in the
                               future it might  so, make  code similar to existing */
  int             limit,  /* length of the *pdef array      */
  svrattrl       *plist,  /* batch request's svrattrl list begins here */
  int             privil, /* requester's privilege          */
  int            *bad,    /* if there is a "bad pbs_attribute" pass back 
                               position via this loc */
  void           *parent, /*may go unused in this function */
  int             mode,  /*passed to attrib's action func not used by 
                             this func at this time*/
  bool            dont_update)

  {
  mgr_count++;

  if (mgr_count < 2)
    return(0);
  else
    return(1);
  }  /* END mgr_set_node_attr() */



/* nothing needed for this function */
int decode_arst(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  return(0);
  }  /* END decode_arst() */

int encode_ll(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  return(PBSE_NONE);
  }


int attr_atomic_node_set(

  struct svrattrl *plist,    /* list of pbs_attribute modif structs */
  pbs_attribute   *old,      /* unused */
  pbs_attribute   *new_attr,      /* new pbs_attribute array begins here */
  attribute_def   *pdef,     /* begin array  definition structs */
  int              limit,    /* number elts in definition array */
  int              unkn,     /* <0 unknown attrib not permitted */
  int              privil,   /* requester's access privileges   */
  int             *badattr,  /* return list position wher bad   */
  bool             update_nodes_file)

  {
  int           acc;
  int           index;
  int           listidx;
  int           rc = 0;
  pbs_attribute temp;

  listidx = 0;

  while (plist)
    {
    /*Traverse loop for each client entered pbs_attribute*/

    listidx++;

    if ((index = find_attr(pdef, plist->al_name, limit)) < 0)
      {
      if (unkn < 0)         /*if unknown attr not allowed*/
        {
        rc =  PBSE_NOATTR;
        break;
        }
      else
        index = unkn;  /*if unknown attr are allowed*/
      }


    /* The name of the pbs_attribute is in the definitions list*/
    /* Now, have we privilege to set the pbs_attribute ?       */
    /* Check access capabilities specified in the attrdef  */
    /* against the requestor's privilege level        */

    acc = (pdef + index)->at_flags & ATR_DFLAG_ACCESS;

    if ((acc & privil & ATR_DFLAG_WRACC) == 0)
      {
      if (privil & ATR_DFLAG_SvWR)
        {
        /*  from a daemon, just ignore this pbs_attribute */
        plist = (struct svrattrl *)GET_NEXT(plist->al_link);
        continue;
        }
      else
        {
        /*from user, no write access to pbs_attribute     */
        rc = PBSE_ATTRRO;
        break;
        }
      }

    /*decode new value*/

    clear_attr(&temp, pdef + index);

    if ((rc = (pdef + index)->at_decode(&temp, plist->al_name,
            plist->al_resc, plist->al_value,0) != 0))
      {
      if ((rc == PBSE_UNKRESC) && (unkn > 0))
        rc = 0;              /*ignore the "error"*/
      else
        break;
      }

    /*update "new" with "temp", MODIFY is set on "new" if changed*/

    (new_attr + index)->at_flags &= ~ATR_VFLAG_MODIFY;

    if ((plist->al_op != INCR) && (plist->al_op != DECR) &&
        (plist->al_op != SET) && (plist->al_op != INCR_OLD))
      plist->al_op = SET;


    if (temp.at_flags & ATR_VFLAG_SET)
      {
      /* "temp" has a data value, use it to update "new" */

      if ((rc = (pdef + index)->at_set(new_attr + index, &temp, plist->al_op)) != 0)
        {
        (pdef + index)->at_free(&temp);
        break;
        }
      }
    else if (temp.at_flags & ATR_VFLAG_MODIFY)
      {

      (pdef + index)->at_free(new_attr + index);
      (new_attr + index)->at_flags |= ATR_VFLAG_MODIFY;
      }

    (pdef + index)->at_free(&temp);
    plist = (struct svrattrl *)GET_NEXT(plist->al_link);
    }

  if (rc != 0)
    {

    /*"at_free" functions get invoked by upstream caller*/
    /*invoking attr_atomic_kill() on the array of       */
    /*node-pbs_attribute structs-- any hanging structs are  */
    /*freed and then the node-pbs_attribute array is freed  */

    *badattr = listidx;   /*the svrattrl that gave a problem*/
    }

  return (rc);
  }




void free_arst(pbs_attribute *attr) {}



void clear_attr(

    pbs_attribute *pattr, /* O */
    attribute_def *pdef)  /* I */

  {
#ifndef NDEBUG

  if (pdef == NULL)
    {
    fprintf(stderr, "Assertion failed, bad pdef in clear_attr\n");

    abort();
    }

#endif /* NDEBUG */

  memset(pattr, 0, sizeof(pbs_attribute));

  pattr->at_type = pdef->at_type;

  if ((pattr->at_type == ATR_TYPE_RESC) ||
      (pattr->at_type == ATR_TYPE_LIST))
    {
    CLEAR_HEAD(pattr->at_val.at_list);
    }

  return;
  }



int str_nc_cmp(

    const char *s1,  /* I */
    const char *s2)

  {
  return(0);
  }



int find_attr(

    struct attribute_def *attr_def, /* ptr to pbs_attribute definitions */
    const char          *name,     /* pbs_attribute name to find */
    int                   limit)    /* limit on size of def array */

  {
  int index;

  if (attr_def != NULL)
    {
    for (index = 0;index < limit;index++)
      {
      if (!str_nc_cmp(attr_def->at_name, name))
        {
        return(index);
        }

      attr_def++;
      }  /* END for (index) */
    }

  return(-1);
  }



int node_gpustatus_list(

    pbs_attribute *new_attr,      /* derive status into this pbs_attribute*/
    void          *pnode,    /* pointer to a pbsnode struct     */
    int            actmode)  /* action mode; "NEW" or "ALTER"   */

  {
  int              rc = 0;

  struct pbsnode *np;
  pbs_attribute   temp;

  np = (struct pbsnode *)pnode;    /* because of at_action arg type */

  switch (actmode)
    {

    case ATR_ACTION_NEW:

      /* if node has a status list, then copy array_strings    */
      /* into temp to use to setup a copy, otherwise setup empty */

      if (np->nd_gpustatus != NULL)
        {
        /* setup temporary pbs_attribute with the array_strings */
        /* from the node                                    */

        temp.at_val.at_arst = np->nd_gpustatus;
        temp.at_flags = ATR_VFLAG_SET;
        temp.at_type  = ATR_TYPE_ARST;

        rc = set_arst(new_attr, &temp, SET);
        }
      else
        {
        /* node has no properties, setup empty pbs_attribute */

        new_attr->at_val.at_arst = NULL;
        new_attr->at_flags       = 0;
        new_attr->at_type        = ATR_TYPE_ARST;
        }

      break;

    case ATR_ACTION_ALTER:

      if (np->nd_gpustatus != NULL)
        {
        free(np->nd_gpustatus->as_buf);
        free(np->nd_gpustatus);

        np->nd_gpustatus = NULL;
        }

      /* update node with new attr_strings */

      np->nd_gpustatus = new_attr->at_val.at_arst;

      new_attr->at_val.at_arst = NULL;
      /* update number of status items listed in node */
      /* does not include name and subnode property */

      if (np->nd_gpustatus != NULL)
        np->nd_ngpustatus = np->nd_gpustatus->as_usedptr;
      else
        np->nd_ngpustatus = 0;

      break;

    default:

      rc = PBSE_INTERNAL;

      break;
    }  /* END switch(actmode) */

  return(rc);
  }  /* END node_gpustatus_list() */



int set_arst(

  pbs_attribute *attr,  /* I/O */
  pbs_attribute *new_attr,   /* I */
  enum batch_op     op)    /* I */

  {
  int  i;
  int  j;
  int   nsize;
  int   need;
  long  offset;
  char *pc = NULL;
  long  used;

  struct array_strings *newpas = NULL;

  struct array_strings *pas = NULL;

  struct array_strings *tmp_arst = NULL;

  pas = attr->at_val.at_arst;
  newpas = new_attr->at_val.at_arst;

  if (newpas == NULL)
    {
    return(PBSE_INTERNAL);
    }

  if (pas == NULL)
    {
    /* not array_strings control structure, make one */

    j = newpas->as_npointers;

    if (j < 1)
      {
      return(PBSE_INTERNAL);
      }

    need = sizeof(struct array_strings) + (j - 1) * sizeof(char *);

    pas = (struct array_strings *)calloc(1, (size_t)need);

    if (pas == NULL)
      {
      return(PBSE_SYSTEM);
      }

    pas->as_npointers = j;

    pas->as_usedptr = 0;
    pas->as_bufsize = 0;
    pas->as_buf     = NULL;
    pas->as_next    = NULL;
    attr->at_val.at_arst = pas;
    }  /* END if (pas == NULL) */

  if ((op == INCR) && !pas->as_buf)
    op = SET; /* no current strings, change op to SET */

  /*
   *    * At this point we know we have a array_strings struct initialized
   *       */

  switch (op)
    {
    case SET:

      /*
       *        * Replace old array of strings with new array, this is
       *               * simply done by deleting old strings and appending the
       *                      * new (to the null set).
       *                             */

      for (i = 0;i < pas->as_usedptr;i++)
        pas->as_string[i] = NULL; /* clear all pointers */

      pas->as_usedptr = 0;

      pas->as_next    = pas->as_buf;

      if (new_attr->at_val.at_arst == (struct array_strings *)0)
        break; /* none to set */

      nsize = newpas->as_next - newpas->as_buf; /* space needed */

      if (nsize > pas->as_bufsize)
        {
        /* new data won't fit */

        if (pas->as_buf)
          free(pas->as_buf);

        nsize += nsize / 2;   /* alloc extra space */

        if (!(pas->as_buf = (char *)calloc(1, (size_t)nsize)))
          {
          pas->as_bufsize = 0;

          return(PBSE_SYSTEM);
          }

        pas->as_bufsize = nsize;
        }
      else
        {
        /* str fits, clear buf */

        memset(pas->as_buf,0,pas->as_bufsize);
        }

      pas->as_next = pas->as_buf;

      /* no break, "SET" falls into "MERGE" to add strings */
    case INCR_OLD:
    case MERGE:

      nsize = newpas->as_next - newpas->as_buf;   /* space needed */
      used = pas->as_next - pas->as_buf;

      if (nsize > (pas->as_bufsize - used))
        {
        need = pas->as_bufsize + 2 * nsize;  /* alloc new buf */

        if (pas->as_buf)
          {
          pc = (char *)realloc(pas->as_buf, (size_t)need);
          if (pc != NULL)
            memset(pc + pas->as_bufsize, 0, need - pas->as_bufsize);
          }
        else
          pc = (char *)calloc(1, (size_t)need);

        if (pc == NULL)
          {
          return(PBSE_SYSTEM);
          }

        offset          = pc - pas->as_buf;

        pas->as_buf     = pc;
        pas->as_next    = pc + used;
        pas->as_bufsize = need;

        for (j = 0;j < pas->as_usedptr;j++) /* adjust points */
          pas->as_string[j] += offset;
        }  /* END if (nsize > (pas->as_bufsize - used)) */

      j = pas->as_usedptr + newpas->as_usedptr;

      if (j > pas->as_npointers)
        {
        struct array_strings *tmpPas;

        /* need more pointers */

        j = 3 * j / 2;  /* allocate extra     */

        need = (int)sizeof(struct array_strings) + (j - 1) * sizeof(char *);

        tmpPas = (struct array_strings *)realloc((char *)pas,(size_t)need);

        if (tmpPas == NULL)
          {
          return(PBSE_SYSTEM);
          }

        tmpPas->as_npointers = j;

        pas = tmpPas;

        attr->at_val.at_arst = pas;
        }  /* END if (j > pas->as_npointers) */

      /* now append new strings */

      for (i = 0;i < newpas->as_usedptr;i++)
        {
        char *tail;
        int   len;
        int   MatchFound; /* boolean */

        if ((op == MERGE) && (tail = strchr(newpas->as_string[i],'=')))
          {
          len = tail - newpas->as_string[i];
          MatchFound = 0;

          for (j = 0;j < pas->as_usedptr;j++)
            {
            if (!strncmp(pas->as_string[j],newpas->as_string[i],len))
              {
              MatchFound = 1;

              break;
              }
            }

          if (MatchFound == 1)
            continue;
          }

        strcpy(pas->as_next,newpas->as_string[i]);

        pas->as_string[pas->as_usedptr++] = pas->as_next;
        pas->as_next += strlen(pas->as_next) + 1;
        }  /* END for (i) */

      break;

    case INCR:

      j = 1.5 * (pas->as_usedptr + newpas->as_usedptr);

      /* calloc the tmp array strings */
      need = sizeof(struct array_strings) + (j-1) * sizeof(char *);
      tmp_arst = (struct array_strings *)calloc(1, need);

      if (tmp_arst == NULL)
        return(PBSE_SYSTEM);

      memset(tmp_arst,0,need);

      nsize = newpas->as_next - newpas->as_buf;   /* space needed */
      used = pas->as_next - pas->as_buf;
      need = 2 * (nsize + used);
      tmp_arst->as_bufsize = need;

      /* calloc the buffer size */
      pc = (char *)calloc(1, need);

      if (pc == NULL)
        return(PBSE_SYSTEM);
      memset(pc, 0, need);

      tmp_arst->as_buf = pc;
      tmp_arst->as_next = pc;
      tmp_arst->as_npointers = j;

      /* now that everything is allocated, copy the strings into the new 
       *        * array_strings struct */

      /* first, copy the new */
      for (i = 0; i < newpas->as_usedptr; i++)
        {
        strcpy(tmp_arst->as_next,newpas->as_string[i]);

        tmp_arst->as_string[tmp_arst->as_usedptr++] = tmp_arst->as_next;
        tmp_arst->as_next += strlen(tmp_arst->as_next) + 1;
        }

      /* second, copy the old if not already there */
      for (i = 0; i < pas->as_usedptr; i++)
        {
        char *tail;
        int   len;
        int   MatchFound = 0; /* boolean */

        if ((tail = strchr(pas->as_string[i],'=')))
          {
          len = tail - pas->as_string[i];

          for (j = 0;j < newpas->as_usedptr;j++)
            {
            if (!strncmp(pas->as_string[i],newpas->as_string[j],len))
              {
              MatchFound = 1;

              break;
              }
            }

          }
        if (MatchFound == 0)
          {
          strcpy(tmp_arst->as_next, pas->as_string[i]);

          tmp_arst->as_string[tmp_arst->as_usedptr++] = tmp_arst->as_next;
          tmp_arst->as_next += strlen(tmp_arst->as_next) + 1;
          }
        }

      /* free the old pas */
      free_arst(attr);

      attr->at_val.at_arst = tmp_arst;

      break;

    case DECR:

      /* decrement (remove) string from array */
      for (j = 0;j < newpas->as_usedptr;j++)
        {
        for (i = 0;i < pas->as_usedptr;i++)
          {
          if (!strcmp(pas->as_string[i], newpas->as_string[j]))
            {
            /* compact buffer */
            int k;

            nsize = strlen(pas->as_string[i]) + 1;

            pc = pas->as_string[i] + nsize;

            need = pas->as_next - pc;

            for (k = 0; k < need; k++)
              pas->as_string[i][k] = *(pc + k);

            /*            memcpy(pas->as_string[i], pc, (size_t)need);*/

            pas->as_next -= nsize;

            /* compact pointers */

            for (++i;i < pas->as_npointers;i++)
              pas->as_string[i - 1] = pas->as_string[i] - nsize;

            pas->as_string[i - 1] = NULL;

            pas->as_usedptr--;

            break;
            }
          }
        }

      break;

    default:

      return(PBSE_INTERNAL);

      /*NOTREACHED*/

      break;
    }  /* END switch(op) */

  attr->at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;

  return(0);
  }  /* END set_arst() */



void *get_next(

  list_link  pl,   /* I */
  char      *file, /* I */
  int        line) /* I */

  {
  if ((pl.ll_next == NULL) ||
      ((pl.ll_next == &pl) && (pl.ll_struct != NULL)))
    {
    fprintf(stderr, "Assertion failed, bad pointer in link: file \"%s\", line %d\n",
        file,
        line);

    abort();
    }

  return(pl.ll_next->ll_struct);
  }  /* END get_next() */




int node_status_list(

  pbs_attribute *new_attr,           /*derive status into this pbs_attribute*/
  void          *pnode,         /*pointer to a pbsnode struct     */
  int            actmode)       /*action mode; "NEW" or "ALTER"   */

  {
  int              rc = 0;

  struct pbsnode  *np;
  pbs_attribute    temp;

  np = (struct pbsnode *)pnode;    /* because of at_action arg type */

  switch (actmode)
    {

    case ATR_ACTION_NEW:

      /* if node has a status list, then copy array_strings    */
      /* into temp to use to setup a copy, otherwise setup empty */

      if (np->nd_status != NULL)
        {
        /* setup temporary pbs_attribute with the array_strings */
        /* from the node                                    */

        temp.at_val.at_arst = np->nd_status;
        temp.at_flags = ATR_VFLAG_SET;
        temp.at_type  = ATR_TYPE_ARST;

        rc = set_arst(new_attr, &temp, SET);
        }
      else
        {
        /* node has no properties, setup empty pbs_attribute */

        new_attr->at_val.at_arst = NULL;
        new_attr->at_flags       = 0;
        new_attr->at_type        = ATR_TYPE_ARST;
        }

      break;

    case ATR_ACTION_ALTER:

      if (np->nd_status != NULL)
        {
        free(np->nd_status->as_buf);
        free(np->nd_status);

        np->nd_status = NULL;
        }

      /* update node with new attr_strings */

      np->nd_status = new_attr->at_val.at_arst;

      new_attr->at_val.at_arst = NULL;
      /* update number of status items listed in node */
      /* does not include name and subnode property */

      if (np->nd_status != NULL)
        np->nd_nstatus = np->nd_status->as_usedptr;
      else
        np->nd_nstatus = 0;

      break;

    default:

      rc = PBSE_INTERNAL;

      break;
    }  /* END switch(actmode) */

  return(rc);
  }  /* END node_status_list() */




attribute_def node_attr_def[] =
  {

	/* ND_ATR_state */
    { ATTR_NODE_state, /* "state" */
    decode_state,
    encode_state,
    set_node_state,
    comp_null,
    free_null,
    node_state,
    NO_USER_SET,
    ATR_TYPE_SHORT,
    PARENT_TYPE_NODE,
    },

	/* ND_ATR_np */
	{ ATTR_NODE_np,  /* "np" */
	  decode_l,
	  encode_l,
	  set_l,
	  comp_null,
	  free_null,
	  node_np_action,
	  NO_USER_SET,
	  ATR_TYPE_LONG,
	  PARENT_TYPE_NODE,
	},
	/* ND_ATR_properties */
	{ ATTR_NODE_properties, /* "properties" */
	  decode_arst,
	  encode_arst,
	  set_arst,
	  comp_null,
	  free_arst,
	  node_prop_list,
	  MGR_ONLY_SET,
	  ATR_TYPE_ARST,
	  PARENT_TYPE_NODE,
	},
  
	/* ND_ATR_ntype */
	{ ATTR_NODE_ntype, /* "ntype" */
	  decode_ntype,
	  encode_ntype,
	  set_node_ntype,
	  comp_null,
	  free_null,
	  node_ntype,
	  NO_USER_SET,
	  ATR_TYPE_SHORT,
	  PARENT_TYPE_NODE,
	},
  
	/* ND_ATR_jobs */
	{   ATTR_NODE_jobs,         /* "jobs" */
		decode_null,
		encode_jobs,
		set_null,
		comp_null,
		free_null,
		NULL_FUNC,
		ATR_DFLAG_RDACC,
		ATR_TYPE_JINFOP,
		PARENT_TYPE_NODE,
	},
  
	/* ND_ATR_status */
	{  ATTR_NODE_status,
	   decode_arst,
	   encode_arst,
	   set_arst,
	   comp_null,
	   free_arst,
	   node_status_list,
	   MGR_ONLY_SET,
	   ATR_TYPE_ARST,
	   PARENT_TYPE_NODE,
	},
  
	/* ND_ATR_note */
	{ ATTR_NODE_note, /* "note" */
	  decode_str,
	  encode_str,
	  set_note_str,
	  comp_str,
	  free_str,
	  node_note,
	  NO_USER_SET,
	  ATR_TYPE_STR,
	  PARENT_TYPE_NODE,
	},
	/* ND_ATR_mom_port */
	{ ATTR_NODE_mom_port,  /* "mom_service_port" */
	  decode_l,
	  encode_l,
	  set_l,
	  comp_null,
	  free_null,
	  node_mom_port_action,
	  NO_USER_SET,
	  ATR_TYPE_LONG,
	  PARENT_TYPE_NODE,
	},
	/* ND_ATR_mom_rm_port */
	{ ATTR_NODE_mom_rm_port,  /* "mom_manager_port" */
	  decode_l,
	  encode_l,
	  set_l,
	  comp_null,
	  free_null,
	  node_mom_rm_port_action,
	  NO_USER_SET,
	  ATR_TYPE_LONG,
	  PARENT_TYPE_NODE,
	},
  /* ND_ATR_num_node_boards */
  { ATTR_NODE_num_node_boards, /* "num_node_boards" */
    decode_l,
    encode_l,
    set_l,
    comp_null,
    free_null,
    node_numa_action,
    NO_USER_SET | ATR_DFLAG_NOSTAT,
    ATR_TYPE_LONG,
    PARENT_TYPE_NODE,
  },
  /* ND_ATR_numa_str */
  { ATTR_NODE_numa_str, /* "node_board_str" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    numa_str_action,
    NO_USER_SET | ATR_DFLAG_NOSTAT,
    ATR_TYPE_STR,
    PARENT_TYPE_NODE,
  },
  /* ND_ATR_gpus */
  { ATTR_NODE_gpus,    /* "gpus" */
    decode_l,
    encode_l,
    set_l,
    comp_null,
    free_null,
    node_gpus_action,
    NO_USER_SET,
    ATR_TYPE_LONG,
    PARENT_TYPE_NODE,
  },
  /* ND_ATR_gpus_str */
  { ATTR_NODE_gpus_str, /* "numa_gpu_node_str" */
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    gpu_str_action,
    NO_USER_SET | ATR_DFLAG_NOSTAT,
    ATR_TYPE_STR,
    PARENT_TYPE_NODE,
  },
  /* ND_ATR_gpustatus */
  {ATTR_NODE_gpustatus,		/* "gpu_status" */
   decode_arst,
   encode_arst,
   set_arst,
   comp_null,
   free_arst,
   node_gpustatus_list,
   MGR_ONLY_SET,
   ATR_TYPE_ARST,
   PARENT_TYPE_NODE,
   },

  };



int create_a_gpusubnode(

  struct pbsnode *pnode)

  {
  /* increment the number of gpu subnodes and gpus free */
  pnode->nd_ngpus++;
  pnode->nd_ngpus_free++;

  return(PBSE_NONE);
  } /* END create_a_gpusubnode() */



struct pbsnode *AVL_find(

  u_long   key,
  uint16_t port, 
  AvlTree  tree)

  {
  if (tree == NULL)
    {
    return( NULL );
    }

  if (key < tree->key)
    return( AVL_find( key, port, tree->left ));
  else if ( key > tree->key )
    return( AVL_find( key, port, tree->right ));
  else
    {
    if (port < tree->port)
      return( AVL_find( key, port, tree->left ) );
    else if ( port > tree->port )
      return( AVL_find( key, port, tree->right ));
    }

  return( tree->pbsnode );
  } /* end AVL_find */



int insert_node(

  all_nodes      *an,    /* M */
  struct pbsnode *pnode) /* I */

  {
  return(0);
  } /* END insert_node() */




int initialize_pbsnode(

  struct pbsnode *pnode,
  char           *pname, /* node name */
  u_long         *pul,  /* host byte order array */
  /* ipaddrs for this node */
  int             ntype, /* time-shared or cluster */
  bool            isNUMANode) /* TRUE if this is a NUMA node. */

  {
  memset(pnode, 0, sizeof(struct pbsnode));

  pnode->nd_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  if (pnode->nd_mutex == NULL)
    {
    log_err(ENOMEM, __func__, "Could not allocate memory for the node's mutex");

    return(ENOMEM);
    }

  pthread_mutex_init(pnode->nd_mutex,NULL);

  pnode->nd_name = pname;

  return(PBSE_NONE);
  }  /* END initialize_pbsnode() */




void attr_atomic_kill(

  pbs_attribute *temp,
  attribute_def *pdef,
  int            limit)

  {
  int i;

  for (i = 0; i < limit; i++)
    (pdef + i)->at_free(temp + i);

  (void)free(temp);
  }

int comp_null(struct pbs_attribute *attr, struct pbs_attribute *with)

  {
  return(0);
  }

int decode_ntype(pbs_attribute *pattr, const char *name, const char *rescn, const char *val, int perm)
  {
  return(0);
  }

int encode_l(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  return(0);
  }

int encode_str(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  return(0);
  }

int decode_null(pbs_attribute *patr, const char *name, const char *rn, const char *val, int perm)
  {
  return(0);
  }

char *parse_comma_string(

  char  *start, /* I (optional) */
  char **ptr)   /* I */

  {
  char *pc; /* if start is null, restart from here */

  char     *back;
  char     *rv;

  if ((ptr == NULL) ||
      ((*ptr == NULL) && (start == NULL)))
    return(NULL);

  if (start != NULL)
    pc = start;
  else
    pc = *ptr;

  if (*pc == '\0')
    {
    return(NULL); /* already at end, no strings */
    }

  /* skip over leading white space */

  while ((*pc != '\n') && isspace((int)*pc) && *pc)
    pc++;

  rv = pc;  /* the start point which will be returned */

  /* go find comma or end of line */

  while (*pc)
    {
    if (*pc == '\\')
      {
      if (*++pc == '\0')
        break;
      }
    else if ((*pc == ',') || (*pc == '\n'))
      {
      break;
      }

    ++pc;
    }

  back = pc;

  while (isspace((int)*--back)) /* strip trailing spaces */
    *back = '\0';

  if (*pc != '\0')
    *pc++ = '\0'; /* if not end, terminate this and adv past */

  *ptr = pc;

  return(rv);
  }  /* END parse_comma_string() */

int decode_str(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  return(0);
  }

int set_node_ntype(pbs_attribute *pattr, pbs_attribute *new_attr, enum batch_op op)
  {
  return(0);
  }

int node_mom_rm_port_action(pbs_attribute *new_attr, void *pobj, int actmode)
  {
  return(0);
  }

int set_null (pbs_attribute * patr, pbs_attribute * new_attr, enum batch_op op)
  {
  return(0);
  }

int set_note_str(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  return(0);
  }

int decode_state(pbs_attribute *pattr, const char *name, const char *rescn, const char *val, int perm)
  {
  return(0);
  }

int encode_jobs(pbs_attribute *pattr, tlist_head *ph, const char *aname, const char *rname, int mode, int perm)
  {
  return(0);
  }

int set_str(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  return(0);
  }

int node_prop_list(pbs_attribute *new_attr, void *pnode, int actmode)
  {
  return(0);
  }

int node_state(pbs_attribute *new_attr, void *pnode, int actmode)
  {
  return(0);
  }

int set_l(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  return(0);
  }

int numa_str_action(pbs_attribute *new_attr, void *pnode, int actmode)
  {
  return(0);
  }

void free_str(struct pbs_attribute *attr) {}

int encode_ntype(pbs_attribute *pattr, tlist_head *ph, const char *aname, const char *rname, int mode, int perm)
  {
  return(0);
  }

int node_note(pbs_attribute *new_attr, void *pnode, int actmode)
  {
  return(0);
  }

int set_node_state(pbs_attribute *pattr, pbs_attribute *new_attr, enum batch_op op)
  {
  return(0);
  }

int node_np_action(pbs_attribute *new_attr, void *pobj, int actmode)
  {
  return(0);
  }

int gpu_str_action(pbs_attribute *new_attr, void *pnode, int actmode)
  {
  return(0);
  }

int comp_str(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  return(0);
  }

int node_numa_action(pbs_attribute *new_attr, void *pnode, int actmode)
  {
  return(0);
  }

int decode_l( pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  return(0);
  }

int encode_state(pbs_attribute *pattr, tlist_head *ph, const char *aname, const char *rname, int mode, int perm)
  {
  return(0);
  }

int node_mom_port_action(pbs_attribute *new_attr, void *pobj, int actmode)
  {
  return(0);
  }

int node_ntype(pbs_attribute *new_attr, void *pnode, int actmode)
  {
  return(0);
  }

int node_gpus_action(pbs_attribute *new_attr, void *pnode, int actmode)
  {
  return(0);
  }

int encode_arst( pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  return(0);
  }

void free_null(struct pbs_attribute *attr) {}

bool reservation_holder::is_orphaned(

  const char *rsv_id,
  std::string &job_id)

  {
  return(true);
  }
  
bool reservation_holder::already_recorded(const char *rsv_id)
  {
  return(true);
  }
  
int reservation_holder::remove_alps_reservation(const char *rsv_id)
  {
  return(0);
  }
  
void reservation_holder::remove_from_orphaned_list(const char *rsv_id) {}

job *svr_find_job(char *jobid, int get_subjob)
  {
  static struct job pjob;
  static int    i = 0;

  if (i == 0)
    {
    pjob.ji_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
    }
  i++;

  return(&pjob);
  }

job *svr_find_job_by_id(int id)
  {
  static struct job pjob;
  static int    i = 0;

  if (i == 0)
    {
    pjob.ji_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
    }
  i++;

  return(&pjob);
  }

void free_br(struct batch_request *preq) {}

int issue_Drequest(

  int                    conn,
  struct batch_request  *request,
  bool                   close_handle)

  {
  issued_request++;
  return(0);
  }

pbs_net_t get_hostaddr(

  int  *local_errno, /* O */    
  const char *hostname)    /* I */

  {
  return(0);
  }

int reservation_holder::track_alps_reservation(job *pjob)
  {
  return(0);
  }

reservation_holder::reservation_holder() {}

reservation_holder alps_reservations;

int svr_connect(

  pbs_net_t        hostaddr,  /* host order */
  unsigned int     port,   /* I */
  int             *my_err,
  struct pbsnode  *pnode,
  void           *(*func)(void *))

  {
  return(0);
  }

int svr_clnodes = 0;

void release_req(

  struct work_task *pwt)

  {
  }

int already_recorded(

  const char *rsv_id)

  {
  static int i = 0;
  if (i++ % 2 == 0)
    return(1);
  else
    return(0);
  }

struct pbsnode *get_next_login_node(

  struct prop *needed)

  {
  static struct pbsnode login;
  return(&login);
  }

struct batch_request *alloc_br(

  int type)

  {
  static struct batch_request preq;
  return(&preq);
  }

int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg,
  threadpool_t *tp)

  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int copy_properties(struct pbsnode *dest, struct pbsnode *src)
  {
  return(0);
  }

struct pbsnode *find_node_in_allnodes(all_nodes *an, char *nodename)
  {
  return(NULL);
  }

void delete_a_subnode(

  struct pbsnode *pnode)

  {
  }

int remove_alps_reservation(char *rsv_id)
  {
  removed_reservation++;
  return(0);
  }

int add_execution_slot(

  struct pbsnode *pnode)

  {
  if(pnode == NULL) return(PBSE_RMBADPARAM);
  pnode->nd_slots.add_execution_slot();

  if ((pnode->nd_state & INUSE_JOB) != 0)
    pnode->nd_state &= ~INUSE_JOB;

  return(PBSE_NONE);
  } 

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int decode_depend(pbs_attribute*, char const*, char const*, char const*, int)
  {
  fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
  return 0;
  }

int set_depend(pbs_attribute*, pbs_attribute*, batch_op)
  {
  fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
  return 0;
  }

int comp_depend(pbs_attribute*, pbs_attribute*)
  {
  fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
  return 0;
  }

int set_uacl(pbs_attribute*, pbs_attribute*, batch_op)
  {
  fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
  return 0;
  }

void free_depend(pbs_attribute*)
  {
  fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
  }

int depend_on_que(pbs_attribute*, void*, int)
  {
  fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
  return 0;
  }

int comp_checkpoint(pbs_attribute*, pbs_attribute*)
  {
  fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
  return 0;
  }

int job_set_wait(pbs_attribute*, void*, int)
  {
  fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
  return 0;
  }

int encode_depend(pbs_attribute*, list_link*, char const*, char const*, int, int)
  {
  fprintf(stderr,"The function %s needs to be mocked.\n",__func__);
  return 0;
  }

resource_def svr_resc_def_const[] =
  {
    { "string",decode_str,encode_str,set_str,comp_str,free_str,NULL_FUNC,READ_WRITE,ATR_TYPE_STR},
    { "long",decode_l,encode_l,set_l,comp_l,free_null,NULL_FUNC,NO_USER_SET,ATR_TYPE_LONG},
    { "bit",decode_b,encode_b,set_b,comp_b,free_null,NULL_FUNC,ATR_DFLAG_OPRD | ATR_DFLAG_MGRD | ATR_DFLAG_SvRD | ATR_DFLAG_SvWR | ATR_DFLAG_MOM,ATR_TYPE_LONG},
    { "|unknown|", decode_unkn, encode_unkn, set_unkn, comp_unkn, free_unkn, NULL_FUNC, READ_WRITE, ATR_TYPE_LIST }
  };

int svr_resc_size = sizeof(svr_resc_def_const)/sizeof(resource_def);

resource_def *svr_resc_def = svr_resc_def_const;

int encode_complete_req(
    
  pbs_attribute *attr,
  tlist_head    *phead,
  const char    *atname,
  const char    *rsname,
  int            mode,
  int            perm)

  {
  return(0);
  }

int  decode_complete_req(
    
  pbs_attribute *patr,
  const char    *name,
  const char    *rescn,
  const char    *val,
  int            perm)

  {
  return(0);
  }

int comp_complete_req(
   
  pbs_attribute *attr,
  pbs_attribute *with)

  {
  return(0);
  } // END comp_complete_req()

void free_complete_req(

  pbs_attribute *patr) {}

int set_complete_req(
    
  pbs_attribute *attr,
  pbs_attribute *new_attr,
  enum batch_op  op)
  
  {
  return(0);
  }

#ifdef PENABLE_LINUX_CGROUPS
int Machine::getTotalThreads() const
  {
  return(this->totalThreads);
  }

Machine::~Machine() {}
Socket::~Socket() {}
PCI_Device::~PCI_Device() {}
Chip::~Chip() {}
Core::~Core() {}

Machine::Machine(int np, int numa_nodes, int sockets)
  {
  }

void Machine::setMemory(long long mem)
  {
  this->totalMemory = mem;
  }
#endif

