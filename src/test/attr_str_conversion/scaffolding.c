#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>

#include "pbs_error.h"
#include "resource.h"
#include "attribute.h"
#include "attr_req_info.hpp"

int svr_resc_size = 0;
resource_def *svr_resc_def;

int get_parent_and_child(

  char  *start,       /* I */
  char **parent,      /* O */
  char **child,       /* O */
  char **end)         /* O */

  {
  return(PBSE_NONE);
  }

int unescape_xml(

  char *in,   /* I */ 
  char *out,  /* O */
  int   size) /* I */

  {
  return(PBSE_NONE);
  }

void log_err(

  int         errnum,  /* I (errno or PBSErrno) */
  const char *routine, /* I */
  const char *text)    /* I */

  {
  }

int attr_req_info::get_max_values(std::vector<std::string>& names, std::vector<std::string>& values)
  {
  return(0);
  }

int attr_req_info::get_min_values(std::vector<std::string>& names, std::vector<std::string>& values)
  {
  return(0);
  }

int attr_req_info::get_default_values(
    
  std::vector<std::string>& names,
  std::vector<std::string>& values)

  {
  return(0);
  }

int  decode_attr_req_info(
    
  pbs_attribute *patr,
  const char    *name,
  const char    *rescn,
  const char    *val,
  int            perm)

  {
  return(0);
  }

