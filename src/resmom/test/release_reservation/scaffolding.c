#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "log.h"
#include "alps_constants.h"

char log_buffer[LOG_BUF_SIZE];
int  LOGLEVEL = 10;

void log_err(int errnum, const char *routine, const char *text) {}
void log_event(int a, int b, const char *routine, const char *text) {}

int find_error_type(

  xmlNode *node)

  {
  char    *attr_val = (char *)xmlGetProp(node, (const xmlChar *)error_class);
  int      rc = apbasil_fail_transient;
  xmlNode *child;
  xmlNode *grandchild;

  if (!strcmp(attr_val, "PERMANENT"))
    rc = apbasil_fail_permanent;

  for (child = node->children; child != NULL; child = child->next)
    {
    if (!strcmp((const char *)child->name, message))
      {
      for (grandchild = child->children; grandchild != NULL; grandchild = grandchild->next)
        {
        if (!strcmp((const char *)grandchild->name, text_name))
          {
          char *errtext = (char *)xmlNodeGetContent(grandchild);
          if (strstr(errtext, "error") != NULL)
            {
            snprintf(log_buffer, sizeof(log_buffer), "%s alps error: '%s'",
              attr_val, errtext);
            log_err(-1, __func__, log_buffer);
            }
          }
        }
      }
    }

  free(attr_val);

  return(rc);
  }

