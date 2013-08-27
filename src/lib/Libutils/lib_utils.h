#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <netdb.h> /* hostent */
#include <sys/socket.h> /* sockaddr */


#include "mcom.h" /* mxml_t, MDataFormatEnum, mbool_t */
#include "dynamic_string.h" /* dynamic_string */
#include "hash_table.h" /* hash_table_t, bucket, node_comm_t */
#include "mom_hierarchy.h" /* mom_hierarchy_t */
#include "resizable_array.h" /* resizable_array */
#include "threadpool.h" /* threadpool_t */
#include "u_tree.h" /* NodeEntry, AvlTree */

/* u_MXML.c */
int MXMLExtractE(mxml_t *E, mxml_t *C, mxml_t **CP);

int MXMLSetChild(mxml_t *E, char *CName, mxml_t **CE);

int MXMLCreateE(mxml_t **E, const char *Name);
 
int MXMLDestroyE(mxml_t **EP);
 
int MXMLSetAttr(mxml_t *E, char *A, void *V, enum MDataFormatEnum Format);

int MXMLAppendAttr(mxml_t *E, char *AName, char *AVal, char Delim);

int MXMLSetVal(mxml_t *E, void *V, enum MDataFormatEnum Format);

int MXMLAddE(mxml_t *E, mxml_t *C);

int MXMLToXString(mxml_t *E, char **Buf, int *BufSize, int MaxBufSize, char **Tail, mbool_t IsRootElement);

int MXMLToString(mxml_t *E, char *Buf, int BufSize, char **Tail, mbool_t IsRootElement);

int MXMLGetAttrF(mxml_t *E, char *AName, int *ATok, void *AVal, enum MDataFormatEnum DFormat, int VSize);

int MXMLGetAttr(mxml_t *E, char *AName, int *ATok, char *AVal, int VSize);

int MXMLGetChild(mxml_t *E, char *CName, int *CTok, mxml_t **C);
int MXMLGetChildCI(mxml_t *E, char *CName, int *CTok, mxml_t **CP);
int        MXMLFromString(mxml_t **EP, char *XMLString, char **Tail, char *EMsg, int emsg_size);

/* u_dynamic_string.c */
size_t need_to_grow(dynamic_string *ds, char *to_check);

int append_dynamic_string(dynamic_string *ds, const char *to_append);

int copy_to_end_of_dynamic_string(dynamic_string *ds, const char *to_copy);

dynamic_string *get_dynamic_string(int initial_size, const char *str);

void free_dynamic_string(dynamic_string *ds);

void clear_dynamic_string(dynamic_string *ds);

char *get_string(dynamic_string *ds);

/* u_groups.c */
struct group * getgrnam_ext(char * grp_name );

/* src/include/u_hash_map_structs.h */

/* u_mom_hierarchy.c */
mom_hierarchy_t *initialize_mom_hierarchy(void);

int add_network_entry(mom_hierarchy_t *nt, char *name, struct addrinfo *ai, unsigned short rm_port, int path, int level);

int rm_establish_connection(node_comm_t *nc);

node_comm_t *force_path_update(mom_hierarchy_t *nt);

node_comm_t *update_current_path(mom_hierarchy_t *nt);

int write_tcp_reply(struct tcp_chan *chan, int protocol, int version, int command, int exit_code);

int read_tcp_reply(struct tcp_chan *chan, int protocol, int version, int command, int *exit_status);

/* u_mu.c */
int is_whitespace(char c);
 
int MUSNPrintF(char **BPtr, int *BSpace, const char *Format, ...);

int MUStrNCat(char **BPtr, int *BSpace, const char *Src);

int MUSleep(long SleepDuration);

int MUReadPipe(char *Command, char *Buffer, int BufSize);

int write_buffer(char *buf, int len, int fds);

int swap_things(resizable_array *ra, void *thing1, void *thing2);

int check_and_resize(resizable_array *ra);

void update_next_slot(resizable_array *ra);

int insert_thing(resizable_array *ra, void *thing);

int insert_thing_after(resizable_array *ra, void *thing, int index);

int insert_thing_before(resizable_array *ra, void *thing, int index);

int is_present(resizable_array *ra, void *thing);

void unlink_slot(resizable_array *ra, int index);

int remove_thing(resizable_array *ra, void *thing);

void *pop_thing(resizable_array *ra);

int remove_thing_from_index(resizable_array *ra, int index);

resizable_array *initialize_resizable_array(int size);

void *next_thing(resizable_array *ra, int *iter);

void *next_thing_from_back(resizable_array *ra, int *iter);

void initialize_ra_iterator(resizable_array *ra, int *iter);

int get_index(resizable_array *ra, void *thing);

void *get_thing_from_index(resizable_array *ra, int index);

/* u_threadpool.c */
int create_work_thread(void);

/* static void work_thread_cleanup(void *a); */
 
void work_cleanup(void *a);

/* static void *work_thread(void *a); */

int initialize_threadpool(threadpool_t **pool, int min_threads, int max_threads, int max_idle_time);

int enqueue_threadpool_request(void *(*func)(void *), void *arg);

void destroy_request_pool(void);

void start_request_pool();

/* u_tree.c */
/* static int height(NodeEntry node); */

/* static int Max(int right_side, int left_side ); */


/* static NodeEntry single_rotate_with_left(NodeEntry K2 ); */

/* static NodeEntry single_rotate_with_right(NodeEntry K1 ); */

/* static NodeEntry double_rotate_with_left(NodeEntry K3 ); */

/* static NodeEntry double_rotate_with_right(NodeEntry K1 ); */

AvlTree AVL_insert(u_long key, uint16_t port, struct pbsnode *node, AvlTree tree );

struct pbsnode *AVL_find(u_long key, uint16_t port, AvlTree tree);

int AVL_is_in_tree(u_long key, uint16_t port, AvlTree tree);

int AVL_is_in_tree_no_port_compare(u_long key, uint16_t port, AvlTree tree);

uint16_t AVL_get_port_by_ipaddr(u_long key, AvlTree tree);

AvlTree AVL_delete_node(u_long key, uint16_t port, AvlTree tree);

int AVL_list_add_item(AvlTree tree, char **Buf, long *current_len, long *max_len);
int AVL_list( AvlTree tree, char **Buf, long *current_len, long *max_len );

/* u_users.c */
struct passwd * getpwnam_ext(char * user_name );

/* u_xml.c */
int get_parent_and_child(char *start, char **parent, char **child, char **end);

int escape_xml(char *in, char *out, int size);

char *find_next_tag(char *buffer, char **tag);

int unescape_xml(char *in, char *out, int size);
