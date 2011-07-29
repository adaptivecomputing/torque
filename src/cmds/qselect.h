#include "license_pbs.h" /* See here for the software license */

void set_attrop(struct attropl **list, char *a_name, char *r_name, char *v_name, enum batch_op op);

void check_op(char *optarg, enum batch_op *op, char *optargout);

int check_res_op(char *optarg, char *resource_name, enum batch_op *op, char *resource_value, char **res_pos);

/* main */
