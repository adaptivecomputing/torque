
#include <pbs_config.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <stdlib.h>

#include "pbs_job.h"
#include "resource.h"
#include "pbs_error.h"
#include "resmon.h"

unsigned int             total_chars_read = 0;
ssize_t                  last_line = 0;
std::vector<std::string> task_lines;
// usage_information maps the job id to the usage reported
std::map<std::string, task_usage_info> usage_information;



/*
 * update_usage_information()
 *
 * Updates our map with the latest and greatest usage information
 */

void update_usage_information(

  std::string     &jobid,
  task_usage_info &tui)

  {
  std::map<std::string, task_usage_info>::iterator it = usage_information.find(jobid);

  if (it != usage_information.end())
    {
    if (tui.cput > it->second.cput)
      usage_information[jobid].cput = tui.cput;

    if (tui.mem > it->second.mem)
      usage_information[jobid].mem = tui.mem;
    }
  else
    usage_information[jobid] = tui;
  } // END update_usage_information()


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

  const std::string     &line,
  std::set<std::string> &jobs_this_iteration)

  {
  size_t pos = line.find("jobid: ");

  if (pos != std::string::npos)
    {
    std::string jobid(line.substr(pos + 7)); // + 7 to pass 'jobid: '
    pos = jobid.find(",");

    if (pos != std::string::npos)
      {
      jobid.erase(pos);

      if (jobs_this_iteration.find(jobid) != jobs_this_iteration.end())
        {
        // We're reading the file from the end, or most recent first, so there is no reason to
        // look at a second entry for a job.
        return(PBSE_NONE);
        }

      pos = line.find("taskstats [");

      if (pos != std::string::npos)
        {
        // Add 11 for 'taskstats ['
        std::string      stats = line.substr(pos + 11, std::string::npos);
        char            *work_str = strdup(stats.c_str());
        char            *ptr = work_str;
        char            *tmp_ptr = strstr(ptr, "utime',");
        task_usage_info  tui;

        if (tmp_ptr != NULL)
          {
          ptr = tmp_ptr + 7; // move past "utime',"

          if (*ptr == ' ')
            ptr++;

          tui.cput = strtol(ptr, &ptr, 10);
          }

        if ((tmp_ptr = strstr(ptr, "stime',")) != NULL)
          {
          ptr = tmp_ptr + 7; // move past "stime',"
          
          if (*ptr == ' ')
            ptr++;

          tui.cput += strtol(ptr, &ptr, 10);
          }

        if ((ptr = strstr(ptr, "max_rss',")) != NULL)
          {
          ptr += 9; // move past "max_rss'," 

          if (*ptr == ' ')
            ptr++;

          tui.mem = strtoll(ptr, NULL, 10);
          }

        jobs_this_iteration.insert(jobid);
        update_usage_information(jobid, tui);

        free(work_str);
        }
      }
    }
  

  return(PBSE_NONE);
  } // parse_rur_line()



/*
 * append_date()
 *
 * Appends the date format to the end of the task_stats path
 */

void append_date(

  std::string &path)

  {
  // NYI: find out the date format and append it
  // NYI: add logic to notice when it is a new day
  } // END append_date()



void read_rur_stats_file(
    
  const char *basepath)

  {
  std::string   path(basepath);

  append_date(path);

  std::ifstream task_file(path.c_str());

  if (task_file.is_open())
    {
    std::set<std::string> jobs_this_iteration;
    task_file.seekg(total_chars_read);
    last_line = task_lines.size();

    // Read in all new lines
    for (std::string line; std::getline(task_file, line); )
      {
      total_chars_read += line.size() + 1; // Add one for the newline that gets eaten.
      task_lines.push_back(line);
      }

    for (ssize_t i = task_lines.size() - 1; i >= last_line; i--)
      parse_rur_line(task_lines[i], jobs_this_iteration);
    }
  } // END read_rur_stats_file()



void update_jobs_usage(

  mom_job *pjob)

  {
  std::string numeric_jobid(pjob->get_jobid());
  size_t      pos = numeric_jobid.find(".");
  if (pos != std::string::npos)
    numeric_jobid.erase(pos);

  std::map<std::string, task_usage_info>::iterator it = usage_information.find(numeric_jobid);
  if (it != usage_information.end())
    {
    resource_def *rd;
    resource     *pres;
      
    rd = find_resc_def(svr_resc_def, "cput", svr_resc_size);
    pres = add_resource_entry(pjob->get_attr(JOB_ATR_resc_used), rd);
    pres->rs_value.at_flags |= ATR_VFLAG_SET;
    pres->rs_value.at_type = ATR_TYPE_LONG;
    pres->rs_value.at_val.at_long = MAX(it->second.cput, pres->rs_value.at_val.at_long);

    rd = find_resc_def(svr_resc_def, "mem", svr_resc_size);
    pres = add_resource_entry(pjob->get_attr(JOB_ATR_resc_used), rd);
    pres->rs_value.at_flags |= ATR_VFLAG_SET;
    pres->rs_value.at_type = ATR_TYPE_SIZE;
    pres->rs_value.at_val.at_size.atsv_shift = 10;
    pres->rs_value.at_val.at_size.atsv_units = ATR_SV_BYTESZ;
    pres->rs_value.at_val.at_size.atsv_num = MAX(it->second.mem, pres->rs_value.at_val.at_size.atsv_num);
    }
  } // END update_jobs_usage()

