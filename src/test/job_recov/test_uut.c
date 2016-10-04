#include "license_pbs.h" /* See here for the software license */
#include "pbs_config.h"
#include "job_recov.h"
#include "test_job_recov.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <semaphore.h>
#include "pbs_error.h"
#include "pbs_job.h"
#include "attribute.h"
#include "resource.h"
#include "completed_jobs_map.h"
#include "server.h"
#include "array.h"

sem_t *job_clone_semaphore;
extern int set_nodes_attr(job *pjob);
extern int svr_resc_size;
extern attribute_def job_attr_def[];
extern void free_server_attrs(tlist_head *att_head);
extern completed_jobs_map_class completed_jobs_map;
int fill_resource_list(job **pj, xmlNodePtr resource_list_node, char *log_buf, size_t buflen, const char *aname);
void init_resc_defs();

char  server_name[] = "lei.ac";

int add_encoded_attributes(xmlNodePtr *attr_node, pbs_attribute *pattr);
void translate_dependency_to_string(pbs_attribute *pattr, std::string &value);
int  set_array_job_ids(job **pjob, char *log_buf, size_t buflen);
svrattrl *fill_svrattr_info(const char *aname, const char *avalue, const char *rname, char *log_buf, size_t      buf_len);
void decode_attribute(svrattrl *pal, job **pjob, bool freeExisting);
job_array *ghost_create_jobs_array(job *pjob, const char *array_id);
void check_and_reallocate_job_ids(job_array *pa, int index);
void update_recovered_array_values(job_array *pa, job *pjob);

void clear_attr(pbs_attribute *pattr, attribute_def *def);

void init()
  {
  server.sv_attr_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  }


START_TEST(ghost_array_test)
  {
  const char *array_id = "10[].napali";
  job *pjob = (job *)calloc(1, sizeof(job));
  snprintf(pjob->ji_qs.ji_jobid, sizeof(pjob->ji_qs.ji_jobid), "%s", "10[0].napali");

  init();
  
  for (int i = 0;i < JOB_ATR_LAST;i++)
    {
    clear_attr(&pjob->ji_wattr[i], &job_attr_def[i]);
    }

  pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str = strdup("dbeer@napali");
  pjob->ji_wattr[JOB_ATR_job_owner].at_flags = ATR_VFLAG_SET;

  job_array *pa = ghost_create_jobs_array(pjob, array_id);
  fail_unless(!strcmp(pa->ai_qs.parent_id, array_id));
  fail_unless(!strcmp(pa->ai_qs.fileprefix, "10.napali"), "prefix=%s", pa->ai_qs.fileprefix);
  fail_unless(pa->job_ids[0] != NULL);
  fail_unless(!strcmp(pa->job_ids[0], pjob->ji_qs.ji_jobid));
  fail_unless(pa->ai_qs.array_size == 101); // DEFAULT_ARRAY_RECOV_SIZE

  pa->job_ids[50] = strdup("10[50].napali");

  check_and_reallocate_job_ids(pa, 210);
  fail_unless(!strcmp(pa->job_ids[0], "10[0].napali"));
  fail_unless(!strcmp(pa->job_ids[50], "10[50].napali"));
  fail_unless(pa->ai_qs.array_size == 404); // should have doubled twice to accomodate index 210

  pjob->ji_qs.ji_state = JOB_STATE_RUNNING;
  update_recovered_array_values(pa, pjob);
  fail_unless(pa->ai_qs.num_jobs == 1);
  fail_unless(pa->ai_qs.num_started == 1);
  fail_unless(pa->ai_qs.jobs_running == 1);
      
  pjob->ji_qs.ji_state = JOB_STATE_COMPLETE;
  update_recovered_array_values(pa, pjob);
  fail_unless(pa->ai_qs.num_jobs == 2, "%d jobs", pa->ai_qs.num_jobs);
  fail_unless(pa->ai_qs.num_started == 2);
  fail_unless(pa->ai_qs.jobs_done == 1);
  fail_unless(pa->ai_qs.jobs_running == 1);
  
  pjob->ji_qs.ji_state = JOB_STATE_QUEUED;
  update_recovered_array_values(pa, pjob);
  fail_unless(pa->ai_qs.num_jobs == 3);
  fail_unless(pa->ai_qs.num_started == 2);
  fail_unless(pa->ai_qs.jobs_done == 1);
  fail_unless(pa->ai_qs.jobs_running == 1);
  }
