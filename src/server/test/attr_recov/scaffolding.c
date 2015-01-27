#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <string.h> /* strchr */
#include <ctype.h> /* isspace */

#include "list_link.h" /* list_link */
#include "attribute.h" /* pbs_attribute, attribute_def */
#include <string>


ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int write_buffer(char *buf, int len, int fds)
  {
  fprintf(stderr, "The call to write_buffer needs to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int attr_to_str(std::string&, attribute_def *at_def, struct pbs_attribute attr, bool XML)
  {
  fprintf(stderr, "The call to attr_to_str needs to be mocked!!\n");
  exit(1);
  }
#if 0


void clear_dynamic_string(dynamic_string *ds)
  {
  fprintf(stderr, "The call to attr_to_str needs to be mocked!!\n");
  exit(1);
  }

dynamic_string *get_dynamic_string(int initial_size, const char *str)
  {
  fprintf(stderr, "The call to attr_to_str needs to be mocked!!\n");
  exit(1);
  }

void free_dynamic_string(dynamic_string *ds)
  {
  fprintf(stderr, "The call to attr_to_str needs to be mocked!!\n");
  exit(1);
  }
#endif


int ctnodes(
  char *spec
  )
  {
  return(1);
  }

/**
 * Gets the number of items in a string list.
 * @param csv_str  The string list.
 * @return The number of items in the list.
 */
int csv_length(const char *csv_str)
  {
  int  length = 0;
  const char *cp;

  if (!csv_str || *csv_str == 0)
    return(0);

  length++;

  cp = csv_str;

  while ((cp = strchr(cp, ',')))
    {
    cp++;
    length++;
    }

  return(length);
  }



/**
 * Gets the nth item from a comma seperated list of names.
 * @param csv_str  The string list.
 * @param n The item number requested (0 is the first item).
 * @return Null if csv_str is null or empty,
 *     otherwise, a pointer to a local buffer containing the nth item.
 */
#define NBUFFERS        32
char *csv_nth(const char *csv_str, int n)
  {
  int  i;
  const char *cp;
  char *tp;
  static char buffer[NBUFFERS][128];
  static  int     buffer_index;

  if (!csv_str || *csv_str == 0)
    return(0);

  cp = csv_str;

  for (i = 0; i < n; i++)
    {
    if (!(cp = strchr(cp, ',')))
      {
      return(0);
      }

    cp++;
    }

  buffer_index++;

  if (buffer_index >= NBUFFERS)
    buffer_index = 0;

  memset(buffer[buffer_index], 0, sizeof(buffer[buffer_index]));

  if ((tp = strchr((char *)cp, ',')))
    {
    if ((tp - cp) > 128)
      return 0;
    /* Does this need to be null terminated? */
    strncpy(buffer[buffer_index], cp, tp - cp);
    }
  else
    {
    snprintf(buffer[buffer_index], 128, "%s", cp);
    }

  return(buffer[buffer_index]);
  }


/**
 *
 * csv_find_string
 *
 * Search a csv list for an entry that matches a specified search string.
 */
char *csv_find_string(
    
  const char *csv_str,
  const char *search_str)

  {
  int  i;
  int  nitems;
  int  search_length = 0;
  char *cp;

  if (!search_str)
    return(NULL);

  search_length = strlen(search_str);

  nitems = csv_length(csv_str);

  for (i = 0; i < nitems; i++)
    {
    cp = csv_nth(csv_str, i);

    if (cp)
      {
      while (isspace(*cp))
        cp++;

      if ((int)strlen(cp) >= search_length &&
          !isalpha(cp[search_length]) &&
          !strncmp(cp, search_str, search_length))
        {
        return(cp);
        }
      }
    }

  return(NULL);
  }

char saveBuff[4096];
int saveBuffRdPtr;
int saveBuffEndPtr;

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  int lenRead = 0;
  int i;
  char *buff = (char *)buf;
  for(i=0;(i<count)&&(saveBuffRdPtr < saveBuffEndPtr);i++)
    {
    *buff++ = saveBuff[saveBuffRdPtr++];
    lenRead++;
    }
  return (lenRead);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

