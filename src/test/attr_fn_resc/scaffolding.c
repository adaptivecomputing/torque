#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "list_link.h" /* list_link */
#include "resource.h" /* resource_def */

resource_def svr_resc_def_const[] =
  {
    { "string",decode_str,encode_str,set_str,comp_str,free_str,NULL_FUNC,READ_WRITE,ATR_TYPE_STR},
    { "long",decode_l,encode_l,set_l,comp_l,free_null,NULL_FUNC,NO_USER_SET,ATR_TYPE_LONG},
    { "bit",decode_b,encode_b,set_b,comp_b,free_null,NULL_FUNC,ATR_DFLAG_OPRD | ATR_DFLAG_MGRD | ATR_DFLAG_SvRD | ATR_DFLAG_SvWR | ATR_DFLAG_MOM,ATR_TYPE_LONG},
    { "|unknown|", decode_unkn, encode_unkn, set_unkn, comp_unkn, free_unkn, NULL_FUNC, READ_WRITE, ATR_TYPE_LIST }
  };

int svr_resc_size = sizeof(svr_resc_def_const)/sizeof(resource_def);
resource_def *svr_resc_def = svr_resc_def_const;

int ctnodes(const char *spec)
  {
  fprintf(stderr, "The call to ctnodes needs to be mocked!!\n");
  exit(1);
  }

#if 0
void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  } 

void insert_link(struct list_link *old, struct list_link *new_link, void *pobj, int position) 
  {
  fprintf(stderr, "The call to insert_link needs to be mocked!!\n");
  exit(1);
  } 

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  } 

void append_link(tlist_head *head, list_link *new_link, void *pobj)
 {
 fprintf(stderr, "The call to append_link needs to be mocked!!\n");
 exit(1);
 }

#endif
