#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "test_u_dynamic_string.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

char  buf[4096];

char *strings[] = {"tom", "george", "bart", "da bears", "glockenspiel", "frijoles" };
int   strings_len = 6; /* number of strings in strings */

char *sentence[] = {"I ", "am ", "going ", "to ", "fly ", "to ", "the ", "moon ", "alsdkjfasdl;jkfasdlkj ",
                     "'tiki' ", "slartibartfast & ", "<jimbo> ", " says I\""};
int   num_words = 13; /* number of strings in sentence */
char *sentence_concat = "I am going to fly to the moon alsdkjfasdl;jkfasdlkj 'tiki' slartibartfast & <jimbo>  says I\"";
char *sentence_zero_to_six = "I am going to fly to the ";
char *sentence_seven_to_end = "moon alsdkjfasdl;jkfasdlkj 'tiki' slartibartfast & <jimbo>  says I\"";
char *sentence_three_to_ten = "to fly to the moon alsdkjfasdl;jkfasdlkj 'tiki' slartibartfast & ";
char *sentence_escaped = "I am going to fly to the moon alsdkjfasdl;jkfasdlkj &apos;tiki&apos; slartibartfast &amp; &lt;jimbo&gt;  says I&quot;";
char *sentence_seven_end_escaped = "moon alsdkjfasdl;jkfasdlkj &apos;tiki&apos; slartibartfast &amp; &lt;jimbo&gt;  says I&quot;";
char *sentence_three_ten_escaped = "to fly to the moon alsdkjfasdl;jkfasdlkj &apos;tiki&apos; slartibartfast &amp; ";


START_TEST(initialize)
  {
  int             i;
  dynamic_string *ds = get_dynamic_string(-1, NULL);

  fail_unless((ds->used == 0), "initial used amount is wrong");
  fail_unless((ds->str[0] == '\0'), "string not empty after initialization");
  fail_unless((ds->size == DS_INITIAL_SIZE), "initial size is wrong");

  free_dynamic_string(ds);

  for (i = 0; i < strings_len; i++)
    {
    ds = get_dynamic_string(20, strings[i]);

    sprintf(buf, "Initial size is %d but should be 20", (int)ds->size);
    fail_unless((ds->size == 20), buf);

    sprintf(buf, "Used is %d but should be %d for string %s",
      (int)ds->used, (int)strlen(strings[i]) + 1, strings[i]);
    fail_unless((ds->used == strlen(strings[i]) + 1), buf);

    sprintf(buf, "Initial string is %s but should be %s", ds->str, strings[i]);
    fail_unless((!strcmp(ds->str, strings[i])), buf);

    clear_dynamic_string(ds);
    fail_unless(ds->used == 0, "used not cleared");
    fail_unless(ds->str[0] == '\0', "string not cleared");

    free_dynamic_string(ds);
    }

  }
END_TEST

START_TEST(appending_and_resizing)
  {
  int                  i;
  /* initialized to 1 because the first string uses strlen + 1, and each
   * subsequent string only adds strlen */
  int                  correct_strlen = 1;
  dynamic_string *ds = get_dynamic_string(6, NULL);

  /* add the entire sentence */
  for (i = 0; i < num_words; i++)
    {
    append_dynamic_string(ds, sentence[i]);
    correct_strlen += strlen(sentence[i]);

    sprintf(buf, "Used should be %d but is %d at iteration %d", correct_strlen, (int)ds->used, i);
    fail_unless(((int)ds->used == correct_strlen), buf);
    }

  sprintf(buf, "Size should be %d but is %d after appending the entire sentence", 96, (int)ds->size);
  fail_unless((96 == ds->size), buf);

  sprintf(buf, "String should be %s but is %s after appending the entire sentence",
    sentence_concat, ds->str);
  fail_unless((!strcmp(sentence_concat, ds->str)), buf);

  free_dynamic_string(ds);

  /* now add words 0-6 */
  ds = get_dynamic_string(8, NULL);
  correct_strlen = 1;

  for (i = 0; i < 7; i++)
    {
    append_dynamic_string(ds, sentence[i]);
    correct_strlen += strlen(sentence[i]);

    sprintf(buf, "Used should be %d but is %d at iteration %d of append test two",
      correct_strlen, (int)ds->used, i);
    fail_unless((correct_strlen == (int)ds->used), buf);
    }

  sprintf(buf, "Size should be %d but is %d after appending words 0-6", 32, (int)ds->size);
  fail_unless((32 == ds->size), buf);

  sprintf(buf, "String should be %s but is %s after appending words 0-6", sentence_zero_to_six, ds->str);
  fail_unless((!strcmp(ds->str, sentence_zero_to_six)), buf);

  free_dynamic_string(ds);

  /* now add words 7-12 */
  ds = get_dynamic_string(8, NULL);
  correct_strlen = 1;

  for (i = 7; i < num_words; i++)
    {
    append_dynamic_string(ds, sentence[i]);
    correct_strlen += strlen(sentence[i]);

    sprintf(buf, "Used should be %d but is %d at iteration %d of append test three",
      correct_strlen, (int)ds->used, i);
    fail_unless((correct_strlen == (int)ds->used), buf);
    }

  sprintf(buf, "Size should be 128 but is %d after appending words 7-12", (int)ds->size);
  fail_unless((128 == ds->size), buf);

  sprintf(buf, "String should be %s but is %s after appending words 7-12", sentence_seven_to_end, ds->str);
  fail_unless((!strcmp(ds->str, sentence_seven_to_end)), buf);

  free_dynamic_string(ds);

  /* now add words 3-10 */
  ds = get_dynamic_string(8, NULL);
  correct_strlen = 1;

  for (i = 3; i < 11; i++)
    {
    append_dynamic_string(ds, sentence[i]);
    correct_strlen += strlen(sentence[i]);

    sprintf(buf, "Used should be %d but is %d at iteration %d of append test four",
      correct_strlen, (int)ds->used, i);
    fail_unless((correct_strlen == (int)ds->used), buf);
    }

  sprintf(buf, "Size should be 128 buf is %d after appending words 3-10", (int)ds->size);
  fail_unless((128 == ds->size), buf);

  sprintf(buf, "String should be %s but is %s after appending words 3-10", sentence_three_to_ten, ds->str);
  fail_unless((!strcmp(ds->str, sentence_three_to_ten)), buf);

  clear_dynamic_string(ds);
  fail_unless(ds->used == 0, "used not cleared");
  fail_unless(ds->str[0] == '\0', "string not cleared");

  free_dynamic_string(ds);
  }
