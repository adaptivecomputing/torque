#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

bool dot = false;
bool double_dot = false;
bool first_time = true;
bool dir_is_null = false;
bool time_expired = false;
bool stat_fail = false;


extern "C"
{
char *pbse_to_txt(int err)
  { 
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }

DIR *opendir(const char *name)
  {
  DIR *pointer;

  if (dir_is_null == true)
    return(NULL);

  pointer = (DIR *)malloc(10);
  return(pointer);
  }

int closedir(DIR *dirp)
  {
  if (dirp != NULL)
    free(dirp);
  return(0);
  }

struct dirent *readdir(DIR *dirp)
  {
  struct dirent *d_entry;

  if (dir_is_null == true)
    return(NULL);

  d_entry = (struct dirent *)malloc(sizeof(struct dirent));

  if ((dot == true) && (first_time == true))
    {
    first_time = false;
    d_entry->d_name[0] = '.';
    d_entry->d_name[2] = '\0';
    return(d_entry);
    }

  if ((double_dot == true) && (first_time == true))
    {
    first_time = false;
    d_entry->d_name[0] = '.';
    d_entry->d_name[1] = '.';
    d_entry->d_name[2] = '\0';

    return(d_entry);
    }

  if (first_time == true)
    {
    first_time = false;
    strcpy(d_entry->d_name, "somefile");
    return(d_entry);
    }

  return(NULL);
  }

}

int stat(const char *path, struct stat *buf) throw()
  {

  if (stat_fail == true)
    return(-1);

  if (time_expired == false)
    {
    buf->st_mtime  = time((time_t *)NULL);

    return(0);
    }

  buf->st_mtime = 10;
 

  return (0);
  }

int remove(const char *path) throw()
  {
  return(0);
  }


