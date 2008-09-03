/* $Id: attrib.c,v 1.5 2006/09/07 15:23:14 ciesnik Exp $ */
/*
 *  DRMAA library for Torque/PBS
 *  Copyright (C) 2006  Poznan Supercomputing and Networking Center
 *                      DSP team <dsp-devel@hedera.man.poznan.pl>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
# include <pbs_config.h>
#endif

#include <string.h>
#include <attrib.h>

#ifndef lint
static char rcsid[]
# ifdef __GNUC__
__attribute__((unused))
# endif
= "$Id: attrib.c,v 1.5 2006/09/07 15:23:14 ciesnik Exp $";
#endif

struct drmaa_attrib
  {
  const char *name;
  int code;
  };

extern const struct drmaa_attrib *
        drmaa_attrib_lookup(const char *str, unsigned int len);

struct pbs_attrib
  {
  const char *name;
  int code;
  };

extern const struct pbs_attrib *
        pbs_attrib_lookup(const char *str, unsigned int len);

const drmaa_attrib_info_t *
attr_by_drmaa_name(const char *drmaa_name)
  {

  const struct drmaa_attrib *attr
        = drmaa_attrib_lookup(drmaa_name, strlen(drmaa_name));

  if (attr == NULL)
    return NULL;
  else
    return & drmaa_attr_table[ attr->code ];
  }

const drmaa_attrib_info_t *
attr_by_pbs_name(const char *pbs_name)
  {

  const struct pbs_attrib *attr
        = pbs_attrib_lookup(pbs_name, strlen(pbs_name));

  if (attr == NULL)
    return NULL;

  return & drmaa_attr_table[ attr->code ];
  }

#define OK ATTR_F_IMPL
#define no 0

const drmaa_attrib_info_t drmaa_attr_table[] =
  {
  /* DRMAA 1.0 attributes: */
    { ATTR_JOB_NAME,            "drmaa_job_name"             , "Job_Name"      , OK | ATTR_F_STR   },
  { ATTR_JOB_PATH,            "drmaa_remote_command"       , NULL            , OK | ATTR_F_PATH  },
  { ATTR_ARGV,                "drmaa_v_argv"               , NULL            , OK | ATTR_F_VECTOR | ATTR_F_STR  },
  { ATTR_ENV,                 "drmaa_v_env"                , "Variable_List" , OK | ATTR_F_VECTOR | ATTR_F_STR  },
  { ATTR_INPUT_PATH,          "drmaa_input_path"           , NULL            , OK | ATTR_F_PATH  },
  { ATTR_OUTPUT_PATH,         "drmaa_output_path"          , "Output_Path"   , OK | ATTR_F_PATH  },
  { ATTR_ERROR_PATH,          "drmaa_error_path"           , "Error_Path"    , OK | ATTR_F_PATH  },
  { ATTR_JOIN_FILES,          "drmaa_join_files"           , "Join_Path"     , OK | ATTR_F_BOOL  },
  { ATTR_TRANSFER_FILES,      "drmaa_transfer_files"       , NULL            , OK | ATTR_F_BOOL  },
  { ATTR_JOB_WORKING_DIR,     "drmaa_wd"                   , NULL            , OK | ATTR_F_PATH  },
  { ATTR_EMAIL,               "drmaa_v_email"              , "Mail_Users"    , OK | ATTR_F_VECTOR | ATTR_F_STR },
  { ATTR_BLOCK_EMAIL,         "drmaa_block_email"          , NULL            , OK | ATTR_F_BOOL  },
  { ATTR_START_TIME,          "drmaa_start_time"           , NULL            , no | ATTR_F_TIME  },
  { ATTR_JOB_SUBMIT_STATE,    "drmaa_js_state"             , "substate"      , OK | ATTR_F_STR   },
  { ATTR_HARD_CPU_TIME_LIMIT, "drmaa_duration_hlimit"      , NULL            , no | ATTR_F_TIMED },
  { ATTR_SOFT_CPU_TIME_LIMIT, "drmaa_duration_slimit"      , NULL            , no | ATTR_F_TIMED },
  { ATTR_HARD_WCT_LIMIT,      "drmaa_wct_hlimit"           , NULL            , no | ATTR_F_TIMED },
  { ATTR_SOFT_WCT_LIMIT,      "drmaa_wct_slimit"           , NULL            , no | ATTR_F_TIMED },
  { ATTR_DEADLINE_TIME,       "drmaa_deadline_time"        , NULL            , no | ATTR_F_TIME  },
  { ATTR_JOB_CATEGORY,        "drmaa_job_category"         , NULL            , no | ATTR_F_STR   },
  { ATTR_NATIVE,              "drmaa_native_specification" , NULL            , no | ATTR_F_STR   },
  /* Additional Torque/OpenPBS attribs (not in DRMAA specification): */
  /* Used by OpenPBS user commands: */
  { ATTR_CHECKPOINT,          NULL, "Checkpoint"       , ATTR_F_STR                               },
  { ATTR_GROUP_LIST,          NULL, "group_list"       , ATTR_F_VECTOR                            },
  { ATTR_HOLD_TYPES,          NULL, "Hold_Types"       , ATTR_F_STR                               },
  { ATTR_KEEP_FILES,          NULL, "Keep_Files"       , ATTR_F_BOOL                              },
  { ATTR_RESOURCES,           NULL, "Resource_List"    , ATTR_F_VECTOR | ATTR_F_STR               },
  { ATTR_MAIL_POINTS,         NULL, "Mail_Points"      , ATTR_F_STR                               },
  { ATTR_PRIORITY,            NULL, "Priority"         , ATTR_F_INT                               },
  { ATTR_DESTINATION_QUEUE,   NULL, "destination"      , ATTR_F_STR                               },
  { ATTR_RERUNABLE,           NULL, "Rerunable"        , ATTR_F_BOOL                              },
  { ATTR_USER_LIST,           NULL, "User_List"        , ATTR_F_VECTOR | ATTR_F_STR               },
  { ATTR_REMOTE_USER,         NULL, "Account_Name"     , ATTR_F_STR                               },
  { ATTR_SHELL,               NULL, "Shell_Path_List"  , ATTR_F_VECTOR | ATTR_F_STR | ATTR_F_PATH },
  { ATTR_DEPENDENCIES,        NULL, "depend"           , ATTR_F_VECTOR | ATTR_F_STR               },
  { ATTR_INTERACTIVE,         NULL, "interactive"      , ATTR_F_BOOL                              },
  { ATTR_STAGEIN,             NULL, "stagein"          , ATTR_F_VECTOR | ATTR_F_STR | ATTR_F_PATH },
  { ATTR_STAGEOUT,            NULL, "stageout"         , ATTR_F_VECTOR | ATTR_F_STR | ATTR_F_PATH },
  /* Additional job and general attribute names: */
  { ATTR_JOB_STATE,           NULL, "job_state"      , ATTR_F_STR                                 },
  { ATTR_CTIME,               NULL, "ctime"          , ATTR_F_STR                                 },
  { ATTR_ETIME,               NULL, "etime"          , ATTR_F_STR                                 },
  { ATTR_QTIME,               NULL, "qtime"          , ATTR_F_STR                                 },
  { ATTR_RESOURCES_USED,      NULL, "resources_used" , 0                                          },
  { ATTR_EXIT_STATUS,         NULL, "exit_status"    , ATTR_F_INT                                 },
  { ATTR_JOB_OWNER,           NULL, "Job_Owner"      , ATTR_F_STR                                 },
  { ATTR_EUSER,               NULL, "euser"          , 0                                          },
  { ATTR_EGROUP,              NULL, "egroup"         , 0                                          },
  { ATTR_HOP_COUNT,           NULL, "hop_count"      , 0                                          },
  { ATTR_SECURITY,            NULL, "security"       , 0                                          },
  { ATTR_SCHEDULER_HINT,      NULL, "sched_hint"     , 0                                          },
  { ATTR_SERVER,              NULL, "server"         , 0                                          },
  { ATTR_QUEUE,               NULL, "queue"          , 0                                          },
  { ATTR_QUEUE_RANK,          NULL, "queue_rank"     , 0                                          },
  { ATTR_EXECUTION_HOST,      NULL, "exec_host"      , 0                                          },
  { ATTR_TOTAL_JOBS,          NULL, "total_jobs"     , 0                                          },
  { ATTR_MAX_RUNNING,         NULL, "max_running"    , 0                                          },
  { ATTR_COMMENT,             NULL, "comment"        , 0                                          },
  { ATTR_JOB_COOKIE,          NULL, "cookie"         , 0                                          },
  { ATTR_HASHNAME,            NULL, "hashname"       , 0                                          },
  { ATTR_SESSION_ID,          NULL, "session_id"     , 0                                          },
  { ATTR_ALT_ID,              NULL, "alt_id"         , 0                                          },

  { -1, NULL, NULL, 0 } /* sentinel */
  };
