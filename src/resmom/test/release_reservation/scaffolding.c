#include <stdlib.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "log.h"

char log_buffer[LOG_BUF_SIZE];
int  LOGLEVEL = 10;

void log_err(int errnum, const char *routine, char *text) {}

int find_error_type(

  xmlNode *node)

  {
  return(0);
  }

void log_event(

  int         eventtype,
  int         objclass,
  const char *objname,
  char       *text)
  {
  }
