#include "alps_constants.h"
#include "track_alps_reservations.h"
#include <check.h>


char *eh1 = (char *)"napali+l11+tom";
char *eh2 = (char *)"napali/0+napali/1+l11/0+l11/1";
int   internal_job_ids[] = { 1, 2, 3, 4 };
char *jobids[] = { (char *)"1.napali", (char *)"2.napali", (char *)"3.napali", (char *)"4.napali" };
char *rsvids[] = {(char *)"1234", (char *)"1235", (char *)"1236", (char *)"1237" };

reservation_holder alps_reservations;

int  add_node_names(alps_reservation *, job *);
void free_alps_reservation(alps_reservation *ar);
alps_reservation *populate_alps_reservation(job *);



START_TEST(add_node_names_test)
  {
  alps_reservation ar;
  job              pjob;

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = NULL;
  fail_unless(add_node_names(&ar, &pjob) == -1, "add_node_names() should have returned -1");

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = eh1;
  fail_unless(add_node_names(&ar, &pjob) == 0, "failure in add_node_names()");
  fail_unless(ar.ar_node_names.size() == 3, "didn't get the right count of nodes");

  ar.ar_node_names.clear();
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = eh2;
  fail_unless(add_node_names(&ar, &pjob) == 0, "failure in add_node_names()");
  }
END_TEST




START_TEST(populate_ar_test)
  {
  alps_reservation *ar;
  job               pjob;

  strcpy(pjob.ji_qs.ji_jobid, jobids[0]);
  pjob.ji_internal_id = internal_job_ids[0];
  pjob.ji_wattr[JOB_ATR_reservation_id].at_val.at_str = rsvids[0];
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = eh1;

  fail_unless((ar = populate_alps_reservation(&pjob)) != NULL, "couldn't create an alps reservation");
  fail_unless(ar->internal_job_id == internal_job_ids[0], "job id created incorrectly");
  fail_unless(!strcmp(ar->rsv_id, rsvids[0]), "rsv id created incorrectly");

  delete ar;

  strcpy(pjob.ji_qs.ji_jobid, jobids[1]);
  pjob.ji_internal_id = internal_job_ids[1];
  pjob.ji_wattr[JOB_ATR_reservation_id].at_val.at_str = rsvids[1];
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = eh2;
  
  fail_unless((ar = populate_alps_reservation(&pjob)) != NULL, "couldn't create an alps reservation");
  fail_unless(ar->internal_job_id == internal_job_ids[1], "job id created incorrectly");
  fail_unless(!strcmp(ar->rsv_id, rsvids[1]), "rsv id created incorrectly");
  }
END_TEST



START_TEST(insert_create_inspect_test)
  {
  job               pjob;
  char              job_id[PBS_MAXSVRJOBID];

  strcpy(pjob.ji_qs.ji_jobid, jobids[0]);
  pjob.ji_wattr[JOB_ATR_reservation_id].at_val.at_str = rsvids[0];
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = eh1;

  alps_reservations.lock();
  alps_reservations.clear();
  alps_reservations.unlock();

  fail_unless(track_alps_reservation(&pjob) == 0, "couldn't create the reservation");
  alps_reservations.lock();
  fail_unless(alps_reservations.count() == 1, "incorrect count of reservations");
  pjob.ji_wattr[JOB_ATR_reservation_id].at_val.at_str = NULL;
  fail_unless(track_alps_reservation(&pjob) == 0, "track_alps_reservation failed with empty job");
  fail_unless(alps_reservations.count() == 1, "incorrect count after empty job");
  alps_reservations.unlock();

  strcpy(pjob.ji_qs.ji_jobid, jobids[0]);
  pjob.ji_wattr[JOB_ATR_reservation_id].at_val.at_str = rsvids[1];
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = eh1;
  fail_unless(track_alps_reservation(&pjob) == 0, "couldn't create the reservation");

  strcpy(pjob.ji_qs.ji_jobid, jobids[0]);
  pjob.ji_wattr[JOB_ATR_reservation_id].at_val.at_str = rsvids[2];
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = eh1;
  fail_unless(track_alps_reservation(&pjob) == 0, "couldn't create the reservation");

  strcpy(pjob.ji_qs.ji_jobid, jobids[0]);
  pjob.ji_wattr[JOB_ATR_reservation_id].at_val.at_str = rsvids[3];
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = eh1;
  fail_unless(track_alps_reservation(&pjob) == 0, "couldn't create the reservation");

  fail_unless(already_recorded(rsvids[0]) == 1, "rsv_id 0 not found");
  fail_unless(already_recorded(rsvids[1]) == 1, "rsv_id 0 not found");
  fail_unless(already_recorded(rsvids[2]) == 1, "rsv_id 0 not found");
  fail_unless(already_recorded(rsvids[3]) == 1, "rsv_id 0 not found");
  fail_unless(already_recorded((char *)"tom") == 0,     "missing rsv_id somehow found");
  fail_unless(already_recorded((char *)"tommy") == 0,   "missing rsv_id somehow found");

  fail_unless(is_orphaned(rsvids[0], job_id) == true, "no job but not orphaned?");
  fail_unless(is_orphaned(rsvids[1], job_id) == false, "job 1 returned but orphaned?");
  fail_unless(is_orphaned(rsvids[2], job_id) == false, "job 2 returned but orphaned?");
  fail_unless(is_orphaned(rsvids[3], job_id) == true, "completed job but not orphaned?");

  fail_unless(remove_alps_reservation((char *)"00") == THING_NOT_FOUND, "found something that doesn't exist");
  fail_unless(remove_alps_reservation(rsvids[0]) == 0, "couldn't remove reservation 1");
  fail_unless(remove_alps_reservation(rsvids[1]) == 0, "couldn't remove reservation 2");
  fail_unless(remove_alps_reservation(rsvids[2]) == 0, "couldn't remove reservation 3");
  fail_unless(remove_alps_reservation(rsvids[3]) == 0, "couldn't remove reservation 4");
  }
END_TEST




Suite *track_alps_reservations_suite(void)
  {
  Suite *s = suite_create("track_alps_reservations suite methods");

  TCase *tc_core = tcase_create("add_node_names_test");
  tcase_add_test(tc_core, add_node_names_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("populate_ar_test");
  tcase_add_test(tc_core, populate_ar_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("insert_create_inspect_test");
  tcase_add_test(tc_core, insert_create_inspect_test);
  suite_add_tcase(s, tc_core);
  
  return(s);
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(track_alps_reservations_suite());
  srunner_set_log(sr, "track_alps_reservations_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }

