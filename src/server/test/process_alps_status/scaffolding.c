#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "dynamic_string.h"
#include "batch_request.h"
#include "work_task.h"
#include "attribute.h"
#include "u_tree.h"


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

int LOGLEVEL = 0;
all_nodes allnodes;
struct node_state
  {
  short  bit;
  char  *name;
  } ns[] =

{
    {INUSE_UNKNOWN, ND_state_unknown},
    {INUSE_DOWN,    ND_down},
    {INUSE_OFFLINE, ND_offline},
    {INUSE_RESERVE, ND_reserve},
    {INUSE_JOB,     ND_job_exclusive},
    {INUSE_JOBSHARE, ND_job_sharing},
    {INUSE_BUSY,    ND_busy},
    {0,             NULL}
};

void log_record(int eventtype, int objclass, const char *objname, char *msg) {}
void log_event(int eventtype, int objclass, const char *objname, char *text) {}
void log_err(int errnum, const char *routine, char *text) {}
void lock_node(struct pbsnode *pnode, const char *method, char *msg, int log_level) {}
void unlock_node(struct pbsnode *pnode, const char *method, char *msg, int log_level) {}




size_t need_to_grow(

  dynamic_string *ds,
  const char     *to_check)

  {
  size_t to_add = strlen(to_check) + 1;
  size_t to_grow = 0;

  if (ds->size < ds->used + to_add)
    {
    to_grow = to_add + ds->size;

    if (to_grow < (ds->size * 4))
      to_grow = ds->size * 4;
    }

  return(to_grow);
  } /* END need_to_grow() */



int resize_if_needed(

  dynamic_string *ds,
  const char     *to_check)

  {
  size_t  new_size = need_to_grow(ds, to_check);
  size_t  difference;
  char   *tmp;

  if (new_size > 0)
    {
    /* need to resize */
    difference = new_size - ds->size;

    if ((tmp = realloc(ds->str, new_size)) == NULL)
      return(ENOMEM);

    ds->str = tmp;
    /* zero out the new space as well */
    memset(ds->str + ds->size, 0, difference);
    ds->size = new_size;
    }

  return(PBSE_NONE);
  } /* END resize_if_needed() */




/*
 * appends to a string, resizing if necessary
 *
 * @param ds - the string to append to
 * @param to_append - the string being appended
 */
int append_dynamic_string(
    
  dynamic_string *ds,        /* M */
  const char     *to_append) /* I */

  {
  int len = strlen(to_append);
  int add_one = FALSE;
  int offset = ds->used;

  if (ds->used == 0)
    add_one = TRUE;
  else
    offset -= 1;

  resize_if_needed(ds, to_append);
  strcat(ds->str + offset, to_append);
    
  ds->used += len;

  if (add_one == TRUE)
    ds->used += 1;

  return(PBSE_NONE);
  } /* END append_dynamic_string() */



/* 
 * initializes a dynamic string and returns it, or NULL if there is no memory 
 *
 * @param initial_size - the initial size of the string, use default if -1
 * @param str - the initial string to place in the dynamic string if not NULL
 * @return - the dynamic string object or NULL if no memory
 */
dynamic_string *get_dynamic_string(
    
  int         initial_size, /* I (-1 means default) */
  const char *str)          /* I (optional) */

  {
  dynamic_string *ds = calloc(1, sizeof(dynamic_string));

  if (ds == NULL)
    return(ds);

  if (initial_size > 0)
    ds->size = initial_size;
  else
    ds->size = DS_INITIAL_SIZE;
    
  ds->str = calloc(1, ds->size);

  if (ds->str == NULL)
    {
    free(ds);
    return(NULL);
    }
    
  /* initialize empty str */
  ds->used = 0;

  /* add the string if it exists */
  if (str != NULL)
    {
    if (append_dynamic_string(ds,str) != PBSE_NONE)
      {
      free_dynamic_string(ds);
      return(NULL);
      }
    }

  return(ds);
  } /* END get_dynamic_string() */



