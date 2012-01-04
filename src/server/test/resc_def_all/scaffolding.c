#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "server.h" /* server */
#include "resource.h" /* resource, resource_def */
#include "attribute.h" /* attribute, attribute_def, batch_op */
#include "list_link.h" /* tlist_head */

struct server server;

resource *add_resource_entry(attribute *pattr, resource_def *prdef)
  {
  fprintf(stderr, "The call to add_resource_entry to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

int encode_unkn(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_unkn to be mocked!!\n");
  exit(1);
  }

void free_unkn(attribute *pattr)
  {
  fprintf(stderr, "The call to free_unkn to be mocked!!\n");
  exit(1);
  }

int set_unkn(struct attribute *old, struct attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_unkn to be mocked!!\n");
  exit(1);
  }

int decode_unkn(attribute *patr, char *name, char *rescn, char *value, int perm)
  {
  fprintf(stderr, "The call to decode_unkn to be mocked!!\n");
  exit(1);
  }

int comp_unkn(struct attribute *attr, struct attribute *with)
  {
  fprintf(stderr, "The call to comp_unkn to be mocked!!\n");
  exit(1);
  }

int set_b(struct attribute *attr, struct attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_b to be mocked!!\n");
  exit(1);
  }

int comp_b(struct attribute *attr, struct attribute *with)
  {
  fprintf(stderr, "The call to comp_b to be mocked!!\n");
  exit(1);
  }

int encode_b(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_b to be mocked!!\n");
  exit(1);
  }

int decode_b(attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_b to be mocked!!\n");
  exit(1);
  }

int encode_l(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_l to be mocked!!\n");
  exit(1);
  }

int set_l(struct attribute *attr, struct attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_l to be mocked!!\n");
  exit(1);
  }

int comp_l(struct attribute *attr, struct attribute *with)
  {
  fprintf(stderr, "The call to comp_l to be mocked!!\n");
  exit(1);
  }

int decode_l(attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_l to be mocked!!\n");
  exit(1);
  }

int encode_str(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_str to be mocked!!\n");
  exit(1);
  }

int decode_str(attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_str to be mocked!!\n");
  exit(1);
  }

int set_str(struct attribute *attr, struct attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_str to be mocked!!\n");
  exit(1);
  }

int comp_str(struct attribute *attr, struct attribute *with)
  {
  fprintf(stderr, "The call to comp_str to be mocked!!\n");
  exit(1);
  }

void free_str(struct attribute *attr)
  {
  fprintf(stderr, "The call to free_str to be mocked!!\n");
  exit(1);
  }

int comp_ll(struct attribute *attr, struct attribute *with)
  {
  fprintf(stderr, "The call to comp_ll to be mocked!!\n");
  exit(1);
  }

int set_ll(struct attribute *attr, struct attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_ll to be mocked!!\n");
  exit(1);
  }

int encode_ll(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_ll to be mocked!!\n");
  exit(1);
  }

int decode_ll(attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_ll to be mocked!!\n");
  exit(1);
  }

int encode_time(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_time to be mocked!!\n");
  exit(1);
  }

int decode_time(attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_time to be mocked!!\n");
  exit(1);
  }

int encode_size(attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_size to be mocked!!\n");
  exit(1);
  }

int decode_size(attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_size to be mocked!!\n");
  exit(1);
  }

int set_size(struct attribute *attr, struct attribute *new, enum batch_op op)
  {
  fprintf(stderr, "The call to set_size to be mocked!!\n");
  exit(1);
  }

int comp_size(struct attribute *attr, struct attribute *with)
  {
  fprintf(stderr, "The call to comp_size to be mocked!!\n");
  exit(1);
  }

void free_null(struct attribute *attr)
  {
  fprintf(stderr, "The call to free_null to be mocked!!\n");
  exit(1);
  }

int decode_tokens(attribute *patr, char *name, char *rescn, char *val, int perm)
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
