#include <string.h>
#include <string>
#include "server.h"
#include "svrfunc.h"

/*
 * get_path_jobdata
 *
 * Return adjusted path name if needed for subdivided job data directories.
 *
 * For sites expected to have large numbers of queued jobs server_priv/jobs
 * (and arrays) can be subdivided into 10 directories (0..9) to have jobs
 * equally distributed among them to avoid many jobs all in one directory.
 *
 * Sites can chose to use this scheme by setting use_jobs_subdirs to
 * true in the server.
 *
 * This function implements a simple hashing scheme (last digit of the numeric
 * part of the job id) to contruct a path for scripts, job and array data that
 * enables them to be distributed to a numbered directory if use_jobs_subdirs
 * is true. If this parameter has not been set, then the basepath parameter
 * is returned.
 *
 * See buildutils/pbs_mkdirs.in for the required directories that are
 * contructed. See initialize_paths() in src/server/pbsd_init.c
 * for the code that checks the permissions of these directories.
 *
 * @param jobid (I) - job id
 * @param basepath (I) - the base path (most likely path_jobs or path_arrays)
 */

std::string get_path_jobdata(

  const char *jobid,
  const char *basepath)

  {
  std::string ret_path("");
  bool        use_jobs_subdirs = false;

  if ((jobid == NULL) || (basepath == NULL))
    return(ret_path);

  ret_path = basepath;

  // get use_jobs_subdirs value if set 
  get_svr_attr_b(SRV_ATR_use_jobs_subdirs, &use_jobs_subdirs);

  // if we are using divided subdirectories in server_priv/{jobs,arrays}
  //  then adjust path
  if ((use_jobs_subdirs == true) && isdigit(*jobid))
    {
    char *p = (char *)jobid + 1;

    // point p to the first non-digit in string
    while (isdigit(*p))
      p++;

    // move back 1 char to the last digit of the job id
    p--;

    // append the last digit of the numeric part of the job id on the string
    ret_path.push_back(*p);

    // append slash
    ret_path.push_back('/');
    }

  return(ret_path);
  }