END_TEST


START_TEST(test_decode_attribute)
  {
  char      buf[1024];
  job      *pjob = new job();
  svrattrl *pal = fill_svrattr_info("Hold_Types", "1", "", buf, sizeof(buf));
  pal->al_flags = ATR_VFLAG_SET;

  decode_attribute(pal, &pjob, false);
  fail_unless(pjob->ji_wattr[JOB_ATR_hold].at_val.at_long == 1, "val: %d", (int)pjob->ji_wattr[JOB_ATR_hold].at_val.at_long);
  fail_unless((pjob->ji_wattr[JOB_ATR_hold].at_flags & ATR_VFLAG_SET) != 0);
  }
END_TEST

START_TEST(ghost_array_test2)
  {
  const char *array_id = "10[].napali";
  job *pjob = (job *)calloc(1, sizeof(job));
  snprintf(pjob->ji_qs.ji_jobid, sizeof(pjob->ji_qs.ji_jobid), "%s", "10[0].napali");

  init();
  
  for (int i = 0;i < JOB_ATR_LAST;i++)
    {
    clear_attr(&pjob->ji_wattr[i], &job_attr_def[i]);
    }

  pjob->ji_wattr[JOB_ATR_job_owner].at_val.at_str = strdup("dbeer@napali");
  pjob->ji_wattr[JOB_ATR_job_owner].at_flags = ATR_VFLAG_SET;
  pjob->ji_wattr[JOB_ATR_job_array_id].at_val.at_long = 500; // >DEFAULT_ARRAY_RECOV_SIZE

  job_array *pa = ghost_create_jobs_array(pjob, array_id);
  fail_unless(pa->ai_ghost_recovered == true);
  fail_unless(pa->ai_qs.array_size == 501); // 1 more than 500
  }
END_TEST

START_TEST(test_set_array_jobs_ids)
  {
  job  *pjob = new job();
  char  buf[1024];
  std::string expected_err_msg;

  init();

  sprintf(pjob->ji_qs.ji_jobid, "4[21].napali");
  fail_unless(set_array_job_ids(&pjob, buf, sizeof(buf)) == PBSE_NONE);

  pjob->ji_wattr[JOB_ATR_job_array_id].at_flags |= ATR_VFLAG_SET;
  pjob->ji_wattr[JOB_ATR_job_array_id].at_val.at_long = 21;
  fail_unless(set_array_job_ids(&pjob, buf, sizeof(buf)) != PBSE_NONE);

  // check the returned error message
  expected_err_msg = "array struct missing for array job ";
  expected_err_msg.append(pjob->ji_qs.ji_jobid);
  fail_unless(strcmp(buf, expected_err_msg.c_str()) == 0);

  pjob = new job();
  sprintf(pjob->ji_qs.ji_jobid, "4[].napali");
  fail_unless(set_array_job_ids(&pjob, buf, sizeof(buf)) == -1); // We should delete template jobs that don't have an array
  fail_unless(pjob->ji_is_array_template == TRUE);
  }
END_TEST


START_TEST(test_translate_dependency_to_string)
  {
  pbs_attribute dep_attr;
  depend dep;
  depend_job *dj = new depend_job();

  dj->dc_child = "1.napali";
  
  dep.dp_type = JOB_DEPEND_TYPE_AFTEROK;
  CLEAR_LINK(dep.dp_link);
  dep.dp_jobs.push_back(dj);

  dep_attr.at_flags = ATR_VFLAG_SET;
  CLEAR_HEAD(dep_attr.at_val.at_list);
  append_link(&dep_attr.at_val.at_list, &dep.dp_link, &dep);

  std::string value;

  // make sure we don't segfault
  translate_dependency_to_string(NULL, value);
  translate_dependency_to_string(&dep_attr, value);
  fail_unless(value == "afterok:1.napali");
  }
END_TEST


