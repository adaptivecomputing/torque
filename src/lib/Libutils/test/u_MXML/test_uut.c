#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"


/*
 * MXMLExtractE
 * MXMLSetChild
 * MXMLSetAttr
 * MXMLAppendAttr
 * MXMLSetVal
 * MXMLAddE
 * MXMLToXString
 * MXMLToString
 *
 *
 */

const char *sampleXML = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\
<!-- Edited by XMLSpy® -->\
<breakfast_menu>\
    <food>\
        <name>Belgian Waffles</name>\
        <price>$5.95</price>\
        <description>two of our famous Belgian Waffles with plenty of real maple syrup</description>\
        <calories>650</calories>\
        <extra parm=\"stuff\" str=\"string\" int=\"123\" long=\"3245\" double=\"3.14159\"/>\
    </food>\
    <food>\
        <name>Strawberry Belgian Waffles</name>\
        <price>$7.95</price>\
        <description>light Belgian waffles covered with strawberries and whipped cream</description>\
        <calories>900</calories>\
    </food>\
    <food>\
        <name>Berry-Berry Belgian Waffles</name>\
        <price>$8.95</price>\
        <description>light Belgian waffles covered with an assortment of fresh berries and whipped cream</description>\
        <calories>900</calories>\
    </food>\
    <food>\
        <name>French Toast</name>\
        <price>$4.50</price>\
        <description>thick slices made from our homemade sourdough bread</description>\
        <calories>600</calories>\
    </food>\
    <food>\
        <name>Homestyle Breakfast</name>\
        <price>$6.95</price>\
        <description>two eggs, bacon or sausage, toast, and our ever-popular hash browns</description>\
        <calories>950</calories>\
    </food>\
</breakfast_menu>";

const char *badXML = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\
<!-- Edited by XMLSpy® -->\
<breakfast_menu>\
    <food>\
        <name>Belgian Waffles</name>\
        <price>$5.95</price>\
        <description>two of our famous Belgian Waffles with plenty of real maple syrup</description>\
        <calories>650</calories>\
        <extra parm=\"stuff\"/>\
    </food>\
    <food>\
        <name>Strawberry Belgian Waffles</name>\
        <price>$7.95</price>\
        <description>light Belgian waffles covered with strawberries and whipped cream</description>\
        <calories>900</calories>\
    </food>\
    <fud>\
        <name>Berry-Berry Belgian Waffles</name>\
        <price>$8.95</price>\
        <description>light Belgian waffles covered with an assortment of fresh berries and whipped cream</description>\
        <calories>900</calories>\
    </food>\
    <food>\
        <name>French Toast</name>\
        <price>$4.50</price>\
        <description>thick slices made from our homemade sourdough bread</description>\
        <calories>600</calories>\
    </food>\
    <food>\
        <name>Homestyle Breakfast</name>\
        <price>$6.95</price>\
        <description>two eggs, bacon or sausage, toast, and our ever-popular hash browns</description>\
        <calories>950</calories>\
    </food>\
</breakfast_menu>";



START_TEST(test_one)
  {
  mxml_t *pXml = NULL;

  fail_unless(MXMLFromString(&pXml,(char *)sampleXML,NULL,NULL,0) == SUCCESS);
  fail_unless(MXMLDestroyE(&pXml) == SUCCESS);

  fail_unless(MXMLFromString(&pXml,(char *)badXML,NULL,NULL,0) != SUCCESS);
  fail_unless(MXMLDestroyE(&pXml) == SUCCESS);

  }
END_TEST

