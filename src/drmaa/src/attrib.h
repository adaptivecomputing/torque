/* $Id: attrib.h,v 1.4 2006/06/05 23:27:17 ciesnik Exp $ */
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

#ifndef __DRMAA__ATTRIB_H
#define __DRMAA__ATTRIB_H

#ifdef HAVE_CONFIG_H
# include <pbs_config.h>
#endif

#include <sys/types.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#include <drmaa.h>

typedef struct drmaa_attrib_info_s drmaa_attrib_info_t;

enum
  {
  ATTR_F_VECTOR  = 1 << 0, /* bit 0: scalar/vector attribute */
  ATTR_F_BOOL    = 1 << 1, /* bits 1-2: type */
  ATTR_F_INT     = 2 << 1,
  ATTR_F_STR     = 3 << 1,
  ATTR_F_PATH    = 1 << 3, /* bits 3-4: (string) format */
  ATTR_F_TIME    = 2 << 3,
  ATTR_F_TIMED   = 3 << 3,
  ATTR_F_IMPL    = 1 << 5, /* bit 5: implemented flag */
  ATTR_F_TYPE_MASK   =  06,
  ATTR_F_FORMAT_MASK = 030
  };

struct drmaa_attrib_info_s
  {
  int         code;
  const char *drmaa_name;
  const char *pbs_name;
  unsigned    flags;
  };

/** @brief Attributes information table. */
extern const drmaa_attrib_info_t drmaa_attr_table[];

#if defined(__GNUC__) && 0
bool drmaa_is_vector(const drmaa_attrib_info_t *attr) __attribute__((weak));
bool drmaa_is_implemented(const drmaa_attrib_info_t *attr) __attribute__((weak));
bool
drmaa_is_vector(const drmaa_attrib_info_t *attr)
  {
  return attr->flags & ATTR_F_VECTOR;
  }

bool
drmaa_is_implemented(const drmaa_attrib_info_t *attr)
  {
  return attr->flags & ATTR_F_IMPL;
  }

#else
#define drmaa_is_vector( attr )         ( ((attr)->flags & ATTR_F_VECTOR) != 0 )
#define drmaa_is_implemented( attr )    ( ((attr)->flags & ATTR_F_IMPL) != 0 )
#endif /* __GNUC__ */

const drmaa_attrib_info_t *
attr_by_drmaa_name(const char *drmaa_name);

const drmaa_attrib_info_t *
attr_by_pbs_name(const char *pbs_name);


/**
 * Attributes codes.
 * Keep it synchronized with @ref drmaa_attr_table.
 */
typedef enum
  {

  /* DRMAA 1.0 attributes: */
  ATTR_JOB_NAME,
  ATTR_JOB_PATH,
  ATTR_ARGV,
  ATTR_ENV,
  ATTR_INPUT_PATH,
  ATTR_OUTPUT_PATH,
  ATTR_ERROR_PATH,
  ATTR_JOIN_FILES,
  ATTR_TRANSFER_FILES, /* optional */
  ATTR_JOB_WORKING_DIR,
  ATTR_EMAIL,
  ATTR_BLOCK_EMAIL,
  ATTR_START_TIME,
  ATTR_JOB_SUBMIT_STATE,
  ATTR_HARD_CPU_TIME_LIMIT, /* optional */
  ATTR_SOFT_CPU_TIME_LIMIT, /* optional */
  ATTR_HARD_WCT_LIMIT, /* optional */
  ATTR_SOFT_WCT_LIMIT, /* optional */
  ATTR_DEADLINE_TIME, /* optional */
  ATTR_JOB_CATEGORY,
  ATTR_NATIVE,

  /* Additional Torque/OpenPBS attribs (not in DRMAA specification): */
  /* Used by OpenPBS user commands: */
  ATTR_CHECKPOINT,
  ATTR_GROUP_LIST,
  ATTR_HOLD_TYPES,
  ATTR_KEEP_FILES,
  ATTR_RESOURCES,
  ATTR_MAIL_POINTS,
  ATTR_PRIORITY,
  ATTR_DESTINATION_QUEUE,
  ATTR_RERUNABLE,
  ATTR_USER_LIST,
  ATTR_REMOTE_USER,
  ATTR_SHELL,
  ATTR_DEPENDENCIES,
  ATTR_INTERACTIVE,
  ATTR_STAGEIN,
  ATTR_STAGEOUT,

  /* Additional job and general attribute names: */
  ATTR_JOB_STATE,
  ATTR_CTIME,
  ATTR_ETIME,
  ATTR_QTIME,
  ATTR_RESOURCES_USED,
  ATTR_EXIT_STATUS,
  ATTR_JOB_OWNER,
  ATTR_EUSER,
  ATTR_EGROUP,
  ATTR_HOP_COUNT,
  ATTR_SECURITY,
  ATTR_SCHEDULER_HINT,
  ATTR_SERVER,
  ATTR_QUEUE,
  ATTR_QUEUE_RANK,
  ATTR_EXECUTION_HOST,
  ATTR_TOTAL_JOBS,
  ATTR_MAX_RUNNING,
  ATTR_COMMENT,
  ATTR_JOB_COOKIE,
  ATTR_HASHNAME,
  ATTR_SESSION_ID,
  ATTR_ALT_ID,

  N_ATTRIBS,

  MIN_DRMAA_ATTR = ATTR_JOB_NAME,
  MAX_DRMAA_ATTR = ATTR_NATIVE,
  N_DRMAA_ATTRIBS = MAX_DRMAA_ATTR + 1
  } drmaa_attribute_t;

#endif /* __ATTRIB_H */

