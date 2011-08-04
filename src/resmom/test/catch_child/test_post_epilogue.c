#include "test_catch_child.h"
#include "catch_child.h"
#include <errno.h>
#include <stdlib.h>

extern int tc;
extern int func_num;
extern int errno;
extern int LOGLEVEL;

START_TEST(test_pe_negsock)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int ev = 0;
  int res = 0;
  strcpy(pjob->ji_qs.ji_jobid, "hithere");
  func_num = POST_EPILOGUE;
  tc = 1;
  errno = EINTR;
  LOGLEVEL = 2;
  res = post_epilogue(pjob, ev);
  fail_unless(res == 1, "This didn't return correctly");
  free(pjob);
  }
END_TEST

START_TEST(test_pe_negpostsock)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int ev = 0;
  int res = 0;
  strcpy(pjob->ji_qs.ji_jobid, "hithere");
  func_num = POST_EPILOGUE;
  tc = 2;
  errno = ETIMEDOUT;
  res = post_epilogue(pjob, ev);
  fail_unless(res == 1, "This didn't return correctly");
  free(pjob);
  }
END_TEST

START_TEST(test_pe_prnull)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int ev = 0;
  int res = 0;
  strcpy(pjob->ji_qs.ji_jobid, "hithere");
  func_num = POST_EPILOGUE;
  tc = 3;
  errno = EINPROGRESS;
  res = post_epilogue(pjob, ev);
  free(pjob);
  }
END_TEST

START_TEST(test_pe_disreqhdr)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int ev = 0;
  int res = 0;
  strcpy(pjob->ji_qs.ji_jobid, "hithere");
  func_num = POST_EPILOGUE;
  tc = 4;
  res = post_epilogue(pjob, ev);
  fail_unless(res == 1, "This didn't return correctly");
  free(pjob);
  }
END_TEST

START_TEST(test_pe_disjobobit)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int ev = 0;
  int res = 0;
  strcpy(pjob->ji_qs.ji_jobid, "hithere");
  func_num = POST_EPILOGUE;
  tc = 5;
  res = post_epilogue(pjob, ev);
  fail_unless(res == 1, "This didn't return correctly");
  free(pjob);
  }
END_TEST

START_TEST(test_pe_disreqextend)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int ev = 0;
  int res = 0;
  strcpy(pjob->ji_qs.ji_jobid, "hithere");
  func_num = POST_EPILOGUE;
  tc = 6;
  res = post_epilogue(pjob, ev);
  fail_unless(res == 1, "This didn't return correctly");
  free(pjob);
  }
END_TEST

START_TEST(test_pe_disfail)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int ev = 0;
  int res = 0;
  strcpy(pjob->ji_qs.ji_jobid, "hithere");
  func_num = POST_EPILOGUE;
  tc = 7;
  res = post_epilogue(pjob, ev);
  free(pjob);
  fail_unless(res == 0, "This should have reached the end of the function");
  }
END_TEST

Suite *post_epilogue_suite(void)
  {
  Suite *s = suite_create("post_epilogue methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_pe_negsock);
  tcase_add_test(tc_core, test_pe_negpostsock);
  tcase_add_test(tc_core, test_pe_prnull);
  tcase_add_test(tc_core, test_pe_disreqhdr);
  tcase_add_test(tc_core, test_pe_disjobobit);
  tcase_add_test(tc_core, test_pe_disreqextend);
  tcase_add_test(tc_core, test_pe_disfail);
  suite_add_tcase(s, tc_core);
  return s;
  }

void rundebug()
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int ev = 0;
  int res = 0;
  strcpy(pjob->ji_qs.ji_jobid, "hithere");
  func_num = POST_EPILOGUE;
  tc = 1;
  errno = EINTR;
  LOGLEVEL = 7;
  res = post_epilogue(pjob, ev);
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(post_epilogue_suite());
  srunner_set_log(sr, "post_epilogue_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