START_TEST(test_two)
  {
  mxml_t *pXml = NULL;
  mxml_t *pFood = NULL;
  mxml_t *pExtra = NULL;
  int i;
  long l;
  double d;
  char bf[10240];
  char *buff = NULL;

  fail_unless(MXMLFromString(&pXml,(char *)sampleXML,NULL,NULL,0) == SUCCESS);

  fail_unless(MXMLGetChild(pXml,(char *)"food",NULL,&pFood) == SUCCESS);
  fail_unless(MXMLGetChildCI(pFood,(char *)"EXtrA",NULL,&pExtra) == SUCCESS);
  fail_unless(MXMLGetAttr(pExtra,(char *)"parm",NULL,bf,sizeof(bf)) == SUCCESS);
  fail_unless(!strcmp(bf,"stuff"));

  fail_unless(MXMLGetAttrF(pExtra,(char *)"str",NULL,(void *)bf,mdfString,sizeof(bf)) == SUCCESS);
  fail_unless(!strcmp(bf,"string"));

  fail_unless(MXMLGetAttrF(pExtra,(char *)"int",NULL,(void *)&i,mdfInt,sizeof(i)) == SUCCESS);
  fail_unless(i == 123);
  fail_unless(MXMLGetAttrF(pExtra,(char *)"intless",NULL,(void *)&i,mdfInt,sizeof(i)) == FAILURE);
  fail_unless(i==0);


  fail_unless(MXMLGetAttrF(pExtra,(char *)"long",NULL,(void *)&l,mdfLong,sizeof(l)) == SUCCESS);
  fail_unless(l == 3245);
  fail_unless(MXMLGetAttrF(pExtra,(char *)"longless",NULL,(void *)&l,mdfLong,sizeof(l)) == FAILURE);
  fail_unless(l == 0);

  fail_unless(MXMLGetAttrF(pExtra,(char *)"double",NULL,(void *)&d,mdfDouble,sizeof(d)) == SUCCESS);
  fail_unless(d == 3.14159);
  fail_unless(MXMLGetAttrF(pExtra,(char *)"doubleless",NULL,(void *)&d,mdfDouble,sizeof(d)) == FAILURE);
  fail_unless(d == 0.0);

  fail_unless(MXMLToString(pXml,bf,sizeof(bf),NULL,true) == SUCCESS);
  fail_unless(MXMLToXString(pXml,&buff,&i,sizeof(bf),NULL,true) == SUCCESS);
  if (buff != NULL) free(buff);

  fail_unless(MXMLDestroyE(&pXml) == SUCCESS);


  }
END_TEST

const char *smallXML = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\
<!-- Edited by XMLSpy® -->\
<body>\
    <heart>\
        <valve>fluttery</valve>\
    </heart>\
</body>";


START_TEST(test_three)
  {
  mxml_t *pXml = NULL;
  mxml_t *pHeart = NULL;
  mxml_t *pValve = NULL;
  mxml_t *pOut = NULL;
  int   i = 31459;
  long l = 314159278;
  double d = 3.141592;

  fail_unless(MXMLFromString(&pXml,(char *)smallXML,NULL,NULL,0) == SUCCESS);
  fail_unless(MXMLGetChild(pXml,(char *)"heart",NULL,&pHeart) == SUCCESS);
  fail_unless(MXMLGetChild(pHeart,(char *)"valve",NULL,&pValve) == SUCCESS);
  fail_unless(MXMLExtractE(pXml,pValve,&pOut)==SUCCESS);
  fail_unless(MXMLGetChild(pHeart,(char *)"valve",NULL,&pValve) != SUCCESS);

  fail_unless(MXMLSetChild(pHeart,(char *)"valve",&pValve) == SUCCESS);
  for(i=0;i<67;i++)
    {
    fail_unless(MXMLSetAttr(pValve,(char *)"int",&i,mdfInt) == SUCCESS);
    }
  fail_unless(MXMLSetAttr(pValve,(char *)"long",&l,mdfLong) == SUCCESS);
  fail_unless(MXMLSetAttr(pValve,(char *)"double",&d,mdfDouble) == SUCCESS);

  fail_unless(MXMLAppendAttr(pOut,(char *)"stuff",(char *)"Brad",',') == SUCCESS);
  fail_unless(MXMLAppendAttr(pOut,(char *)"stuff",(char *)"Daw",',') == SUCCESS);

  fail_unless(MXMLSetVal(pOut,(void *)"Some Crap",mdfString) == SUCCESS);
  fail_unless(MXMLSetVal(pOut,&i,mdfInt) == SUCCESS);
  fail_unless(MXMLSetVal(pOut,&l,mdfLong) == SUCCESS);
  fail_unless(MXMLSetVal(pOut,&d,mdfDouble) == SUCCESS);

  }
END_TEST

Suite *u_MXML_suite(void)
  {
  Suite *s = suite_create("u_MXML_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_three");
  tcase_add_test(tc_core, test_three);
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
  sr = srunner_create(u_MXML_suite());
  srunner_set_log(sr, "u_MXML_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
