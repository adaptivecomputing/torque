#include <pbs_config.h>

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


#include "array.h"


extern char *path_arrays;


/* old versions of the struct.  These should have hard coded array sizes
   rather than using #defines that can change between version */
int array_upgrade_v1(job_array *pa, int fds, int version, int *old_version);

struct array_info_v1 {
  int  struct_version;
  int  array_size;
  int  num_cloned;
  char owner[82]; 
  char parent_id[86];
  char fileprefix[12];
  char submit_host[65];
};


int array_upgrade(job_array *pa, int fds, int version, int *old_version)
  {

  /* reset the file descriptor */
  if (lseek(fds, 0, SEEK_SET) != 0)
    {
    sprintf(log_buffer, "unable to reset fds\n");
    log_err(-1,"array_upgrade",log_buffer);

    return -1;
    }

  /* call the appropriate upgrader function */
  if (version == 1)
    {
    return array_upgrade_v1(pa, fds, version, old_version);
    }
  else
    {
    return 1;
    }  
  }
  
/* upgrader functions - upgrade from a specific version to the current version
   each time the array structure is updated these functions need to be updated */  

int array_upgrade_v1(job_array *pa, int fds, int version, int *old_version)
  {
  struct array_info_v1 old_array_info;
  array_request_node *rn;

  if (read(fds,(char*)&old_array_info,
      sizeof(old_array_info)) != sizeof(old_array_info))
    {
    return -1;
    }

  pa->ai_qs.struct_version = ARRAY_QS_STRUCT_VERSION;
  pa->ai_qs.array_size = old_array_info.array_size;
  pa->ai_qs.num_cloned = old_array_info.num_cloned;
  strcpy(pa->ai_qs.owner,old_array_info.owner);
  strcpy(pa->ai_qs.parent_id,old_array_info.parent_id);

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


  return 0;

  }
