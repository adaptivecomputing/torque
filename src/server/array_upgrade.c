#include <pbs_config.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


#include "array.h"


extern char *path_arrays;

int array_upgrade_v1(job_array *pa, int *fds, int version, int *old_version);

struct array_info_v1 {
  int  struct_version;
  int  array_size;
  int  num_cloned;
  char owner[16 + 64 + 2]; 
  char parent_id[8 + 64 +5 + 6 + 2 + 1];
  char fileprefix[11 + 1];
  char submit_host[64 +1];
};


int array_upgrade(job_array *pa, int *fds, int version, int *old_version)
  {

  /* reset the file descriptor */
  if (lseek(*fds, 0, SEEK_SET) != 0)
    {
    sprintf(log_buffer, "unable to reset fds\n");
    log_err(-1,"array_upgrade",log_buffer);

    return -1;
    }

  if (version == 1)
    {
    return array_upgrade_v1(pa, fds, version, old_version);
    }
  else
    {
    return 1;
    }  
  }

int array_upgrade_v1(job_array *pa, int *fds, int version, int *old_version)
  {
  struct array_info_v1 old_array_info;
  array_request_node *rn;
/* 
  char namebuf1[MAXPATHLEN];
  char namebuf2[MAXPATHLEN];
*/
  if (read(*fds,(char*)&old_array_info,
      sizeof(old_array_info)) != sizeof(old_array_info))
    {
    return -1;
    }

  pa->ai_qs.struct_version = ARRAY_QS_STRUCT_VERSION;
  pa->ai_qs.array_size = old_array_info.array_size;
  pa->ai_qs.num_cloned = old_array_info.num_cloned;
  strcpy(pa->ai_qs.owner,old_array_info.owner);
  strcpy(pa->ai_qs.parent_id,old_array_info.parent_id);
/* no longer renaming files for recovered jobs/arrays 
  strncpy(pa->ai_qs.fileprefix, old_array_info.parent_id, PBS_JOBBASE);
*/
  strncpy(pa->ai_qs.fileprefix,old_array_info.fileprefix, PBS_JOBBASE);
  strcpy(pa->ai_qs.submit_host,old_array_info.submit_host);
 
  *old_version = old_array_info.struct_version;

  if (old_array_info.num_cloned < old_array_info.array_size)
    {
    rn = (array_request_node*)malloc(sizeof(array_request_node));
    rn->start = old_array_info.num_cloned-1;
    rn->end = old_array_info.array_size-1;

    CLEAR_LINK(rn->request_tokens_link);
    append_link(&pa->request_tokens,&rn->request_tokens_link,(void*)rn);
    }

#if 0
  /* no longer renaming files on upgrade,  
     TODO: GB remove code if we decide to not do rename files */
  if(strcmp(pa->ai_qs.fileprefix, old_array_info.fileprefix) != 0)
    {
    namebuf1[MAXPATHLEN - 1] = '\0';
    namebuf2[MAXPATHLEN - 1] = '\0';

    strncpy(namebuf1, path_arrays, MAXPATHLEN-1);
    strncat(namebuf1, old_array_info.fileprefix, 
            MAXPATHLEN - strlen(namebuf1) - 1);
    strncat(namebuf1, ARRAY_FILE_SUFFIX, 
            MAXPATHLEN - strlen(namebuf1) - 1);

    strncpy(namebuf2, path_arrays, MAXPATHLEN-1);
    strncat(namebuf2, pa->ai_qs.fileprefix, 
            MAXPATHLEN - strlen(namebuf2) - 1);
    strncat(namebuf2, ARRAY_FILE_SUFFIX, 
            MAXPATHLEN - strlen(namebuf2) - 1);

    if ((strcmp(&namebuf1[strlen(namebuf1) - strlen(ARRAY_FILE_SUFFIX)],
        ARRAY_FILE_SUFFIX) != 0) ||
        (strcmp(&namebuf2[strlen(namebuf2) - strlen(ARRAY_FILE_SUFFIX)],
        ARRAY_FILE_SUFFIX) != 0))
      {
      /*
         maybe we should exit with some kind of internal error? */
      return 1;
      }


    rename(namebuf1, namebuf2);

    }
#endif
  return 0;

  }
