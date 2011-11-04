#ifndef _SVR_RECOV_H
#define _SVR_RECOV_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* size_value, attribute_def, attribute */
#include "server.h" /* server */

int svr_recov(char *svrfile, int read_only);

int size_to_str(struct size_value szv, char *out, int space);

int attr_to_str(struct dynamic_string *ds, attribute_def *at_def, struct attribute attr, int XML);

int str_to_attr(char *name, char *val, struct attribute *attr, struct attribute_def *padef);

int svr_recov_xml(char *svrfile, int read_only);

int svr_save_xml(struct server *ps, int mode);

int svr_save(struct server *ps, int mode);

int save_acl(attribute *attr, attribute_def *pdef, char *subdir, char *name);

void recov_acl(attribute *pattr, attribute_def *pdef, char *subdir, char *name);

#endif /* _SVR_RECOV_H */
