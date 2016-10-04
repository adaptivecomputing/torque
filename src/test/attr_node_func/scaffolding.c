#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "id_map.hpp"

#include "execution_slot_tracker.hpp"
#include "attribute.h" /* svrattrl */
#include "id_map.hpp"
#include "pbs_nodes.h"

id_map    job_mapper;

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

void populate_range_string_from_slot_tracker(
    
  const execution_slot_tracker &est,
  std::string &range_str) 
  
  {
  }


int ctnodes(const char *spec)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

int id_map::get_new_id(

  const char *name)

  {
  fprintf(stderr, "The call to %s needs to be mocked!!\n",__func__);
  exit(1);
  }



int id_map::get_id(

  const char *name)

  {
  fprintf(stderr, "The call to %s needs to be mocked!!\n",__func__);
  exit(1);
  }



const char *id_map::get_name(

  int id)

  {
  return(NULL);
  }


id_map::~id_map()
  {
  // leave the destructor blank. C++ destroys globally constructed
  // objects when the main thread exits, causing crashes for other threads.
  // This class is only destroyed at shutdown, and since we don't care about
  // memory usage at shutdown we'll just have the destructor do nothing.
  }



id_map::id_map() : counter(0)
  {
  str_map = new std::map<std::string,int>();
  names = new std::vector<std::string>();
  pthread_mutex_init(&mutex, NULL);
  }



id_map::id_map(const id_map &other) : counter(other.counter)
  {
  str_map = new std::map<std::string,int>(*other.str_map);
  names = new std::vector<std::string>(*other.names);
  pthread_mutex_init(&mutex, NULL);
  }

int csv_length(const char *csv_str)

  {
  return(0);
  }

char *csv_nth(const char *csv_str, int n)

  {
  return(NULL);
  }

char *csv_find_string(
    
  const char *csv_str,
  const char *search_str)

  {
  return(NULL);
  }

pbsnode::pbsnode() {}
pbsnode::~pbsnode() {}
