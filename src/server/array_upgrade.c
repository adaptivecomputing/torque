#include <pbs_config.h>

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


#include "array.h"


extern char *path_arrays;


/* an example prototpe of an upgrade function for a specific version of the struct
   each upgrade function should upgrade a specific version of the struct to the 
   current version!
int array_upgrade_v1(job_array *pa, int fds, int version, int *old_version);
 */

/* old versions of the struct.  These should have hard coded array sizes
   rather than using #defines that can change between version */
struct array_info_v1
  {
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
    log_err(-1, "array_upgrade", log_buffer);

    return -1;
    }

  /* call the appropriate upgrader function, if we ever wish to depricate an old
     array struct version, then we should delete the case from this if else-if 
     block and delete its upgrader function */

  /* version 1 is depricated, but this code is left commented out as an example 
  if (version == 1)
    {

    return array_upgrade_v1(pa, fds, version, old_version);

    }
   */
  if (version < 3)
    {
    /* these versions are depricated, documentation includes a warning 
       about the incompatibility and inability to upgrade, print a quick and 
       dirty error message and exit */
       
    sprintf(log_buffer, "WARNING, unable to upgrade job array\n"
            "structs from versions prior to 2.5.0.\n"
            "Please downgrade TORQUE and upgrade once no job arrays are queued\n");
    log_err(-1, "array_upgrade", log_buffer);
    exit(1);
    }
  else
    {
    if(version == 3)
      {
      /* Between version 3 and version 4 the element ai_qs.num_purged
         was added. We need to account for the new element from version
         3 arrays. */
      return 0;
      }
    return 1;
    }
  }

/* upgrader functions - upgrade from a specific version to the current version
   each time the array structure is updated these functions need to be updated */


/* here is an example that upgraded from ai_qs version 1 to version 2
   This version of the struct has been depricated, but this code is left here
   as an example of an upgrade function */
   
/*   
int array_upgrade_v1(job_array *pa, int fds, int version, int *old_version)
  {

  struct array_info_v1 old_array_info;
  array_request_node *rn;

  if (read(fds, (char*)&old_array_info,
           sizeof(old_array_info)) != sizeof(old_array_info))
    {
    return -1;
    }

  pa->ai_qs.struct_version = ARRAY_QS_STRUCT_VERSION;

  pa->ai_qs.array_size = old_array_info.array_size;
  pa->ai_qs.num_cloned = old_array_info.num_cloned;
  strcpy(pa->ai_qs.owner, old_array_info.owner);
  strcpy(pa->ai_qs.parent_id, old_array_info.parent_id);

  strncpy(pa->ai_qs.fileprefix, old_array_info.fileprefix, PBS_JOBBASE);
  strcpy(pa->ai_qs.submit_host, old_array_info.submit_host);

  *old_version = old_array_info.struct_version;

  if (old_array_info.num_cloned < old_array_info.array_size)
    {
    rn = (array_request_node*)malloc(sizeof(array_request_node));
    rn->start = old_array_info.num_cloned - 1;
    rn->end = old_array_info.array_size - 1;

    CLEAR_LINK(rn->request_tokens_link);
    append_link(&pa->request_tokens, &rn->request_tokens_link, (void*)rn);
    }


  return 0;

  }
*/
