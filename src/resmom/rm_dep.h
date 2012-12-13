#ifndef _RM_DEP_H
#define _RM_DEP_H
#include "license_pbs.h" /* See here for the software license */
/*
** Common resource names for dependent code.  All machines
** supported by the resource monitor should include at least
** these resources.
*/

const char *cput(struct rm_attribute *attrib);
const char *mem(struct rm_attribute *attrib);
const char *sessions(struct rm_attribute *attrib);
const char *pids(struct rm_attribute *attrib);
const char *nsessions(struct rm_attribute *attrib);
const char *nusers(struct rm_attribute *attrib);
const char *size(struct rm_attribute *attrib);
const char *idletime(struct rm_attribute *attrib);
const char *nullproc(struct rm_attribute *attrib);

struct  config  standard_config[] =
  {
    { "cput", {cput}
    },

  { "mem", {mem} },
  { "sessions", {sessions} },
  { "pids", {pids} },
  { "nsessions", {nsessions} },
  { "nusers", {nusers} },
  { "size", {size} },
  { "idletime", {idletime} },
  { NULL,  {nullproc} },
  };

#endif /* _RM_DEP_H */
