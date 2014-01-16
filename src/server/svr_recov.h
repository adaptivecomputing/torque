#ifndef _SVR_RECOV_H
#define _SVR_RECOV_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* size_value, attribute_def, pbs_attribute */
#include "server.h" /* server */
#include <string>

int svr_recov(char *svrfile, int read_only);

int size_to_str(struct size_value szv, char *out, int space);

int attr_to_str(std::string& ds, attribute_def *at_def, struct pbs_attribute attr, int XML);

int svr_recov_xml(char *svrfile, int read_only);

int svr_save_xml(struct server *ps, int mode);

int svr_save(struct server *ps, int mode);

int save_acl(pbs_attribute *attr, attribute_def *pdef, const char *subdir, const char *name);

void recov_acl(pbs_attribute *pattr, attribute_def *pdef, const char *subdir, const char *name);

#endif /* _SVR_RECOV_H */
