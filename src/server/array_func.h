#ifndef _ARRAY_FUNC_H
#define _ARRAY_FUNC_H
#include "license_pbs.h" /* See here for the software license */

#include "array.h" /* job_array, ArrayEventsEnum */
#include "pbs_job.h" /* job */
#include "attribute.h" /* pbs_attribute, svrattrl */
#include "batch_request.h" /* batch_request */


int is_array(char *id);

int array_save(job_array *pa);

void array_get_parent_id(char *job_id, char *parent_id);

int set_slot_limit(char *request, job_array *pa);

int setup_array_struct(job *pjob);

/* static int is_num(char *str); */

/* static int array_request_parse_token(char *str, int *start, int *end); */

/* static int parse_array_request(char *request, tlist_head *tl); */

int delete_array_range(job_array *pa, char *range_str);

int first_job_index(job_array *pa);

int delete_whole_array(job_array *pa);

int hold_array_range(job_array *pa, char *range_str, pbs_attribute *temphold);

int release_array_range(job_array *pa, struct batch_request *preq, char *range_str);

int modify_array_range(job_array *pa, char *range, svrattrl *plist, struct batch_request *preq, int checkpoint_req);

void initialize_all_arrays_array();

int insert_array(job_array *pa);

int remove_array(job_array *pa);

job_array *next_array(int *iter);

#endif /* _ARRAY_FUNC_H */
