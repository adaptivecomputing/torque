#include "license_pbs.h" /* See here for the software license */

#include "qmgr.h" /* objname */
#include "pbs_ifl.h" /* attropl */

/* main */

int attributes(char *attrs, struct attropl **attrlist, int doper);

struct attrl *attropl2attrl(struct attropl *from);

struct server *make_connection(char *name);

int connect_servers(struct objname *server_names, int numservers);

void blanks(int number);

int check_list(char *list);

void disconnect_from_server(struct server *svr);

void clean_up_and_exit(int exit_val);

void display(int otype, char *oname, struct batch_status *status, int format);

int set_active(int obj_type, struct objname *obj_names); 

int execute(int aopt, int oper, int type, char *names, struct attropl *attribs);

void freeattrl(struct attrl *attr);

void freeattropl(struct attropl *attr);

struct objname * commalist2objname(char *names, int type);

int get_request(char *request); 

int is_attr(int object, const char *name, int attr_type);

void show_help(char *str);

int parse(char *request, int *oper, int *type, char **names, struct attropl **attr);

void pstderr_big(const char *svrname, const char *objname, const char *errmesg);

void free_objname_list(struct objname *list);

struct server *find_server(char *name);

struct server * new_server(void);

void free_server(struct server *svr);

struct objname * new_objname(void);

void free_objname(struct objname *obj);

struct objname * strings2objname(char **str, int num, int type);

int is_valid_object(struct objname *obj, int type);

struct objname * default_server_name(void);

struct objname *temp_objname(char *obj_name, char *svr_name, struct server *svr);

void close_non_ref_servers(void);

int parse_request(char *request, char req[][MAX_REQ_WORD_LEN]);

