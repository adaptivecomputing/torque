#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "req_register.h"
#include "test_uut.h"
#include "pbs_error.h"
#include "pbs_job.h"
#include "batch_request.h"
#include "attribute.h"


int check_dependency_job(char *jobid, batch_request *preq, job **job_ptr);
void clear_depend(struct depend *pd, int type, int exist);
depend *find_depend(int type, pbs_attribute *pattr);
depend *make_depend(int type, pbs_attribute *pattr);
depend_job *make_dependjob(struct depend *pdep, const char *jobid);
depend_job *find_dependjob(struct depend *pdep, const char *name);
int register_sync(struct depend *pdep, char *child, char *host, long cost);
int register_dep(pbs_attribute *pattr, batch_request *preq, int type, int *made);
int unregister_dep(pbs_attribute *pattr, batch_request *preq);
int comp_depend(pbs_attribute *a1, pbs_attribute *a2);
void free_depend(pbs_attribute *pattr);
int build_depend(pbs_attribute *pattr, const char *value);
void fast_strcat(char **Dest, const char *Src);
int dup_depend(pbs_attribute *pattr, struct depend *pd);
void cat_jobsvr(char **Dest, const char *Src);
int decode_depend(pbs_attribute *pattr, const char *name, const char *rescn, const char *val, int perm);
int encode_depend(pbs_attribute *pattr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm);
int set_depend(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op op);
int unregister_sync(pbs_attribute *attr, batch_request *preq);
int register_before_dep(batch_request *preq, job *pjob, int type);
int register_dependency(batch_request *preq, job *pjob, int type);
int release_before_dependency(batch_request *preq, job *pjob, int type);
int release_syncwith_dependency(batch_request *preq, job *pjob);
void set_depend_hold(job *pjob, pbs_attribute *pattr, job_array *);
int delete_dependency_job(batch_request *preq, job **pjob_ptr);
int req_register(batch_request *preq);
bool remove_array_dependency_job_from_job(struct array_depend *pdep, job *pjob, char *job_array_id);
void removeAfterAnyDependency(const char *pJobID, const char *targetJob);
bool job_ids_match(const char *parent, const char *child);
int depend_on_que(pbs_attribute *pattr, void *pj, int mode);


extern char  server_name[];
extern int   i;
extern int   svr;
extern int   is_attr_set;
extern int   job_aborted;

char          *job1 = (char *)"1.napali";
char          *job2 = (char *)"2.napali";
char          *host = (char *)"napali";


void initialize_depend_attr(

  pbs_attribute *pattr)

  {
  memset(pattr, 0, sizeof(pbs_attribute));
  CLEAR_HEAD(pattr->at_val.at_list);
  } /* END initialize_depend_attr() */