START_TEST(test_add_encoded_attributes)
  {
  xmlNodePtr attr_node = xmlNewNode(NULL, (xmlChar *)ATTRIB_TAG);
  pbs_attribute attributes[JOB_ATR_LAST];

  memset(&attributes, 0, sizeof(attributes));
  attributes[JOB_ATR_job_owner].at_flags |= ATR_VFLAG_SET;
  attributes[JOB_ATR_job_owner].at_val.at_str = strdup("dbeer@napali");

  add_encoded_attributes(&attr_node, attributes);
  xmlNode *child = attr_node->children;

  fail_unless(child != NULL);
  fail_unless(!strcmp((const char *)child->name, ATTR_owner));
  fail_unless(!strcmp((const char *)child->children->name, "text"));
  fail_unless(!strcmp((const char *)child->children->content, "dbeer@napali"));
  
/*  save for later
  attributes[JOB_ATR_resc_used].at_flags |= ATR_VFLAG_SET;
  CLEAR_HEAD(attributes[JOB_ATR_resc_used].at_val.at_list);
  resource walltime;
  resource_def prd;
  memset(&walltime, 0, sizeof(walltime));
  CLEAR_LINK(walltime.rs_link);
  append_link(&attributes[JOB_ATR_resc_used].at_val.at_list, &walltime.rs_link, &walltime);
  walltime.rs_defin = &prd;
  walltime.rs_value.at_type = ATR_TYPE_LONG;
  walltime.rs_value.at_flags = 0;
  walltime.rs_value.at_val.at_long = 600;
  prd.rs_name = "walltime";
  attributes[JOB_ATR_job_owner].at_flags = 0;
  xmlNodePtr attr_node = xmlNewNode(NULL, (xmlChar *)ATTRIB_TAG);
  add_encoded_attributes(&attr_node, attributes);
  xmlNode *child = attr_node->children;

  fail_unless(child != NULL);
  fail_unless(!strcmp((const char *)child->name, ATTR_used)); */
  }
END_TEST

void add_attr_to_list(tlist_head *atrlist, const char *attrname, size_t len, const char *value)
  {
  svrattrl *pal = (svrattrl *)attrlist_create((char *)attrname, 0, len);
  fail_unless(pal != 0, "failed to allocate svrattrl for %s", value);
  strcpy((char *)pal->al_value, value);
  pal->al_flags = SET;
  append_link(atrlist, &pal->al_link, pal);
  }

void create_minimum_job_attributes(tlist_head& atrlist)
  {
  CLEAR_HEAD(atrlist);

  add_attr_to_list(&atrlist, ATTR_c, 2, "u");
  add_attr_to_list(&atrlist, ATTR_h, 2, "n");
  add_attr_to_list(&atrlist, ATTR_j, 2, "n");
  add_attr_to_list(&atrlist, ATTR_k, 2, "n");
  add_attr_to_list(&atrlist, ATTR_m, 2, "a");
  add_attr_to_list(&atrlist, ATTR_p, 2, "0");
  add_attr_to_list(&atrlist, ATTR_r, 5, "TRUE");
  add_attr_to_list(&atrlist, ATTR_f, 6, "FALSE");
  add_attr_to_list(&atrlist, ATTR_job_radix, 2, "0");
  add_attr_to_list(&atrlist, ATTR_submit_args, 1, "");
  add_attr_to_list(&atrlist, ATTR_N, 6, "STDIN");
  add_attr_to_list(&atrlist, ATTR_submit_host, 7, "lei.ac");
  add_attr_to_list(&atrlist, ATTR_v, 273, "PBS_O_HOME=/home/echan,\
PBS_O_LOGNAME=echan,PBS_O_PATH=/usr/lib/lightdm/lightdm:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games,\
PBS_O_SHELL=/bin/bash,PBS_O_LANG=en_US,PBS_O_WORKDIR=/home/echan/work/dev/torque/trunk,PBS_O_HOST=lei.ac,PBS_O_SERVER=lei"); 
  }

