#include "resource.h" /* resource_def */
#include "pbs_job.h" /* job_file_delete_info */

resource_def *svr_resc_def;
int           svr_resc_size = 0;


job * alloc_job()
  {
  return (job *)calloc(1,sizeof(job));
  }

resource_def *find_resc_def(resource_def *svr_resc, char const * name, int max_index)
  {
  return(NULL);
  }
