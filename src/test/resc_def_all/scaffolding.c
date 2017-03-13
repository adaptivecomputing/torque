#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <string.h>

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

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

int encode_unkn(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_unkn to be mocked!!\n");
  exit(1);
  }

void free_unkn(pbs_attribute *pattr)
  {
  fprintf(stderr, "The call to free_unkn to be mocked!!\n");
  exit(1);
  }

int set_unkn(struct pbs_attribute *old, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_unkn to be mocked!!\n");
  exit(1);
  }

int decode_unkn(pbs_attribute *patr, const char *name, const char *rescn, const char *value, int perm)
  {
  fprintf(stderr, "The call to decode_unkn to be mocked!!\n");
  exit(1);
  }

int comp_unkn(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_unkn to be mocked!!\n");
  exit(1);
  }

int set_b(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_b to be mocked!!\n");
  exit(1);
  }

int comp_b(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_b to be mocked!!\n");
  exit(1);
  }

int encode_b(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_b to be mocked!!\n");
  exit(1);
  }

int decode_b(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_b to be mocked!!\n");
  exit(1);
  }

int encode_l(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_l to be mocked!!\n");
  exit(1);
  }

int set_l(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_l to be mocked!!\n");
  exit(1);
  }

int comp_l(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_l to be mocked!!\n");
  exit(1);
  }

int decode_l(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_l to be mocked!!\n");
  exit(1);
  }

int encode_str(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_str to be mocked!!\n");
  exit(1);
  }

int decode_str(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_str to be mocked!!\n");
  exit(1);
  }

int set_str(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
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

int set_ll(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_ll to be mocked!!\n");
  exit(1);
  }

int encode_ll(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_ll to be mocked!!\n");
  exit(1);
  }

int decode_ll(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_ll to be mocked!!\n");
  exit(1);
  }

int encode_time(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_time to be mocked!!\n");
  exit(1);
  }

int decode_time(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_time to be mocked!!\n");
  exit(1);
  }

int encode_size(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_size to be mocked!!\n");
  exit(1);
  }

int decode_size(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_size to be mocked!!\n");
  exit(1);
  }

int set_size(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
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

int decode_tokens(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_tokens to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_arst(int index, struct array_strings **arst)
  {
  return(0);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj) {}

svrattrl *attrlist_create(const char *aname, const char *rname, int vsize)
  {
  return(NULL);
  }

char * csv_find_string(const char *csv_str, const char *search_str)
  {
  fprintf(stderr, "The call to csv_find_string to be mocked!!\n");
  exit(1);
  }

char *csv_nth(const char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth to be mocked!!\n");
  exit(1);
  }

int csv_length(const char *csv_str)
  {
  fprintf(stderr, "The call to csv_length to be mocked!!\n");
  exit(1);
  }

char *threadsafe_tokenizer(

  char **str,    /* M */
  const char  *delims) /* I */

  {
  char *current_char;
  char *start;

  if ((str == NULL) ||
      (*str == NULL))
    return(NULL);

  /* save start position */
  start = *str;

  /* return NULL at the end of the string */
  if (*start == '\0')
    return(NULL);

  /* begin at the start */
  current_char = start;

  /* advance to the end of the string or until you find a delimiter */
  while ((*current_char != '\0') &&
      (!strchr(delims, *current_char)))
    current_char++;

  /* advance str */
  if (*current_char != '\0')
    {
    /* not at the end of the string */
    *str = current_char + 1;
    *current_char = '\0';
    }
  else
    {
    /* at the end of the string */
    *str = current_char;
    }

  return(start);
  } /* END threadsafe_tokenizer() */

long time_str_to_seconds(const char *str)
  {
  return(0);
  }
