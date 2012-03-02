#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "server.h" /* server */
#include "resource.h" /* resource, resource_def */
#include "attribute.h" /* pbs_attribute, attribute_def, batch_op */
#include "list_link.h" /* tlist_head */

struct server server;

resource *add_resource_entry(pbs_attribute *pattr, resource_def *prdef)
  {
  fprintf(stderr, "The call to add_resource_entry to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

int encode_unkn(pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_unkn to be mocked!!\n");
  exit(1);
  }

void free_unkn(pbs_attribute *pattr)
  {
  fprintf(stderr, "The call to free_unkn to be mocked!!\n");
  exit(1);
  }

int set_unkn(struct pbs_attribute *old, struct pbs_attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_unkn to be mocked!!\n");
  exit(1);
  }

int decode_unkn(pbs_attribute *patr, char *name, char *rescn, char *value, int perm)
  {
  fprintf(stderr, "The call to decode_unkn to be mocked!!\n");
  exit(1);
  }

int comp_unkn(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_unkn to be mocked!!\n");
  exit(1);
  }

int set_b(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_b to be mocked!!\n");
  exit(1);
  }

int comp_b(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_b to be mocked!!\n");
  exit(1);
  }

int encode_b(pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_b to be mocked!!\n");
  exit(1);
  }

int decode_b(pbs_attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_b to be mocked!!\n");
  exit(1);
  }

int encode_l(pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_l to be mocked!!\n");
  exit(1);
  }

int set_l(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_l to be mocked!!\n");
  exit(1);
  }

int comp_l(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_l to be mocked!!\n");
  exit(1);
  }

int decode_l(pbs_attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_l to be mocked!!\n");
  exit(1);
  }

int encode_str(pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_str to be mocked!!\n");
  exit(1);
  }

int decode_str(pbs_attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_str to be mocked!!\n");
  exit(1);
  }

int set_str(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_str to be mocked!!\n");
  exit(1);
  }

int comp_str(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_str to be mocked!!\n");
  exit(1);
  }

void free_str(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_str to be mocked!!\n");
  exit(1);
  }

int comp_ll(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_ll to be mocked!!\n");
  exit(1);
  }

int set_ll(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_ll to be mocked!!\n");
  exit(1);
  }

int encode_ll(pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_ll to be mocked!!\n");
  exit(1);
  }

int decode_ll(pbs_attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_ll to be mocked!!\n");
  exit(1);
  }

int encode_time(pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_time to be mocked!!\n");
  exit(1);
  }

int decode_time(pbs_attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_time to be mocked!!\n");
  exit(1);
  }

int encode_size(pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_size to be mocked!!\n");
  exit(1);
  }

int decode_size(pbs_attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_size to be mocked!!\n");
  exit(1);
  }

int set_size(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_size to be mocked!!\n");
  exit(1);
  }

int comp_size(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_size to be mocked!!\n");
  exit(1);
  }

void free_null(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_null to be mocked!!\n");
  exit(1);
  }

int decode_tokens(pbs_attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_tokens to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_arst(int index, struct array_strings **arst)
  {
  return(0);
  }

char *threadsafe_tokenizer(char **str, char *delims)
  {
  fprintf(stderr, "The call to threadsafe_tokenizer needs to be mocked!!\n");
  exit(1);
  }