END_TEST

START_TEST(appending_xml)
  {
  int             i;
  dynamic_string *ds = get_dynamic_string(6, NULL);

  /* add the entire sentence */
  for (i = 0; i < num_words; i++)
    append_dynamic_string_xml(ds, sentence[i]);

  sprintf(buf, "String should be %s but is %s after xml appending the entire sentence",
    sentence_escaped, ds->str);
  fail_unless((!strcmp(sentence_escaped, ds->str)), buf);

  sprintf(buf, "Used should be %d but is %d after xml appending the entire sentence",
    (int)strlen(sentence_escaped) + 1, (int)ds->used);
  fail_unless((ds->used == strlen(sentence_escaped) + 1), buf);

  free_dynamic_string(ds);

  /* now add words 0-6 */
  ds = get_dynamic_string(8, NULL);

  for (i = 0; i < 7; i++)
    append_dynamic_string_xml(ds, sentence[i]);

  sprintf(buf, "String should be %s but is %s after xml appending words 0-6",
    sentence_zero_to_six, ds->str);
  fail_unless((!strcmp(ds->str, sentence_zero_to_six)), buf);

  sprintf(buf, "Used should be %d but is %d after xml appending words 0-6", 
    (int)strlen(sentence_zero_to_six) + 1, (int)ds->used);
  fail_unless((ds->used == strlen(sentence_zero_to_six) + 1), buf);

  free_dynamic_string(ds);

  /* now add words 7-12 */
  ds = get_dynamic_string(8, NULL);

  for (i = 7; i < num_words; i++)
    append_dynamic_string_xml(ds, sentence[i]);

  sprintf(buf, "String should be %s but is %s after xml appending words 7-12",
     sentence_seven_end_escaped, ds->str);
  fail_unless((!strcmp(ds->str, sentence_seven_end_escaped)), buf);

  sprintf(buf, "Used should be %d but is %d after xml appending words 7-12",
    (int)strlen(sentence_seven_end_escaped) + 1, (int)ds->used);
  fail_unless((ds->used == strlen(sentence_seven_end_escaped) + 1), buf);

  free_dynamic_string(ds);

  /* now add words 3-10 */
  ds = get_dynamic_string(8, NULL);

  for (i = 3; i < 11; i++)
    append_dynamic_string_xml(ds, sentence[i]);

  sprintf(buf, "String should be %s but is %s after xml appending words 3-10",
     sentence_three_ten_escaped, ds->str);
  fail_unless((!strcmp(ds->str, sentence_three_ten_escaped)), buf);

  sprintf(buf, "Used should be %d but is %d after xml appending words 3-10",
    (int)strlen(sentence_three_ten_escaped) + 1, (int)ds->used);
  fail_unless((ds->used == strlen(sentence_three_ten_escaped) + 1), buf);

  clear_dynamic_string(ds);
  fail_unless(ds->used == 0, "used not cleared");
  fail_unless(ds->str[0] == '\0', "string not cleared");

  free_dynamic_string(ds);
  }
END_TEST



