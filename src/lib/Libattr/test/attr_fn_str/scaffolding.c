#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h" /* svrattrl */

extern char in_csv;
int ctnodes(char *spec)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

char* csv_find_string(char const* val1, char const* val2)
  {
  fprintf(stderr, "The call to csv_find_string needs to be mocked!!\n");
  return (in_csv)? (char*)&in_csv : 0;
  }