/* nothing needed for this function */
int save_node_status(struct pbsnode *np, pbs_attribute  *temp)

  {
  return(0);
  } /* END save_node_status() */





struct pbssubn *create_subnode(

  struct pbsnode *pnode)

  {
  pnode->nd_nsn++;
  pnode->nd_nsnfree++;

  return((struct pbssubn *)1);
  }  /* END create_subnode() */



void update_node_state(

  struct pbsnode *np,         /* I (modified) */
  int             newstate)   /* I (one of INUSE_*) */

  {
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

  char *nodename) /* I */

  {
  static struct pbsnode pnode;
  static int    initialized = 0;

  if (initialized == 0)
    {
    memset(&pnode, 0, sizeof(struct pbsnode));
    pnode.nd_name = strdup("george");
    pnode.alps_subnodes.allnodes_mutex = calloc(1, sizeof(pthread_mutex_t));
    pthread_mutex_init(pnode.alps_subnodes.allnodes_mutex, NULL);
    }

  initialized++;

  return(&pnode);
  }  /* END find_nodebyname() */




/* 
 * frees a dynamic string 
 *
 * @param ds - the dynamic string being freed 
 */
void free_dynamic_string(
    
  dynamic_string *ds) /* M */

  {
  free(ds->str);
  free(ds);
  } /* END free_dynamic_string() */



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



