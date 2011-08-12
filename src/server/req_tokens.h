#ifndef _REQ_TOKENS_H
#define _REQ_TOKENS_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* attribute, array_strings, batch_op */

int token_chk(attribute *pattr, void *pobject, int actmode);
 
int compare_tokens(char *token1, char *token2);

static int chk_dup_token(struct array_strings *old, struct array_strings *new);

int set_tokens(struct attribute *attr, struct attribute *new, enum batch_op op);

#endif /* _REQ_TOKENS_H */
