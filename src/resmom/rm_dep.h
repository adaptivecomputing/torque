#ifndef _RM_DEP_H
#define _RM_DEP_H
#include "license_pbs.h" /* See here for the software license */
/*
** Common resource names for dependent code.  All machines
** supported by the resource monitor should include at least
** these resources.
*/

char *cput(struct rm_attribute *attrib);
char *mem(struct rm_attribute *attrib);
char *sessions(struct rm_attribute *attrib);
char *pids(struct rm_attribute *attrib);
char *nsessions(struct rm_attribute *attrib);
char *nusers(struct rm_attribute *attrib);
char *size(struct rm_attribute *attrib);
char *idletime(struct rm_attribute *attrib);
char *nullproc(struct rm_attribute *attrib);

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