void fill_other_job_info(job **pjob, int resc_access_perm)
  {
  job *pj = *pjob;
  char buf[256];

  snprintf(buf, sizeof(buf), "%s", "echan@lei");

  job_attr_def[JOB_ATR_job_owner].at_decode(
    &pj->ji_wattr[JOB_ATR_job_owner],
    NULL,
    NULL,
    buf,
    resc_access_perm);

  time_t time_now = time(NULL);

  pj->ji_wattr[JOB_ATR_ctime].at_val.at_long = (long)time_now;
  pj->ji_wattr[JOB_ATR_ctime].at_flags |= ATR_VFLAG_SET;

  /* set hop count = 1 */
  pj->ji_wattr[JOB_ATR_hopcount].at_val.at_long = 1;
  pj->ji_wattr[JOB_ATR_hopcount].at_flags |= ATR_VFLAG_SET;

  pj->ji_wattr[JOB_ATR_outpath].at_val.at_str = (char *) "/someOutput/STDIN.o945";
  pj->ji_wattr[JOB_ATR_outpath].at_flags |= ATR_VFLAG_SET;

  pj->ji_wattr[JOB_ATR_errpath].at_val.at_str = (char *) "/someOutput/STDIN.e945";
  pj->ji_wattr[JOB_ATR_errpath].at_flags |= ATR_VFLAG_SET;

  job_attr_def[JOB_ATR_at_server].at_decode(
    &pj->ji_wattr[JOB_ATR_at_server],
    NULL,
    NULL,
    server_name,
    resc_access_perm);
   
  strcpy(pj->ji_qs.ji_queue, "batch");
  pj->ji_wattr[JOB_ATR_substate].at_val.at_long = JOB_SUBSTATE_TRANSIN;
  pj->ji_wattr[JOB_ATR_substate].at_flags |= ATR_VFLAG_SET;

  /* set remaining job structure elements */
  pj->ji_qs.ji_state =    JOB_STATE_TRANSIT;
  pj->ji_qs.ji_substate = JOB_SUBSTATE_TRANSIN;
  pj->ji_wattr[JOB_ATR_mtime].at_val.at_long = (long)time_now;
  pj->ji_wattr[JOB_ATR_mtime].at_flags |= ATR_VFLAG_SET;
  pj->ji_qs.ji_un_type = JOB_UNION_TYPE_NEW;
  pj->ji_qs.ji_un.ji_newt.ji_fromsock = 9;

  pj->ji_qs.ji_un.ji_newt.ji_fromaddr = 0;
  pj->ji_qs.ji_un.ji_newt.ji_scriptsz = 0;
  } /* END fill_other_job_info*/

job *create_a_job(const char *jobId)
  {
  job *pj = job_alloc();
  if (pj == NULL) 
    return NULL;

  tlist_head att_head;
  create_minimum_job_attributes(att_head);

  strcpy(pj->ji_qs.ji_jobid, jobId);
  strcpy(pj->ji_qs.ji_fileprefix, jobId);
  pj->ji_modified = 1;
  pj->ji_qs.ji_svrflags = JOB_SVFLG_HERE;
  pj->ji_qs.ji_un_type = JOB_UNION_TYPE_NEW;
  pj->ji_wattr[JOB_ATR_mailpnts].at_val.at_str = 0;

  int resc_access_perm = ATR_DFLAG_WRACC | ATR_DFLAG_MGWR | ATR_DFLAG_RMOMIG;
  int rc;

  svrattrl *psatl = (svrattrl *)GET_NEXT(att_head);
  while (psatl != NULL)
    {
    int attr_index = find_attr(job_attr_def, psatl->al_name, JOB_ATR_LAST);
    attribute_def *pdef = &job_attr_def[attr_index];
    rc = pdef->at_decode(
           &pj->ji_wattr[attr_index],
           psatl->al_name,
           psatl->al_resc,
           psatl->al_value,
           resc_access_perm);
    fail_unless(rc == PBSE_NONE, "Failed call to decode_resc");
    psatl = (svrattrl *)GET_NEXT(psatl->al_link);
    }

/*  rc = set_nodes_attr(pj); */
  fail_unless(rc == PBSE_NONE, "Failed to set nodes");

  fill_other_job_info(&pj, resc_access_perm);

  free_server_attrs(&att_head);

  return pj;
  }

int job_compare_fix_fields(job *pjob1, job* pjob2)
  {
  if (pjob1->ji_qs.qs_version != pjob2->ji_qs.qs_version)
    return -1;
  if (pjob1->ji_qs.ji_state != pjob2->ji_qs.ji_state)
    return -1;
  if (pjob1->ji_qs.ji_substate != pjob2->ji_qs.ji_substate) 
    return -1;
  if (pjob1->ji_qs.ji_svrflags != pjob2->ji_qs.ji_svrflags)
    return -1;
  if (pjob1->ji_qs.ji_stime != pjob2->ji_qs.ji_stime)
    return -1;
  if (strcmp(pjob1->ji_qs.ji_jobid, pjob2->ji_qs.ji_jobid))
    return -1;
  if (strcmp(pjob1->ji_qs.ji_fileprefix, pjob2->ji_qs.ji_fileprefix))
    return -1;
  if (strcmp(pjob1->ji_qs.ji_queue, pjob2->ji_qs.ji_queue))
    return -1;
  if (strcmp(pjob1->ji_qs.ji_destin, pjob2->ji_qs.ji_destin))
    return -1;

  return 0;
  }