START_TEST(test_depend_on_que)
  {
  job       *pjob = (job *)calloc(1, sizeof(job));
  depend    *pdep;
  initialize_depend_attr(pjob->ji_wattr + JOB_ATR_depend);
  pdep = make_depend(JOB_DEPEND_TYPE_AFTERNOTOK, pjob->ji_wattr + JOB_ATR_depend);
  make_dependjob(pdep, job2);

  // These three modes should do nothing
  fail_unless(depend_on_que(pjob->ji_wattr + JOB_ATR_depend, pjob, ATR_ACTION_NEW) == PBSE_NONE);
  fail_unless(depend_on_que(pjob->ji_wattr + JOB_ATR_depend, pjob, ATR_ACTION_FREE) == PBSE_NONE);
  fail_unless(depend_on_que(pjob->ji_wattr + JOB_ATR_depend, pjob, ATR_ACTION_RECOV) == PBSE_NONE);

  fail_unless(depend_on_que(pjob->ji_wattr + JOB_ATR_depend, pjob, ATR_ACTION_ALTER) == PBSE_NONE);
  fail_unless((pjob->ji_wattr[JOB_ATR_hold].at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pjob->ji_qs.ji_state == JOB_STATE_HELD);
  }
END_TEST


START_TEST(test_job_ids_match)
  {
  is_attr_set = 1;
  fail_unless(job_ids_match("0", "0.napali") == true);
  is_attr_set = 0;
  fail_unless(job_ids_match("1.napali", "1.napali") == true);
  fail_unless(job_ids_match("11.napali", "1.napali") == false);
  }
END_TEST


START_TEST(remove_array_dependency_from_job_test)
  {
  job                 *pjob = (job *)calloc(1, sizeof(job));
  struct depend       *pdep;
  struct depend_job   *d1;
  struct array_depend  array_dep;

  initialize_depend_attr(&pjob->ji_wattr[JOB_ATR_depend]);
  pdep = make_depend(JOB_DEPEND_TYPE_AFTEROKARRAY, &pjob->ji_wattr[JOB_ATR_depend]);
  fail_unless((d1 = make_dependjob(pdep, job1)) != NULL, "didn't create dep 1");

  array_dep.dp_type = JOB_DEPEND_TYPE_AFTEROKARRAY;
  fail_unless(remove_array_dependency_job_from_job(&array_dep, pjob, job1) == true);
  }
END_TEST


START_TEST(set_array_depend_holds_test)
  {
  batch_request *preq = (batch_request *)calloc(1, sizeof(batch_request));
  job_array     *pa = (job_array *)calloc(1, sizeof(job_array));

  strcpy(preq->rq_ind.rq_register.rq_child, job1);
  strcpy(preq->rq_ind.rq_register.rq_svr, host);

  CLEAR_HEAD(pa->ai_qs.deps);
  fail_unless(register_array_depend(pa, preq, JOB_DEPEND_TYPE_AFTEROKARRAY, 10) == PBSE_NONE);
  pa->ai_qs.num_successful = 12;
  fail_unless(set_array_depend_holds(pa) == true);

  // Make sure we abort the job when the dependency can't be fulfilled
  job_aborted = 0;
  CLEAR_HEAD(pa->ai_qs.deps);
  fail_unless(register_array_depend(pa, preq, JOB_DEPEND_TYPE_AFTEROKARRAY, 1) == PBSE_NONE);
  pa->ai_qs.num_successful = 0;
  pa->ai_qs.num_failed = 10;
  pa->ai_qs.jobs_done = 10;
  pa->ai_qs.num_jobs = 10;
  fail_unless(set_array_depend_holds(pa) == false);
  fail_unless(job_aborted == 1);
  
  // Make sure we don't abort a job just because the dependency isn't fulfilled
  CLEAR_HEAD(pa->ai_qs.deps);
  fail_unless(register_array_depend(pa, preq, JOB_DEPEND_TYPE_AFTERNOTOKARRAY, 1) == PBSE_NONE);
  pa->ai_qs.num_successful = 10;
  pa->ai_qs.num_failed = 0;
  pa->ai_qs.jobs_done = 10;
  pa->ai_qs.num_jobs = 20;
  fail_unless(set_array_depend_holds(pa) == false);
  fail_unless(job_aborted == 1); // Still 1
  
  // Now abort it
  pa->ai_qs.num_successful = 20;
  pa->ai_qs.jobs_done = 20;
  fail_unless(set_array_depend_holds(pa) == false);
  fail_unless(job_aborted == 2);
  
  // Abort afterstart as well
  CLEAR_HEAD(pa->ai_qs.deps);
  fail_unless(register_array_depend(pa, preq, JOB_DEPEND_TYPE_AFTERSTARTARRAY, 1) == PBSE_NONE);
  pa->ai_qs.num_successful = 0;
  pa->ai_qs.num_failed = 10;
  pa->ai_qs.jobs_done = 10;
  pa->ai_qs.num_jobs = 10;
  fail_unless(set_array_depend_holds(pa) == false);
  fail_unless(job_aborted == 3); 
  }
END_TEST


START_TEST(check_dependency_job_test)
  {
  job           *pjob = NULL;
  batch_request  preq;
  char          *jobid = strdup("1.napali");

  memset(&preq, 0, sizeof(preq));

  fail_unless(check_dependency_job(NULL, NULL, NULL) == PBSE_BAD_PARAMETER, "passed bad params?");
  fail_unless(check_dependency_job(jobid, &preq, &pjob) == PBSE_IVALREQ, "accepted non-server request?");

  preq.rq_fromsvr = 1;
  fail_unless(check_dependency_job(jobid, &preq, &pjob) == PBSE_NONE, "error?");
  fail_unless(pjob != NULL, "didn't get a valid job?");

  fail_unless(check_dependency_job((char *)"bob", &preq, &pjob) == PBSE_UNKJOBID, "found unknown?");
  fail_unless(pjob == NULL, "didn't set job to NULL");

  svr = 1;
  fail_unless(check_dependency_job((char *)"bob", &preq, &pjob) == PBSE_JOBNOTFOUND, "wrong rc");
  fail_unless(pjob == NULL, "didn't set job to NULL");
  svr = 2;
  
  fail_unless(check_dependency_job((char *)"2.napali", &preq, &pjob) == PBSE_BADSTATE, "wrong rc");
  }
END_TEST




START_TEST(find_depend_test)
  {
  pbs_attribute  pattr;
  struct depend *pdep;

  initialize_depend_attr(&pattr);
  pdep = make_depend(1, &pattr);
  pdep = make_depend(2, &pattr);
  pdep = make_depend(3, &pattr);

  pdep = find_depend(1, &pattr);
  fail_unless(pdep != NULL, "couldn't find 1");
  fail_unless(pdep->dp_type == 1, "bad type returned");

  pdep = find_depend(2, &pattr);
  fail_unless(pdep != NULL, "couldn't find 2");
  fail_unless(pdep->dp_type == 2, "bad type returned");

  pdep = find_depend(3, &pattr);
  fail_unless(pdep != NULL, "couldn't find 3");
  fail_unless(pdep->dp_type == 3, "bad type returned");

  pdep = find_depend(0, &pattr);
  fail_unless(pdep == NULL, "found non-existent thing");
  }
END_TEST



START_TEST(clear_depend_test)
  {
  struct depend pd;

  i = 1;
  clear_depend(&pd, 0, 0);

  fail_unless(pd.dp_type == 0, "type not set");
  fail_unless(pd.dp_numexp == 0, "attr not set correctly");
  fail_unless(pd.dp_numreg == 0, "attr not set correctly");
  fail_unless(pd.dp_released == 0, "attr not set correctly");
  
  i = 1;
  depend pd2;
  clear_depend(&pd2, 0, 0);

  make_dependjob(&pd2, job1);
  make_dependjob(&pd2, job2);
  clear_depend(&pd2, 0, 1);
  fail_unless(pd2.dp_type == 0, "type not set");
  fail_unless(pd2.dp_numexp == 0, "attr not set correctly");
  fail_unless(pd2.dp_numreg == 0, "attr not set correctly");
  fail_unless(pd2.dp_released == 0, "attr not set correctly");
  }
END_TEST




START_TEST(make_depend_test)
  {
  pbs_attribute  pattr;
  struct depend *pdep;

  memset(&pattr, 0, sizeof(pattr));
  CLEAR_HEAD(pattr.at_val.at_list);

  pdep = make_depend(1, &pattr);
  fail_unless((pattr.at_flags & ATR_VFLAG_SET) != 0, "didn't set attribute");
  fail_unless(pdep->dp_type == 1, "type not set");
  
  pdep = make_depend(2, &pattr);
  fail_unless(pdep->dp_type == 2, "type not set");
  }
END_TEST




START_TEST(find_dependjob_test)
  {
  struct depend      pdep;
  struct depend_job *d1;
  struct depend_job *d2;

  memset(&pdep, 0, sizeof(pdep));

  fail_unless((d1 = make_dependjob(&pdep, job1)) != NULL, "didn't create dep 1");
  fail_unless((d2 = make_dependjob(&pdep, job2)) != NULL, "didn't create dep 2");
  fail_unless(d1 == find_dependjob(&pdep, job1), "didn't find job1");
  fail_unless(d2 == find_dependjob(&pdep, job2), "didn't find job2");
  fail_unless(find_dependjob(&pdep, (char *)"bob") == NULL, "found bob?");

  svr = 10; // will make display server suffix false, see scaffolding 
  fail_unless(d1 == find_dependjob(&pdep, (char *)"1"), "didn't find job1 without suffix");
  fail_unless(d2 == find_dependjob(&pdep, (char *)"2"), "didn't find job2 without suffix");
  }
END_TEST




START_TEST(make_dependjob_test)
  {
  struct depend      pdep;
  struct depend_job *d1;
  struct depend_job *d2;

  memset(&pdep, 0, sizeof(pdep));

  fail_unless((d1 = make_dependjob(&pdep, job1)) != NULL, "didn't create dep 1");
  fail_unless((d2 = make_dependjob(&pdep, job2)) != NULL, "didn't create dep 2");
  fail_unless(d1->dc_cost == 0, "bad cost");
  fail_unless(d1->dc_state == 0, "bad state");
  fail_unless(d1->dc_child == job1, "bad job id1");
  fail_unless(d2->dc_child == job2, "bad job id2");
  }
END_TEST




START_TEST(register_sync_test)
  {
  struct depend  pdep;

  memset(&pdep, 0, sizeof(pdep));

  fail_unless(register_sync(&pdep, job1, host, 1) == PBSE_NONE, "didn't register");
  fail_unless(register_sync(&pdep, job1, host, 1) == PBSE_NONE, "second register");
  pdep.dp_numexp = -10;
  fail_unless(register_sync(&pdep, job2, host, 1) == PBSE_IVALREQ, "too many registered");

  }
END_TEST




START_TEST(register_dep_test)
  {
  pbs_attribute  pattr;
  batch_request  preq;
  int            made = 0;

  initialize_depend_attr(&pattr);
  memset(&preq, 0, sizeof(preq));
  strcpy(preq.rq_ind.rq_register.rq_svr, host);
  strcpy(preq.rq_ind.rq_register.rq_child, job1);

  strcpy(server_name, host);

  fail_unless(register_dep(&pattr, &preq, 1, &made) == PBSE_NONE, "didn't register");
  fail_unless(register_dep(&pattr, &preq, 1, &made) == PBSE_NONE, "didn't register");
  }
END_TEST




START_TEST(unregister_dep_test)
  {
  pbs_attribute  pattr;
  struct depend *pdep;
  batch_request  preq;
  
  memset(&preq, 0, sizeof(preq));
  strcpy(preq.rq_ind.rq_register.rq_svr, host);
  strcpy(preq.rq_ind.rq_register.rq_child, job1);
  preq.rq_ind.rq_register.rq_dependtype = 1;

  initialize_depend_attr(&pattr);

  fail_unless(unregister_dep(&pattr, &preq) == PBSE_IVALREQ, "didn't error on non-existent dep");

  pdep = make_depend(5, &pattr);
  make_dependjob(pdep, job1);

  fail_unless(unregister_dep(&pattr, &preq) == PBSE_NONE, "didn't unregister");
  }
END_TEST



START_TEST(comp_depend_test)
  {
  fail_unless(comp_depend(NULL, NULL) == -1, "update the unit test");
  }
END_TEST




START_TEST(free_depend_test)
  {
  pbs_attribute  pattr;
  struct depend *pdep;
  
  initialize_depend_attr(&pattr);
  pdep = make_depend(5, &pattr);
  make_dependjob(pdep, job1);
  pdep = make_depend(6, &pattr);
  make_dependjob(pdep, job2);

  free_depend(&pattr);
  fail_unless((pattr.at_flags & ATR_VFLAG_SET) == 0, "didn't free?");
  }
END_TEST




START_TEST(build_depend_test)
  {
  pbs_attribute  pattr;
  
  initialize_depend_attr(&pattr);

  // value must be in the format dependency_type[:job_id][:job_id2...]
  fail_unless(build_depend(&pattr, "") == PBSE_BADATVAL);
  fail_unless(build_depend(&pattr, NULL) == PBSE_BADATVAL);

  // should accept empty dependencies for dependency removal
  fail_unless(build_depend(&pattr, "afterok") != PBSE_BADATVAL);

  fail_unless(build_depend(&pattr, strdup("afterok:1.napali")) == 0, "didn't build");
  fail_unless(build_depend(&pattr, strdup("bob:1.napali")) == PBSE_BADATVAL, "bad dependency didn't fail");
  fail_unless(build_depend(&pattr, strdup("syncwith:1.napali")) == PBSE_BADATVAL, "bad combo not rejected");
  fail_unless(build_depend(&pattr, strdup("on:1.napali")) == PBSE_BADATVAL, "bad combo 2 not rejected");
  
  strcpy(server_name, host);
  initialize_depend_attr(&pattr);
  fail_unless(build_depend(&pattr, strdup("on:6")) == PBSE_NONE, "on build");

  initialize_depend_attr(&pattr);
  fail_unless(build_depend(&pattr, strdup("beforeok:2.napali@bob")) == PBSE_NONE, "@server");
  fail_unless(build_depend(&pattr, strdup("afterokarray:10[].napali")) == PBSE_NONE, "array combo is now allowed");
  }
END_TEST




START_TEST(dup_depend_test)
  {
  struct depend      pdep;
  pbs_attribute      pattr;

  initialize_depend_attr(&pattr);
  memset(&pdep, 0, sizeof(pdep));

  make_dependjob(&pdep, job1);
  make_dependjob(&pdep, job2);
  fail_unless(dup_depend(&pattr, &pdep) == PBSE_NONE, "didn't work");
  }
END_TEST 




START_TEST(fast_strcat_test)
  {
  char  buf[100];
  char *b = buf;

  memset(buf, 0, sizeof(buf));
  fast_strcat(NULL, "a");
  fast_strcat(&b, "1");
  fail_unless(strcmp(buf, "1") == 0, "fail 1");
  fast_strcat(&b, "2");
  fail_unless(strcmp(buf, "12") == 0, "fail 2");
  fast_strcat(&b, "34");
  fail_unless(strcmp(buf, "1234") == 0, "fail 3");
  }
END_TEST




START_TEST(cat_jobsvr_test)
  {
  char  buf[100];
  char *b = buf;

  memset(buf, 0, sizeof(buf));
  cat_jobsvr(NULL, (char *)"a");
  cat_jobsvr(&b, (char *)"bobby:14003");
  fail_unless(strcmp(buf, "bobby\\:14003") == 0, "fail");
  }
END_TEST


START_TEST(req_register_test)
  {
  batch_request *preq = (batch_request *)calloc(1, sizeof(batch_request));

  strcpy(preq->rq_ind.rq_register.rq_parent, job1);
  preq->rq_fromsvr = 1;
  preq->rq_ind.rq_register.rq_dependtype = JOB_DEPEND_TYPE_AFTEROK;

  preq->rq_ind.rq_register.rq_op = -10;
  fail_unless(req_register(preq) == PBSE_IVALREQ);

  preq->rq_ind.rq_register.rq_op = JOB_DEPEND_OP_REGISTER;
  fail_unless(req_register(preq) == PBSE_NONE);
  
  preq->rq_ind.rq_register.rq_op = JOB_DEPEND_OP_RELEASE;
  fail_unless(req_register(preq) == PBSE_NONE);
  
  preq->rq_ind.rq_register.rq_op = JOB_DEPEND_OP_DELETE;
  fail_unless(req_register(preq) == PBSE_NONE);
  
  preq->rq_ind.rq_register.rq_op = JOB_DEPEND_OP_UNREG;
  fail_unless(req_register(preq) == PBSE_NONE);
  }
END_TEST


START_TEST(decode_depend_test)
  {
  pbs_attribute pattr;
  
  initialize_depend_attr(&pattr);

  fail_unless(decode_depend(&pattr, NULL, NULL, NULL, 0) == 0, "null failed");
  fail_unless(decode_depend(&pattr, NULL, NULL, "", 0) == 0, "empty failed");
  fail_unless(decode_depend(&pattr, NULL, NULL, strdup("afterok:1.napali,beforeany:2.napali"), 0) == 0, "empty failed");
  fail_unless(decode_depend(&pattr, NULL, NULL, strdup("bart"), 0) != PBSE_NONE, "bad worked?");
  }
END_TEST


START_TEST(encode_depend_test)
  {
  pbs_attribute  pattr;
  struct depend *pdep;
  tlist_head     phead;
  
  initialize_depend_attr(&pattr);
  CLEAR_HEAD(phead);
  
  fail_unless(encode_depend(NULL, NULL, NULL, NULL, 0, 0) == -1, "null");
  fail_unless(encode_depend(&pattr, NULL, NULL, NULL, 0, 0) == 0, "empty");

  pdep = make_depend(5, &pattr);
  make_dependjob(pdep, job1);
  pdep = make_depend(6, &pattr);
  make_dependjob(pdep, job2);

  fail_unless(encode_depend(&pattr, &phead, "depend", NULL, 0, 0) == 1, "encoding");
  }
END_TEST


START_TEST(set_depend_test)
  {
  pbs_attribute  pattr;
  pbs_attribute  pa2;
  struct depend *pdep;
  tlist_head     phead;
  
  initialize_depend_attr(&pattr);
  initialize_depend_attr(&pa2);
  CLEAR_HEAD(phead);

  pdep = make_depend(5, &pattr);
  make_dependjob(pdep, job1);
  pdep = make_depend(6, &pattr);
  make_dependjob(pdep, job2);

  fail_unless(set_depend(&pa2, &pattr, INCR) == PBSE_IVALREQ, "INCR worked?");
  fail_unless(set_depend(&pa2, &pattr, SET) == PBSE_NONE, "fail set?");
  }
END_TEST


/*
START_TEST(unregister_sync_test)
  {
  pbs_attribute  pattr;
  struct depend *pdep;
  batch_request  preq;
  
  initialize_depend_attr(&pattr);
  memset(&preq, 0, sizeof(preq));
  
  pdep = make_depend(JOB_DEPEND_TYPE_SYNCCT, &pattr);
  make_dependjob(pdep, job1);
  make_dependjob(pdep, job2);
  pdep->dp_released = 1;

  fail_unless(unregister_sync(&pattr, &preq) == PBSE_IVALREQ, "bad name worked?");
  strcpy(preq.rq_ind.rq_register.rq_child, job1);
  fail_unless(unregister_sync(&pattr, &preq) == PBSE_NONE, "success");
  }
END_TEST*/


START_TEST(depend_clrrdy_test)
  {
  pbs_attribute *pattr;
  job            pjob;
  struct depend *pdep;

  memset(&pjob, 0, sizeof(pjob));
  pattr = &pjob.ji_wattr[JOB_ATR_depend];
  initialize_depend_attr(pattr);
  pdep = make_depend(JOB_DEPEND_TYPE_SYNCCT, pattr);
  make_dependjob(pdep, job1);
  depend_clrrdy(&pjob);
  }
END_TEST


START_TEST(register_before_dep_test)
  {
  batch_request  preq;
  job            pjob;
  pbs_attribute *pattr;
  struct depend *pdep;
  int            rc = 0;
  char           buf[1000];
  
  memset(&preq, 0, sizeof(preq));
  memset(&pjob, 0, sizeof(pjob));

  strcpy(preq.rq_ind.rq_register.rq_owner, "dbeer");
  strcpy(preq.rq_ind.rq_register.rq_child, job1);
  strcpy(preq.rq_ind.rq_register.rq_svr, host);

  pattr = &pjob.ji_wattr[JOB_ATR_depend];
  initialize_depend_attr(pattr);
  pdep = make_depend(1, pattr);
  make_dependjob(pdep, job1);
  rc = register_dependency(&preq, &pjob, JOB_DEPEND_TYPE_BEFOREOK);
  snprintf(buf, sizeof(buf), "rc = %d", rc);
  fail_unless(rc == PBSE_NONE, "first, rc = %d", rc);
  
  pattr = &pjob.ji_wattr[JOB_ATR_depend];
  initialize_depend_attr(pattr);
  pdep = make_depend(JOB_DEPEND_TYPE_ON, pattr);
  make_dependjob(pdep, job1);
  fail_unless(register_before_dep(&preq, &pjob, 1) == PBSE_NONE, "second");
  }
END_TEST


START_TEST(register_dependency_test)
  {
  batch_request  preq;
  job            pjob;
  pbs_attribute *pattr;
  
  memset(&preq, 0, sizeof(preq));
  memset(&pjob, 0, sizeof(pjob));

  strcpy(preq.rq_ind.rq_register.rq_owner, "dbeer");
  strcpy(preq.rq_ind.rq_register.rq_child, job1);
  strcpy(preq.rq_ind.rq_register.rq_svr, host);
  strcpy(preq.rq_ind.rq_register.rq_parent, job1);

  fail_unless(register_dependency(&preq, &pjob, 1) == PBSE_IVALREQ);
  strcpy(preq.rq_ind.rq_register.rq_parent, job2);
  fail_unless(register_dependency(&preq, &pjob, -1) == PBSE_IVALREQ);

  pattr = &pjob.ji_wattr[JOB_ATR_depend];
  initialize_depend_attr(pattr);

  fail_unless(register_dependency(&preq, &pjob, JOB_DEPEND_TYPE_AFTERSTART) == PBSE_NONE);
  }
END_TEST


START_TEST(release_before_dependency_test)
  {
  batch_request  preq;
  job            pjob;
  pbs_attribute *pattr;
  struct depend *pdep;
  
  memset(&preq, 0, sizeof(preq));
  memset(&pjob, 0, sizeof(pjob));

  strcpy(preq.rq_ind.rq_register.rq_owner, "dbeer");
  strcpy(preq.rq_ind.rq_register.rq_child, job1);
  strcpy(preq.rq_ind.rq_register.rq_svr, host);

  pattr = &pjob.ji_wattr[JOB_ATR_depend];
  initialize_depend_attr(pattr);
  pdep = make_depend(1, pattr);
  make_dependjob(pdep, job1);
  register_dependency(&preq, &pjob, JOB_DEPEND_TYPE_BEFOREOK);

  fail_unless(release_before_dependency(&preq, &pjob, JOB_DEPEND_TYPE_BEFOREOK) == PBSE_NONE);
  fail_unless(release_before_dependency(&preq, &pjob, JOB_DEPEND_TYPE_BEFOREOK) == PBSE_IVALREQ);
  }
END_TEST


START_TEST(release_syncwith_dependency_test)
  {
  pbs_attribute *pattr;
  job            pjob;
  struct depend *pdep;
  batch_request  preq;

  memset(&preq, 0, sizeof(preq));
  memset(&pjob, 0, sizeof(pjob));
  pattr = &pjob.ji_wattr[JOB_ATR_depend];
  initialize_depend_attr(pattr);

  fail_unless(release_syncwith_dependency(&preq, &pjob) == PBSE_NOSYNCMSTR);

  pdep = make_depend(JOB_DEPEND_TYPE_SYNCWITH, pattr);
  make_dependjob(pdep, job1);
  pdep->dp_released = 0;
  
  fail_unless(release_syncwith_dependency(&preq, &pjob) == PBSE_NONE);
  }
END_TEST


START_TEST(set_depend_hold_test)
  {
  pbs_attribute *pattr;
  job            pjob;
  job            pjob2;
  struct depend *pdep;
 
  memset(&pjob, 0, sizeof(pjob));
  pattr = &pjob.ji_wattr[JOB_ATR_depend];
  initialize_depend_attr(pattr);
  pdep = make_depend(JOB_DEPEND_TYPE_AFTERNOTOK, pattr);
  make_dependjob(pdep, job2);

  set_depend_hold(&pjob, pattr, NULL);
  fail_unless((pjob.ji_wattr[JOB_ATR_hold].at_flags & ATR_VFLAG_SET) != 0);
  fail_unless(pjob.ji_qs.ji_state == JOB_STATE_HELD);
  
  memset(&pjob2, 0, sizeof(pjob2));
  pjob2.ji_qs.ji_substate = JOB_SUBSTATE_DEPNHOLD;
  strcpy(pjob2.ji_qs.ji_jobid, job2);
  pattr = &pjob2.ji_wattr[JOB_ATR_depend];
  initialize_depend_attr(pattr);
  pdep = make_depend(JOB_DEPEND_TYPE_AFTEROK, pattr);
  make_dependjob(pdep, job2);

  int saved_err = 0;
  try
    {
    set_depend_hold(&pjob2, pattr, NULL);
    }
  catch (int err)
    {
    saved_err = err;
    }

  fail_unless(saved_err == PBSE_BADDEPEND);

  memset(&pjob, 0, sizeof(pjob));
  pattr = &pjob.ji_wattr[JOB_ATR_depend];
  initialize_depend_attr(pattr);
  pdep = make_depend(JOB_DEPEND_TYPE_AFTERSTARTARRAY, pattr);
  // intentially skip make_depend() call so
  // a dependency is not actually created.
  // set_depend_hold() expected to gracefully
  // handle this condition.
  set_depend_hold(&pjob, pattr, NULL);
  fail_unless((pjob.ji_wattr[JOB_ATR_hold].at_flags & ATR_VFLAG_SET) == 0);
  fail_unless(pjob.ji_qs.ji_state != JOB_STATE_HELD);
  }
END_TEST


START_TEST(delete_dependency_job_test)
  {
  job           *pjob = job_alloc();
  batch_request  preq;

  memset(&preq, 0, sizeof(preq));
  strcpy(preq.rq_ind.rq_register.rq_parent, job1);
  strcpy(preq.rq_ind.rq_register.rq_child, job1);

  fail_unless(delete_dependency_job(&preq, &pjob) == PBSE_IVALREQ);
  strcpy(preq.rq_ind.rq_register.rq_child, job2);
  pjob->ji_qs.ji_state = JOB_STATE_RUNNING;
  fail_unless(delete_dependency_job(&preq, &pjob) == PBSE_NONE);
  fail_unless(pjob != NULL);
  pjob->ji_qs.ji_state = JOB_STATE_QUEUED;
  fail_unless(delete_dependency_job(&preq, &pjob) == PBSE_NONE);
  fail_unless(pjob == NULL);
  }
END_TEST


extern job *pGlobalJob;

START_TEST(remove_after_any_test)
  {
  job *pJob = job_alloc();
  job *pTJob = job_alloc();
  job *pOJob = job_alloc();
  struct depend_job *pj;

  strcpy(pJob->ji_qs.ji_jobid,"job 1");
  strcpy(pTJob->ji_qs.ji_jobid,"job 2");
  strcpy(pOJob->ji_qs.ji_jobid,"job 3");

  pbs_attribute *pattr = &pJob->ji_wattr[JOB_ATR_depend];
  initialize_depend_attr(pattr);
  struct depend *pdep = make_depend(JOB_DEPEND_TYPE_AFTERANY, pattr);
  make_dependjob(pdep, pTJob->ji_qs.ji_jobid);

  make_dependjob(pdep, pOJob->ji_qs.ji_jobid);

  pattr = &pJob->ji_wattr[JOB_ATR_depend];
  pdep = find_depend(JOB_DEPEND_TYPE_AFTERANY,pattr);
  pj = find_dependjob(pdep,pTJob->ji_qs.ji_jobid);
  fail_unless((pj != NULL),"Dependency not found.");
  fail_unless(pj->dc_child == pTJob->ji_qs.ji_jobid, "Wrong job found.");
  pj = find_dependjob(pdep,pOJob->ji_qs.ji_jobid);
  fail_unless((pj != NULL),"Wrong dependency deleted.");
  fail_unless(pj->dc_child == pOJob->ji_qs.ji_jobid,"Wrong job found.");

  pGlobalJob = pJob;

  removeAfterAnyDependency(pJob->ji_qs.ji_jobid,pTJob->ji_qs.ji_jobid);

  pGlobalJob = NULL;

  pattr = &pJob->ji_wattr[JOB_ATR_depend];
  pdep = find_depend(JOB_DEPEND_TYPE_AFTERANY,pattr);
  pj = find_dependjob(pdep,pTJob->ji_qs.ji_jobid);
  fail_unless((pj == NULL),"Dependency not deleted.");
  pj = find_dependjob(pdep,pOJob->ji_qs.ji_jobid);
  fail_unless((pj != NULL),"Wrong dependency deleted.");
  fail_unless(pj->dc_child == pOJob->ji_qs.ji_jobid, "Wrong job found.");

  pGlobalJob = pJob;

  removeAfterAnyDependency(pJob->ji_qs.ji_jobid,pOJob->ji_qs.ji_jobid);

  pGlobalJob = NULL;

  pattr = &pJob->ji_wattr[JOB_ATR_depend];
  pdep = find_depend(JOB_DEPEND_TYPE_AFTERANY,pattr);
  fail_unless(pdep == NULL, "Dependency not deleted");
  }
END_TEST




Suite *req_register_suite(void)
  {
  Suite *s = suite_create("req_register_suite methods");
  TCase *tc_core = tcase_create("check_dependency_job_test");
  tcase_add_test(tc_core, check_dependency_job_test);
  tcase_add_test(tc_core, find_depend_test);
  tcase_add_test(tc_core, clear_depend_test);
  tcase_add_test(tc_core, make_depend_test);
  tcase_add_test(tc_core, register_sync_test);
  tcase_add_test(tc_core, find_dependjob_test);
  tcase_add_test(tc_core, make_dependjob_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("register_dep_test");
  tcase_add_test(tc_core, register_dep_test);
  tcase_add_test(tc_core, unregister_dep_test);
  tcase_add_test(tc_core, comp_depend_test);
  tcase_add_test(tc_core, free_depend_test);
  tcase_add_test(tc_core, build_depend_test);
  tcase_add_test(tc_core, dup_depend_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("cat_jobsvr_test");
  tcase_add_test(tc_core, cat_jobsvr_test);
  tcase_add_test(tc_core, fast_strcat_test);
  tcase_add_test(tc_core, test_depend_on_que);

  tc_core = tcase_create("set_depend_test");
  tcase_add_test(tc_core, set_depend_test);
  tcase_add_test(tc_core, encode_depend_test);
  tcase_add_test(tc_core, decode_depend_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("depend_clrrdy_test");
  tcase_add_test(tc_core, depend_clrrdy_test);
  tcase_add_test(tc_core, register_before_dep_test);
  tcase_add_test(tc_core, register_dependency_test);
  tcase_add_test(tc_core, release_before_dependency_test);
  tcase_add_test(tc_core, release_syncwith_dependency_test);
  tcase_add_test(tc_core, set_depend_hold_test);
  tcase_add_test(tc_core, delete_dependency_job_test);
  tcase_add_test(tc_core, remove_after_any_test);
  tcase_add_test(tc_core, req_register_test);
  tcase_add_test(tc_core, set_array_depend_holds_test);
  tcase_add_test(tc_core, remove_array_dependency_from_job_test);
  tcase_add_test(tc_core, test_job_ids_match);
  suite_add_tcase(s, tc_core);

/*  tc_core = tcase_create("unregister_sync_test");
  tcase_add_test(tc_core, unregister_sync_test);
  suite_add_tcase(s, tc_core);
*/

  return s;
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(req_register_suite());
  srunner_set_log(sr, "req_register_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