START_TEST(getter)
  {
  dynamic_string *ds;
  char           *string;

  ds = get_dynamic_string(-1, NULL);
  string = get_string(ds);

  fail_unless((strlen(string) == 0), "length of string is nonzero");
  fail_unless(string == ds->str, "String pointers do not match in test 1");
  free_dynamic_string(ds);

  ds = get_dynamic_string(-1, "bread");
  string = get_string(ds);
  fail_unless(string == ds->str, "String pointers do not match in test 2");

  clear_dynamic_string(ds);
  fail_unless(ds->used == 0, "used not cleared");
  fail_unless(ds->str[0] == '\0', "string not cleared");

  free_dynamic_string(ds);
  }
END_TEST



START_TEST(series_of_strings)
  {
  dynamic_string *ds = get_dynamic_string(-1, NULL);
  int             i;
  size_t          correct_len = 0;
  size_t          initial_len;
  char           *str;

  for (i = 0; i < num_words; i++)
    {
    copy_to_end_of_dynamic_string(ds, sentence[i]);
    correct_len += strlen(sentence[i]) + 1;

    /* verify length */
    snprintf(buf, sizeof(buf),
      "Used should be %d but is %d after adding word %d",
      (int)correct_len, (int)ds->used, i);
    fail_unless((correct_len == ds->used), buf);
    }

  /* make sure the correct strings are stored */
  str = ds->str;
  i = 0;
  while ((str != NULL) &&
         (*str != '\0'))
    {
    snprintf(buf, sizeof(buf),
      "String should be %s but is %s at word %d",
      sentence[i], str, i);
    fail_unless((!strcmp(str, sentence[i])), buf);

    str += strlen(str) + 1;
    i++;
    }

  initial_len = correct_len;

  /* test string deletion */
  for (i = 0; i < num_words; i++)
    {
    delete_last_word_from_dynamic_string(ds);
    correct_len -= strlen(sentence[num_words - i - 1]) + 1;

    snprintf(buf, sizeof(buf),
      "Used should be %d but is %d after deleting the %dth last word started at %d",
      (int)correct_len, (int)ds->used, i, (int)initial_len);
    fail_unless((correct_len == ds->used), buf);
    }

  clear_dynamic_string(ds);
  fail_unless(ds->used == 0, "used not cleared");
  fail_unless(ds->str[0] == '\0', "string not cleared");

  free_dynamic_string(ds);
  }
END_TEST




START_TEST(char_size_test)
  {
  dynamic_string    *ds = get_dynamic_string(-1, NULL);
  char              *str = "a monday";
  int                num_chars = 8;
  int                i;
  struct size_value  sz;

  for (i = 0; i < num_chars; i++)
    {
    append_char_to_dynamic_string(ds, str[i]);
    }

  snprintf(buf, sizeof(buf), "string should be %s but is %s",
    str, ds->str);
  fail_unless(!strcmp(ds->str, str), buf);

  clear_dynamic_string(ds);


  sz.atsv_num = 5;
  sz.atsv_shift = 10;

  size_to_dynamic_string(ds, sz);
  snprintf(buf, sizeof(buf), "string should be 5kb but is %s", ds->str);
  fail_unless(!strcmp(ds->str, "5kb"), buf);

  clear_dynamic_string(ds);
  sz.atsv_shift = 20;
  size_to_dynamic_string(ds, sz);
  snprintf(buf, sizeof(buf), "string should be 5mb but is %s", ds->str);
  fail_unless(!strcmp(ds->str, "5mb"), buf);

  clear_dynamic_string(ds);
  sz.atsv_shift = 30;
  size_to_dynamic_string(ds, sz);
  snprintf(buf, sizeof(buf), "string should be 5gb but is %s", ds->str);
  fail_unless(!strcmp(ds->str, "5gb"), buf);

  clear_dynamic_string(ds);
  sz.atsv_shift = 40;
  size_to_dynamic_string(ds, sz);
  snprintf(buf, sizeof(buf), "string should be 5tb but is %s", ds->str);
  fail_unless(!strcmp(ds->str, "5tb"), buf);

  clear_dynamic_string(ds);
  sz.atsv_shift = 50;
  size_to_dynamic_string(ds, sz);
  snprintf(buf, sizeof(buf), "string should be 5pb but is %s", ds->str);
  fail_unless(!strcmp(ds->str, "5pb"), buf);
  }
END_TEST





Suite *u_dynamic_string_suite(void)
  {
  Suite *s = suite_create("u_dynamic_string_suite methods");
  TCase *tc_core = tcase_create("initialize");
  tcase_add_test(tc_core, initialize);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("appending_and_resizing");
  tcase_add_test(tc_core, appending_and_resizing);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("appending_xml");
  tcase_add_test(tc_core, appending_xml);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("getter");
  tcase_add_test(tc_core, getter);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("series_of_strings");
  tcase_add_test(tc_core, series_of_strings);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("char_size_test");
  tcase_add_test(tc_core, char_size_test);
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
  sr = srunner_create(u_dynamic_string_suite());
  srunner_set_log(sr, "u_dynamic_string_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }

