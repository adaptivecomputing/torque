
#include <pbs_config.h>
#include <string>
#include <string.h>
#include <stdlib.h>

#include "pbs_job.h"
#include "resource.h"
#include "pbs_error.h"
#include "resmon.h"

/*
 * parse_rur_line()
 *
 * Parses a line of an rur file to find the stats for this job. 
 * @param line - the line of the file
 * @param pjob - the job whose stats we're finding
 */

// RUR usage lines look like this
// 2013-11-02T11:09:49.457770-05:00 c0-0c1s1n2 RUR 2417 p0-201311-1t153028 [RUR@34] uid: 12345, apid: 86989, jobid: 0, cmdname: /lus/tmp/rur01.2338/./CPU01-2338 plugin: taskstats ['utime', 10000000, 'stime', 0, 'max_rss', 940, 'rchar', 107480, 'wchar', 90, 'exitcode:signal', ['0:0'], 'core', 0]


int parse_rur_line(

  const std::string &line,
  mom_job           *pjob)

  {
  size_t pos = line.find("taskstats");

  if (pos != std::string::npos)
    {
    if (line.at(pos) == ' ')
      pos++;
    if (line.at(pos) == '[')
      pos++;

    std::string   stats = line.substr(pos, std::string::npos);
    char         *work_str = strdup(stats.c_str());
    char         *ptr = strstr(work_str, "utime',");
    long          cput = 0;
    long long     mem = 0;
    resource_def *rd;
    resource     *pres;

    if (ptr != NULL)
      {
      ptr += 7; // move past "utime',"

      if (*ptr == ' ')
        ptr++;

      cput = strtol(ptr, &ptr, 10);
      }

    if ((ptr = strstr(ptr, "stime',")) != NULL)
      {
      ptr += 7; // move past "stime',"
      
      if (*ptr == ' ')
        ptr++;

      cput += strtol(ptr, &ptr, 10);
      }

    if (cput > 0)
      {
      rd = find_resc_def(svr_resc_def, "cput", svr_resc_size);
      pres = add_resource_entry(pjob->get_attr(JOB_ATR_resc_used), rd);
      pres->rs_value.at_flags |= ATR_VFLAG_SET;
      pres->rs_value.at_type = ATR_TYPE_LONG;
      pres->rs_value.at_val.at_long = MAX(cput, pres->rs_value.at_val.at_long);
      }

    if ((ptr = strstr(ptr, "max_rss',")) != NULL)
      {
      ptr += 9; // move past "max_rss'," 

      if (*ptr == ' ')
        ptr++;

      mem = strtoll(ptr, NULL, 10);

      if (mem > 0)
        {
        rd = find_resc_def(svr_resc_def, "mem", svr_resc_size);
        pres = add_resource_entry(pjob->get_attr(JOB_ATR_resc_used), rd);
        pres->rs_value.at_flags |= ATR_VFLAG_SET;
        pres->rs_value.at_type = ATR_TYPE_SIZE;
        pres->rs_value.at_val.at_size.atsv_shift = 10;
        pres->rs_value.at_val.at_size.atsv_units = ATR_SV_BYTESZ;
        pres->rs_value.at_val.at_size.atsv_num = MAX(mem, pres->rs_value.at_val.at_size.atsv_num);
        }
      }

    free(work_str);
    }

  return(PBSE_NONE);
  } // parse_rur_line()