void update_subnode(

  struct pbsnode *pnode)

  {
  struct  pbssubn *pnxtsn;
  unsigned short   state;

  state = pnode->nd_state & INUSE_COMMON_MASK;

  for (pnxtsn = pnode->nd_psn;pnxtsn;pnxtsn = pnxtsn->next)
    {
    pnxtsn->host  = pnode;

    pnxtsn->inuse = (pnxtsn->inuse & ~INUSE_COMMON_MASK) | state;
    }

  return;
  }  /* END update_subnode() */


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

  if (SBM & (INUSE_JOBSHARE))
    {
    len = strlen(ND_job_sharing) + 1;

    if (len < BufSize)
      {
      if (Buf[0] != '\0')
        strcat(Buf, ",");
      else
        len--;

      strcat(Buf, ND_job_sharing);
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
  int             mode)  /*passed to attrib's action func not used by 
                             this func at this time*/

  {
  static int count = 0;

  count++;

  if (count < 2)
    return(0);
  else
    return(1);
  }  /* END mgr_set_node_attr() */



/* nothing needed for this function */
int decode_arst(pbs_attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  return(0);
  }  /* END decode_arst() */




int attr_atomic_node_set(

  struct svrattrl *plist,    /* list of pbs_attribute modif structs */
  pbs_attribute   *old,      /* unused */
  pbs_attribute   *new,      /* new pbs_attribute array begins here */
  attribute_def   *pdef,     /* begin array  definition structs */
  int              limit,    /* number elts in definition array */
  int              unkn,     /* <0 unknown attrib not permitted */
  int              privil,   /* requester's access privileges   */
  int             *badattr)  /* return list position wher bad   */

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

    (new + index)->at_flags &= ~ATR_VFLAG_MODIFY;

    if ((plist->al_op != INCR) && (plist->al_op != DECR) &&
        (plist->al_op != SET) && (plist->al_op != INCR_OLD))
      plist->al_op = SET;


    if (temp.at_flags & ATR_VFLAG_SET)
      {
      /* "temp" has a data value, use it to update "new" */

      if ((rc = (pdef + index)->at_set(new + index, &temp, plist->al_op)) != 0)
        {
        (pdef + index)->at_free(&temp);
        break;
        }
      }
    else if (temp.at_flags & ATR_VFLAG_MODIFY)
      {

      (pdef + index)->at_free(new + index);
      (new + index)->at_flags |= ATR_VFLAG_MODIFY;
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

    char *s1,  /* I */
    char *s2)

  {
  return(0);
  }



int find_attr(

    struct attribute_def *attr_def, /* ptr to pbs_attribute definitions */
    char                 *name,     /* pbs_attribute name to find */
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

    pbs_attribute *new,      /* derive status into this pbs_attribute*/
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

        rc = set_arst(new, &temp, SET);
        }
      else
        {
        /* node has no properties, setup empty pbs_attribute */

        new->at_val.at_arst = NULL;
        new->at_flags       = 0;
        new->at_type        = ATR_TYPE_ARST;
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

      np->nd_gpustatus = new->at_val.at_arst;

      new->at_val.at_arst = NULL;
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



int get_value_hash(

  hash_table_t *ht,
  void         *key)
  
  {
  if (((char *)key)[0] == 'n')
    return(1);
  else
    return(-1);
  } /* END get_value_hash() */



int set_arst(

  pbs_attribute *attr,  /* I/O */
  pbs_attribute *new,   /* I */
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
  newpas = new->at_val.at_arst;

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

      if (new->at_val.at_arst == (struct array_strings *)0)
        break; /* none to set */

      nsize = newpas->as_next - newpas->as_buf; /* space needed */

      if (nsize > pas->as_bufsize)
        {
        /* new data won't fit */

        if (pas->as_buf)
          free(pas->as_buf);

        nsize += nsize / 2;   /* alloc extra space */

        if (!(pas->as_buf = calloc(1, (size_t)nsize)))
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
          pc = realloc(pas->as_buf, (size_t)need);
          if (pc != NULL)
            memset(pc + pas->as_bufsize, 0, need - pas->as_bufsize);
          }
        else
          pc = calloc(1, (size_t)need);

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
      pc = calloc(1, need);

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

  pbs_attribute *new,           /*derive status into this pbs_attribute*/
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

        rc = set_arst(new, &temp, SET);
        }
      else
        {
        /* node has no properties, setup empty pbs_attribute */

        new->at_val.at_arst = NULL;
        new->at_flags       = 0;
        new->at_type        = ATR_TYPE_ARST;
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

      np->nd_status = new->at_val.at_arst;

      new->at_val.at_arst = NULL;
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
  int             ntype) /* time-shared or cluster */

  {
  memset(pnode, 0, sizeof(struct pbsnode));

  pnode->nd_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  if (pnode->nd_mutex == NULL)
    {
    log_err(ENOMEM, __func__, "Could not allocate memory for the node's mutex");

    return(ENOMEM);
    }

  pthread_mutex_init(pnode->nd_mutex,NULL);

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



int check_and_resize(

  resizable_array *ra)

  {
  static char *id = "check_and_resize";
  slot        *tmp;
  size_t       remaining;
  size_t       size;

  if (ra->max == ra->num + 1)
    {
    /* double the size if we're out of space */
    size = (ra->max * 2) * sizeof(slot);

    if ((tmp = realloc(ra->slots,size)) == NULL)
      {
      log_err(ENOMEM,id,"No memory left to resize the array");
      return(ENOMEM);
      }

    remaining = ra->max * sizeof(slot);

    memset(tmp + ra->max, 0, remaining);

    ra->slots = tmp;

    ra->max = ra->max * 2;
    }

  return(PBSE_NONE);
  } /* END check_and_resize() */




void update_next_slot(

  resizable_array *ra) /* M */
  
  {
  while ((ra->next_slot < ra->max) &&
      (ra->slots[ra->next_slot].item != NULL))
    ra->next_slot++;
  } /* END update_next_slot() */




int insert_thing(

    resizable_array *ra,
    void             *thing)

  {
  int rc;

  /* check if the array must be resized */
  if ((rc = check_and_resize(ra)) != PBSE_NONE)
    {
    return(-1);
    }

  ra->slots[ra->next_slot].item = thing;

  /* save the insertion point */
  rc = ra->next_slot;

  /* handle the backwards pointer, next pointer is left at zero */
  ra->slots[rc].prev = ra->last;

  /* make sure the empty slot points to the next occupied slot */
  if (ra->last == ALWAYS_EMPTY_INDEX)
    {
    ra->slots[ALWAYS_EMPTY_INDEX].next = rc;
    }

  /* update the last index */
  ra->slots[ra->last].next = rc;
  ra->last = rc;

  /* update the new item's next index */
  ra->slots[rc].next = ALWAYS_EMPTY_INDEX;

  /* increase the count */
  ra->num++;

  update_next_slot(ra);

  return(rc);
  } /* END insert_thing() */



int comp_null(struct pbs_attribute *attr, struct pbs_attribute *with)

  {
  return(0);
  }

int decode_ntype(pbs_attribute *pattr, char *name, char *rescn, char *val, int perm)
  {
  return(0);
  }

int encode_l(pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  return(0);
  }

int encode_str(pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  return(0);
  }

int decode_null(pbs_attribute *patr, char *name, char *rn, char *val, int perm)
  {
  return(0);
  }

uint32_t hashbig( const void *key, size_t length, uint32_t initval)
    {
    uint32_t a,b,c;
    union { const void *ptr; size_t i; } u; /* to cast key to (size_t) happily */
    
    /* Set up the internal state */
    a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;
    
    u.ptr = key;
    if (HASH_BIG_ENDIAN && ((u.i & 0x3) == 0))
      {
      const uint32_t *k = (const uint32_t *)key;         /* read 32-bit chunks */
      const uint8_t  *k8;
      
      /*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
      while (length > 12)
        {
        a += k[0];
        b += k[1];
        c += k[2];
        mix(a,b,c);
        length -= 12;
        k += 3;
        }
      
      /*----------------------------- handle the last (probably partial) block */
      /* 
       * "k[2]<<8" actually reads beyond the end of the string, but
       * then shifts out the part it's not allowed to read.  Because the
       * string is aligned, the illegal read is in the same word as the
       * rest of the string.  Every machine with memory protection I've seen
       * does it on word boundaries, so is OK with this.  But VALGRIND will
       * still catch it and complain.  The masking trick does make the hash
       * noticably faster for short strings (like English words).
       * dbeer- also using only the valgrind friendly code 
       */
      
      k8 = (const uint8_t *)k;
      switch(length)                   /* all the case statements fall through */
        {
        case 12:
          c+=k[2];
          b+=k[1];
          a+=k[0];
          break;
        case 11:
          c+=((uint32_t)k8[10])<<8;  /* fall through */
        case 10:
          c+=((uint32_t)k8[9])<<16;  /* fall through */
        case 9:
          c+=((uint32_t)k8[8])<<24;  /* fall through */
        case 8:
          b+=k[1]; a+=k[0];
          break;
        case 7:
          b+=((uint32_t)k8[6])<<8;   /* fall through */
        case 6:
          b+=((uint32_t)k8[5])<<16;  /* fall through */
        case 5:
          b+=((uint32_t)k8[4])<<24;  /* fall through */
        case 4:
          a+=k[0]; break;
        case 3:
          a+=((uint32_t)k8[2])<<8;   /* fall through */
        case 2:
          a+=((uint32_t)k8[1])<<16;  /* fall through */
        case 1:
          a+=((uint32_t)k8[0])<<24; break;
        case 0:
          return c;
        }
      }
    else
      {                        /* need to read the key one byte at a time */
      const uint8_t *k = (const uint8_t *)key;

      /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
      while (length > 12)
        {
        a += ((uint32_t)k[0])<<24;
        a += ((uint32_t)k[1])<<16;
        a += ((uint32_t)k[2])<<8;
        a += ((uint32_t)k[3]);
        b += ((uint32_t)k[4])<<24;
        b += ((uint32_t)k[5])<<16;
        b += ((uint32_t)k[6])<<8;
        b += ((uint32_t)k[7]);
        c += ((uint32_t)k[8])<<24;
        c += ((uint32_t)k[9])<<16;
        c += ((uint32_t)k[10])<<8;
        c += ((uint32_t)k[11]);
        mix(a,b,c);
        length -= 12;
        k += 12;
        }

      /*-------------------------------- last block: affect all 32 bits of (c) */
      switch(length)                   /* all the case statements fall through */
        {
        case 12:
          c+=k[11];
        case 11:
          c+=((uint32_t)k[10])<<8;
        case 10:
          c+=((uint32_t)k[9])<<16;
        case 9:
          c+=((uint32_t)k[8])<<24;
        case 8:
          b+=k[7];
        case 7:
          b+=((uint32_t)k[6])<<8;
        case 6:
          b+=((uint32_t)k[5])<<16;
        case 5:
          b+=((uint32_t)k[4])<<24;
        case 4:
          a+=k[3];
        case 3:
          a+=((uint32_t)k[2])<<8;
        case 2:
          a+=((uint32_t)k[1])<<16;
        case 1:
          a+=((uint32_t)k[0])<<24;
          break;
        case 0:
          return c;
        }
      }

    final(a,b,c);
    return c;
    } /* END hashbig() */


int get_hash(

  hash_table_t *ht,
  void         *key)

  {
  /* since key is a job name, just get the numbers from the front */
  size_t   len = strlen(key);
  uint32_t hash;
  uint32_t mask = 0;
  uint32_t counter = ht->size >> 1;

  hash = hashbig(key,len,0);

  for (mask = 0; counter != 0; counter = counter >> 1)
    mask |= counter;

  return(hash & mask);
  } /* END get_hash() */


void add_to_bucket(

    bucket **buckets,
    int      index,
    char    *key,
    int      value)

  {
  if (buckets[index] == NULL)
    {
    /* empty bucket, just add it here */
    buckets[index] = calloc(1, sizeof(bucket));
    buckets[index]->value = value;
    buckets[index]->key   = key;
    buckets[index]->next  = NULL;
    }
  else
    {
    /* make this the last bucket at the end */
    bucket *b = buckets[index];

    while (b->next != NULL)
      b = b->next;

    b->next = calloc(1, sizeof(bucket));
    b->next->value = value;
    b->next->key   = key;
    b->next->next  = NULL;
    }
  } /* END add_to_bucket() */




void free_buckets(

  bucket **buckets,
  int      size)

  {
  int     i;
  bucket *b;
  bucket *tmp;

  for (i = 0; i < size; i++)
    {
    b = buckets[i];

    while (b != NULL)
      {
      tmp = b;
      b = b->next;

      free(tmp);
      }
    }
  } /* END free_buckets() */



int add_hash(hash_table_t *ht, int value, void *key)
  {
  int index;

  /* check if we need to rehash */
  if (ht->size == ht->num)
    {
    int      old_bucket_size = ht->size;
    int      new_bucket_size = ht->size * 2;
    size_t   amount = new_bucket_size * sizeof(bucket *);
    bucket **tmp = calloc(1, amount);
    int      i;
    int      new_index;

    ht->size = new_bucket_size;

    /* rehash all of the hold values */
    for (i = 0; i < old_bucket_size; i++)
      {
      bucket *b = ht->buckets[i];

      while (b != NULL)
        {
        new_index = get_hash(ht,b->key);

        add_to_bucket(tmp,new_index,b->key,b->value);

        b = b->next;
        }
      } /* END foreach (old buckets) */

    free_buckets(ht->buckets, old_bucket_size);

    free(ht->buckets);

    ht->buckets = tmp;
    } /* END if (need to rehash) */

  index = get_hash(ht,key);

  add_to_bucket(ht->buckets,index,key,value);

  ht->num++;

  return(PBSE_NONE);
  }

int append_char_to_dynamic_string(
    
  dynamic_string *ds,
  char            c)

  {
  char str[2];
  str[0] = c;
  str[1] = '\0';

  return(append_dynamic_string(ds, str));
  } /* END append_char_to_dynamic_string() */

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

int decode_str(pbs_attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  return(0);
  }

int set_node_ntype(pbs_attribute *pattr, pbs_attribute *new, enum batch_op op)
  {
  return(0);
  }

int node_mom_rm_port_action(pbs_attribute *new, void *pobj, int actmode)
  {
  return(0);
  }

int set_null (pbs_attribute * patr, pbs_attribute * new, enum batch_op op)
  {
  return(0);
  }

int set_note_str(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op)
  {
  return(0);
  }

int decode_state(pbs_attribute *pattr, char *name, char *rescn, char *val, int perm)
  {
  return(0);
  }

int encode_jobs(pbs_attribute *pattr, tlist_head *ph, char *aname, char *rname, int mode, int perm)
  {
  return(0);
  }

int set_str(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op)
  {
  return(0);
  }

int node_prop_list(pbs_attribute *new, void *pnode, int actmode)
  {
  return(0);
  }

int node_state(pbs_attribute *new, void *pnode, int actmode)
  {
  return(0);
  }

int set_l(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op)
  {
  return(0);
  }

int numa_str_action(pbs_attribute *new, void *pnode, int actmode)
  {
  return(0);
  }

void free_str(struct pbs_attribute *attr) {}

int encode_ntype(pbs_attribute *pattr, tlist_head *ph, char *aname, char *rname, int mode, int perm)
  {
  return(0);
  }

int node_note(pbs_attribute *new, void *pnode, int actmode)
  {
  return(0);
  }

int set_node_state(pbs_attribute *pattr, pbs_attribute *new, enum batch_op op)
  {
  return(0);
  }

int node_np_action(pbs_attribute *new, void *pobj, int actmode)
  {
  return(0);
  }

int gpu_str_action(pbs_attribute *new, void *pnode, int actmode)
  {
  return(0);
  }

int comp_str(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  return(0);
  }

int node_numa_action(pbs_attribute *new, void *pnode, int actmode)
  {
  return(0);
  }

int decode_l( pbs_attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  return(0);
  }

int encode_state(pbs_attribute *pattr, tlist_head *ph, char *aname, char *rname, int mode, int perm)
  {
  return(0);
  }

int node_mom_port_action(pbs_attribute *new, void *pobj, int actmode)
  {
  return(0);
  }

int node_ntype(pbs_attribute *new, void *pnode, int actmode)
  {
  return(0);
  }

int node_gpus_action(pbs_attribute *new, void *pnode, int actmode)
  {
  return(0);
  }

int encode_arst( pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  return(0);
  }

void free_null(struct pbs_attribute *attr) {}

int is_orphaned(

  char *rsv_id)

  {
  return(1);
  }

job *svr_find_job(char *jobid)
  {
  static struct job pjob;
  static int    i = 0;

  if (i == 0)
    {
    pjob.ji_mutex = calloc(1, sizeof(pthread_mutex_t));
    }
  i++;

  return(&pjob);
  }

void free_br(struct batch_request *preq) {}

int issue_Drequest(

  int                    conn,
  struct batch_request  *request,
  void                 (*func) (struct work_task *),
  struct work_task     **ppwt)

  {
  return(0);
  }

pbs_net_t get_hostaddr(

  int  *local_errno, /* O */    
  char *hostname)    /* I */

  {
  return(0);
  }

int create_alps_reservation(

  char       *exec_hosts,
  char       *username,
  char       *jobid,
  char       *apbasil_path,
  char       *apbasil_protocol,
  long long   pagg_id_value,
  char      **reservation_id)

  {
  return(0);
  }

int svr_connect(

  pbs_net_t        hostaddr,  /* host order */
  unsigned int     port,   /* I */
  int             *my_err,
  struct pbsnode  *pnode,
  void           *(*func)(void *),
  enum conn_type   cntype)

  {
  return(0);
  }

int svr_clnodes = 0;

void release_req(

  struct work_task *pwt)

  {
  }

int already_recorded(

  char *rsv_id)

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
  void *arg)

  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }
