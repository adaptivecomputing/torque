#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "execution_slot_tracker.hpp"
#include "attribute.h" /* svrattrl */

svrattrl *attrlist_create(const char *aname, const char *rname, int vsize)
 {
 fprintf(stderr, "The call to attrlist_create needs to be mocked!!\n");
 exit(1);
 } 

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, const char *objname, const char *text)
  {
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
  exit(1);
  }

int set_str(struct pbs_attribute *attr, struct pbs_attribute *new_link, enum batch_op op)
  {
  fprintf(stderr, "The call to set_str needs to be mocked!!\n");
  exit(1);
  }

int set_arst(struct pbs_attribute *attr, struct pbs_attribute *new_link, enum batch_op op) 
  {
  fprintf(stderr, "The call to set_arst needs to be mocked!!\n");
  exit(1);
  }

char *parse_comma_string(char *start, char **ptr) 
  {
  fprintf(stderr, "The call to parse_comma_string needs to be mocked!!\n");
  exit(1);
  }

void populate_range_string_from_slot_tracker(execution_slot_tracker &est, std::string &range) {}


int ctnodes(char *spec)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }
