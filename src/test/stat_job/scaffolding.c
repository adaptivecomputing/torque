#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute_def, svrattrl */
#include "server.h" /* server */
#include "batch_request.h" /* batch_request */
#include "list_link.h" /* list_link */
#include "resource.h" /* list_link */

attribute_def job_attr_def[10];
struct server server;

resource_def svr_resc_def_const[100];
/*
resource_def svr_resc_def_const[] =
  {
    { "arch",
    decode_str,
    encode_str,
    set_str,
    comp_str,
    free_str,
    NULL_FUNC,
    READ_WRITE,
    ATR_TYPE_STR
    },
  { "cpupercent",
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    NO_USER_SET,
    ATR_TYPE_LONG
  },
  { "cput",
    decode_time,
    encode_time,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM | ATR_DFLAG_ALTRUN,
    ATR_TYPE_LONG
  },
  { "file",
    decode_size,
    encode_size,
    set_size,
    comp_size,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM,
    ATR_TYPE_SIZE
  },
  { "mem",
    decode_size,
    encode_size,
    set_size,
    comp_size,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM | ATR_DFLAG_ALTRUN | ATR_DFLAG_RASSN,
    ATR_TYPE_SIZE
  },
  { "pmem",
    decode_size,
    encode_size,
    set_size,
    comp_size,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM | ATR_DFLAG_ALTRUN | ATR_DFLAG_RMOMIG,
    ATR_TYPE_SIZE
  },
  { "ncpus",
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM | ATR_DFLAG_RMOMIG | ATR_DFLAG_RASSN,
    ATR_TYPE_LONG
  },
  { "vmem",
    decode_size,
    encode_size,
    set_size,
    comp_size,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM | ATR_DFLAG_ALTRUN | ATR_DFLAG_RASSN,
    ATR_TYPE_SIZE
  },
  { "pvmem",
    decode_size,
    encode_size,
    set_size,
    comp_size,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM | ATR_DFLAG_RMOMIG,
    ATR_TYPE_SIZE
  },
  { "nice", 
    decode_l,
    encode_l,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM,
    ATR_TYPE_LONG
  },
  { "pcput",
    decode_time,
    encode_time,
    set_l,
    comp_l,
    free_null,
    NULL_FUNC,
    READ_WRITE | ATR_DFLAG_MOM,
    ATR_TYPE_LONG
  },
  }; */
resource_def *svr_resc_def = svr_resc_def_const;
int svr_resc_size = sizeof(svr_resc_def_const) / sizeof(resource_def);

svrattrl *attrlist_create(const char *aname, const char *rname, int vsize)
  {
  fprintf(stderr, "The call to attrlist_create to be mocked!!\n");
  exit(1);
  }

int svr_authorize_jobreq(struct batch_request *preq, job *pjob)
  {
  fprintf(stderr, "The call to svr_authorize_jobreq to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  return(0);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

int remove_procct(

  job *pjob)

  {
  return(0);
  }

void job::encode_plugin_resource_usage(
    
  tlist_head *phead) const

  {
  }