int job_compare_union_fields(job *pjob1, job *pjob2)
  {
  if (pjob1->ji_qs.ji_un.ji_newt.ji_fromaddr != pjob2->ji_qs.ji_un.ji_newt.ji_fromaddr)
    return -1;
  if (pjob1->ji_qs.ji_un.ji_newt.ji_fromsock != pjob2->ji_qs.ji_un.ji_newt.ji_fromsock)
    return -1;
  if (pjob1->ji_qs.ji_un.ji_newt.ji_scriptsz != pjob2->ji_qs.ji_un.ji_newt.ji_scriptsz)
    return -1;

  return 0;
  }

int job_compare_attributes(job *pjo1, job *pj2)
  {
  return 0;
  }

int job_compare(job *pjob1, job *pjob2)
  {
  if (job_compare_fix_fields(pjob1, pjob2))
    return -1;
  if (job_compare_union_fields(pjob1, pjob2))
    return -1;
  if (job_compare_attributes(pjob1, pjob2))
    return -1;

  return 0;
  }

START_TEST(fill_resource_list_test)
  {
  const char *rl_sample = "<Resource_List>\n  <neednodes flags=\"1\">2</neednodes>\n  <nodect flags=\"1\">2</nodect>\n  <nodes flags=\"1\">2</nodes>\n  </Resource_List>";
  xmlDocPtr   doc = xmlReadMemory(rl_sample, strlen(rl_sample), "Resource List", NULL, 0);
  job        *pjob = new job();
  char        buf[1024];
  svr_resc_def = svr_resc_def_const;
  init();
  init_resc_defs();
  
  for (int i = 0;i < JOB_ATR_LAST;i++)
    {
    clear_attr(&pjob->ji_wattr[i], &job_attr_def[i]);
    }

  fail_unless(fill_resource_list(&pjob, xmlDocGetRootElement(doc), buf, sizeof(buf), ATTR_l) == 0);
  
  const char *rl_empty_sample = "<Resource_List>\n</Resource_List>";
  doc = xmlReadMemory(rl_empty_sample, strlen(rl_empty_sample), "Resource_List", NULL, 0);
  fail_unless(fill_resource_list(&pjob, xmlDocGetRootElement(doc), buf, sizeof(buf), ATTR_l) == -1);
  }
END_TEST

START_TEST(test_job_recover)
  {
  /* call initXML */
  char jobFileName[MAXPATHLEN];
  const char *jobid = "unit_test_job1"; 

  job *pj;
  pj = create_a_job(jobid);
  fail_unless((pj != NULL), "unable to create a job");
  snprintf(jobFileName, MAXPATHLEN, "/tmp/%s.JB", jobid);
  int rc = saveJobToXML(pj, jobFileName);
  fail_unless(rc == PBSE_NONE, "Failed to save job to an xml file");

  job *recov_pj = job_recov(jobFileName);
  fail_unless(recov_pj != NULL);

  rc = job_compare(pj, recov_pj);
  fail_unless(rc == 0, "jobs (saved & recovered) did not compare the same");

  unlink(jobFileName);
  }
END_TEST

Suite *job_recov_suite(void)
  {
  Suite *s = suite_create("job_recov_suite methods");

  TCase *tc_core = tcase_create("test_job_recover");
  tcase_add_test(tc_core, ghost_array_test);
  tcase_add_test(tc_core, ghost_array_test2);
  tcase_add_test(tc_core, test_job_recover);
  tcase_add_test(tc_core, fill_resource_list_test);
  tcase_add_test(tc_core, test_add_encoded_attributes);
  tcase_add_test(tc_core, test_translate_dependency_to_string);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_moar");
  tcase_add_test(tc_core, test_set_array_jobs_ids);
  tcase_add_test(tc_core, test_decode_attribute);
  suite_add_tcase(s, tc_core);

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
  sr = srunner_create(job_recov_suite());
  srunner_set_log(sr, "job_recov_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
