/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
*
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
*
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
*
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
*
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
*
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
*
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
*
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
*
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
*
* 7. DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/

#ifndef _PBS_IFL_DEF
#include "pbs_ifl.h"
#endif
#ifndef lONG_H
#include "pbs_constants.h" /* Long */
#endif
#include "list_link.h"
#include <string>

/*
 * This header file contains the definitions for attributes
 *
 * Other required header files:
 * "list_link.h"
 * "portability.h"
 *
 * Attributes are represented in one or both of two forms, external and
 * internal.  When an attribute is moving external to the server, either
 * to the network or to disk (for saving), it is represented in the
 * external form, a "svrattropl" structure.  This structure holds the
 * attribute name as a string.  If the attribute is a resource type, the
 * resource name and resource value are encoded as strings, a total of three
 * strings.  If the attribute is not a resource, then the attribute value is
 * coded into a string for a total of two strings.
 *
 * Internally, attributes exist in two separate structures.  The
 * attribute type is defined by a "definition structure" which contains
 * the name of the attribute, flags, and pointers to the functions used
 * to access the value.  This info is "hard coded".  There is one
 * "attribute definition" per (attribute name, parent object type) pair.
 *
 * The attribute value is contained in another struture which contains
 * the value and flags.  Both the attribute value and definition are in
 * arrays and share the same index.  When an
 * object is created, the attributes associated with that object
 * are created with default values.
 */

#include <sys/time.h>
#include <time.h>

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H 1

/* define the size of fields in the structures */

#define ATRFLAG 16
#define ATRTYPE  6  /* sync w/ATR_TYPE_*  (see #defines below) */
#define ATRPART  3

/*
 * The svrattrl structure is used to hold the external form of attributes.
 */

struct svrattrl
  {
  list_link      al_link;

  struct attropl al_atopl;    /* name,resource,value, see pbs_ifl.h   */
  int            al_tsize;    /* size of this structure (variable)    */
  int            al_nameln;   /* len of name string (including null)  */
  int            al_rescln;   /* len of resource name string (+ null) */
  int            al_valln;    /* len of value, may contain many nulls */

  unsigned int   al_flags:
  ATRFLAG;   /* copy of attribute value flags */

  /* data follows directly after */
  };

typedef struct svrattrl svrattrl;

#define al_name  al_atopl.name
#define al_resc  al_atopl.resource
#define al_value al_atopl.value
#define al_op  al_atopl.op



/*
 * The value of an attribute is contained in the following structure.
 *
 * The length field specifies the amount of memory that has been
 * malloc-ed for the value (used for at_str, at_array, and at_resrc).
 * If zero, no space has been malloc-ed.
 *
 * The union member is selected based on the value type given in the
 * flag field of the definition.
 */

enum cpu_frequency_type
  {
  P0 = 0,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,
  Performance,PowerSave,OnDemand,Conservative,UserSpace,
  AbsoluteFrequency,
  Invalid
  };


typedef struct cpu_frequency_value
  {
  unsigned int frequency_type;
  unsigned long mhz;
  } cpu_frequency_value;

struct size_value
  {
  unsigned long atsv_num; /* numeric part of a size value */

unsigned int  atsv_shift:
  8; /* binary shift count, K=10, g=20 */

unsigned int  atsv_units:
  1; /* units (size in words or bytes) */
  };

#define ATR_SV_BYTESZ 0   /* size is in bytes */
#define ATR_SV_WORDSZ 1   /* size is in words */

union attr_val    /* the attribute value */
  {
  long                  at_long;      /* long integer */
  Long                  at_ll;        /* largest long integer */
  char                  at_char;      /* single character */
  char                 *at_str;       /* char string (alloc) */

  struct array_strings *at_arst;      /* array of strings (alloc) */

  struct size_value     at_size;      /* size value */
  tlist_head            at_list;      /* list of resources,  ... (alloc) */

  struct  pbsnode      *at_jinfo;     /* ptr to node's job info  */
  short                 at_short;     /* short int; node's state */
  struct timeval        at_timeval;
  cpu_frequency_value   at_frequency;
  void                 *at_ptr;
  };


struct pbs_attribute
  {

unsigned int at_flags:
  ATRFLAG; /* attribute flags */

unsigned int at_type:
  ATRTYPE; /* type of attribute    */
  union  attr_val at_val;  /* the attribute value */
  };

typedef struct pbs_attribute pbs_attribute;

/*
 * The following structure is used to define an attribute for any parent
 * object.  The structure declares the attribute's name, value type, and
 * access methods.  This information is "built into" the server in an array
 * of attribute_def structures.  The definition occurs once for a given name.
 */

struct attribute_def
  {
  const char *at_name;
  int (*at_decode)(pbs_attribute *patr, const char *name, const char *rn, const char *val, int perm);
  int (*at_encode)(pbs_attribute *pattr, tlist_head *phead,
                      const char *aname, const char *rsname, int mode, int perm);
  int (*at_set)(pbs_attribute *pattr, pbs_attribute *new_attr, enum batch_op);
  int (*at_comp)(pbs_attribute *pattr, pbs_attribute *with);
  void (*at_free)(pbs_attribute *pattr);
  int (*at_action)(pbs_attribute *pattr, void *pobject, int actmode);

  unsigned int at_flags:
  ATRFLAG; /* flags: perms, ...  */

  unsigned int at_type:
  ATRTYPE; /* type of attribute  */

  unsigned int at_parent:
  ATRPART; /* type of parent object */
  };

typedef struct attribute_def attribute_def;

/* Defines for type of Attribute based on data type    */

#define ATR_TYPE_LONG          1 /* Long integer, also Boolean */
#define ATR_TYPE_CHAR          2 /* single character */
#define ATR_TYPE_STR           3 /* string, null terminated */
#define ATR_TYPE_ARST          4 /* Array of strings (char **) */
#define ATR_TYPE_SIZE          5 /* size (integer + suffix) */
#define ATR_TYPE_RESC          6 /* list type: resources only */
#define ATR_TYPE_LIST          7 /* list type:  dependencies, unkn, etc */
#define ATR_TYPE_ACL           8 /* Access Control Lists */
#define ATR_TYPE_LL            9 /* Long (64 bit) integer */
#define ATR_TYPE_SHORT         10 /* short integer    */
#define ATR_TYPE_JINFOP        13 /* struct jobinfo*  */
#define ATR_TYPE_TV            14 /* struct timeval */
#define ATR_TYPE_FREQ          15 /* CPU frequency */
#define ATR_TYPE_REQ           16 /* job req information */
#define ATR_TYPE_ATTR_REQ_INFO 17 /* queue and server req_information types */

/* Defines for  Flag field in attribute_def         */

#define ATR_DFLAG_USRD   0x01 /* User client can read (status) attribute */
#define ATR_DFLAG_USWR   0x02 /* User client can write (set)   attribute */
#define ATR_DFLAG_OPRD   0x04 /* Operator client can read   attribute */
#define ATR_DFLAG_OPWR   0x08 /* Operator client can write  attribute */
#define ATR_DFLAG_MGRD   0x10 /* Manager client can read  attribute */
#define ATR_DFLAG_MGWR   0x20 /* Manager client can write attribute */
#define ATR_DFLAG_PRIVR  0x40 /* Private (only owner, oper, manager can read) */
#define ATR_DFLAG_Creat  0x80 /* Can be set on create only */
#define ATR_DFLAG_SvRD   0x100 /* job attribute is sent to server on move */
#define ATR_DFLAG_SvWR   0x200 /* job attribute is settable by server/Sch */
#define ATR_DFLAG_MOM    0x400 /* attr/resc sent to MOM "iff" set    */

#define ATR_DFLAG_RDACC  0x515 /* Read access mask  */
#define ATR_DFLAG_WRACC  0x6AA /* Write access mask */
#define ATR_DFLAG_ACCESS 0x7ff /* Mask access flags */

#define ATR_DFLAG_ALTRUN 0x0800 /* (job) attr/resc is alterable in Run state */
#define ATR_DFLAG_NOSTAT 0x1000 /* attribute sent on status by request only  */
#define ATR_DFLAG_SELEQ  0x2000 /* attribute is only selectable eq/ne      */
#define ATR_DFLAG_RASSN  0x4000 /* resc to be summed in resources_used      */
#define ATR_DFLAG_RMOMIG 0x8000 /* resource to be ignored by mom      */

/* combination defines for permission field */

#define READ_ONLY    ATR_DFLAG_USRD | ATR_DFLAG_OPRD | ATR_DFLAG_MGRD
#define READ_WRITE   ATR_DFLAG_USRD | ATR_DFLAG_OPRD | ATR_DFLAG_MGRD | ATR_DFLAG_USWR | ATR_DFLAG_OPWR | ATR_DFLAG_MGWR
#define NO_USER_SET  ATR_DFLAG_USRD | ATR_DFLAG_OPRD | ATR_DFLAG_MGRD | ATR_DFLAG_OPWR | ATR_DFLAG_MGWR
#define MGR_ONLY_SET ATR_DFLAG_USRD | ATR_DFLAG_OPRD | ATR_DFLAG_MGRD | ATR_DFLAG_MGWR

/* Defines for Flag field in attribute (value)   */

#define ATR_VFLAG_SET    0x01 /* has specifed value (is set) */
#define ATR_VFLAG_MODIFY 0x02 /* value has been modified */
#define ATR_VFLAG_DEFLT  0x04 /* value is default value */
#define ATR_VFLAG_SEND   0x08 /* value to be sent to server */

#define ATR_TRUE  "True"
#define ATR_FALSE "False"

/* Defines for Parent Object type field in the attribute definition */
/* really only used for telling queue types apart   */

#define PARENT_TYPE_JOB      1
#define PARENT_TYPE_QUE_ALL  2
#define PARENT_TYPE_QUE_EXC  3
#define PARENT_TYPE_QUE_RTE  4
#define PARENT_TYPE_QUE_PULL 5
#define PARENT_TYPE_SERVER   6
#define PARENT_TYPE_NODE     7
/* DIAGTODO: define PARENT_TYPE_DIAG, but atrpart field isn't wide enough */

/*
 * values for the "actmode" parameter to at_action()
 */
#define ATR_ACTION_NOOP      0
#define ATR_ACTION_NEW       1
#define ATR_ACTION_ALTER     2
#define ATR_ACTION_RECOV     3
#define ATR_ACTION_FREE      4

/*
 * values for the mode parameter to at_encode(), determines:
 * - list separater character for encode_arst()
 * - which resources are encoded (see attr_fn_resc.c[encode_resc]
 */
#define ATR_ENCODE_CLIENT 0 /* encode for sending to client + sched */
#define ATR_ENCODE_SVR    1 /* encode for sending to another server */
#define ATR_ENCODE_MOM    2 /* encode for sending to MOM  */
#define ATR_ENCODE_SAVE   3 /* encode for saving to disk         */

/*
 * structure to hold array of pointers to character strings
 */

struct array_strings
  {
  int   as_npointers; /* number of pointer slots in this block */
  int   as_usedptr;   /* number of used pointer slots */
  int   as_bufsize;   /* size of buffer holding strings */
  char *as_buf;       /* address of buffer */
  char *as_next;      /* first available byte in buffer */
  char *as_string[1]; /* first string pointer */
  /* The space after this is often used to store additional pointers */
  /* Don't add additional members to the end of this struct */
  };

/*
 * specific attribute value function prototypes
 */

void clear_attr(pbs_attribute *pattr, attribute_def *pdef);
int  find_attr(attribute_def *attrdef, const char *name, int limit);
int  recov_attr(int fd, void *parent, attribute_def *padef,
                pbs_attribute *pattr, int limit, int unknown, int do_actions);
long attr_ifelse_long(pbs_attribute *, pbs_attribute *, long);
void free_null(pbs_attribute *attr);
void free_noop(pbs_attribute *attr);
svrattrl *attrlist_alloc(int szname, int szresc, int szval);
svrattrl *attrlist_create(const char *aname, const char *rname, int szval);
void free_attrlist(tlist_head *attrhead);
int  attr_atomic_set(svrattrl *plist, pbs_attribute *old,
                           pbs_attribute *new_attr, attribute_def *pdef, int limit,
                           int unkn, int privil, int *badattr);
int  attr_atomic_node_set(svrattrl *plist, pbs_attribute *old,
                                pbs_attribute *new_attr, attribute_def *pdef, int limit,
                                int unkn, int privil, int *badattr, bool dont_update_nodes_file);
void attr_atomic_kill(pbs_attribute *temp, attribute_def *pdef, int);

void overwrite_complete_req(pbs_attribute *attr, pbs_attribute *new_attr);

long time_str_to_seconds(const char *str);
int  decode_b(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_c(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_l(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_ll(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_size(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_str(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_time(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_arst(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_acl_arst(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_arst_direct(pbs_attribute *patr, const char *val);
int  decode_arst_merge(pbs_attribute *,const char *,const char *,const char *);
int  decode_resc(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_depend(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_hold(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_uacl(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_unkn(pbs_attribute *patr, const char *name, const char *rn, const char *val, int);
int  decode_tv(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int);
int  decode_nppcu(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int);
int  decode_frequency(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);
int  decode_complete_req(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);

int  decode_attr_req_info(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);
int decode_size(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm);


 
int get_time_string(char *time_string, int string_size, long timeval);
int encode_b(pbs_attribute *attr, tlist_head *phead, const char *atname,
                           const char *rsname, int mode, int perm);
int encode_c(pbs_attribute *attr, tlist_head *phead, const char *atname,
                           const char *rsname, int mode, int perm);
int encode_l(pbs_attribute *attr, tlist_head *phead, const char *atname,
                           const char *rsname, int mode, int perm);
int encode_ll(pbs_attribute *attr, tlist_head *phead, const char *atname,
                           const char *rsname, int mode, int perm);
int encode_size(pbs_attribute *attr, tlist_head *phead, const char *atname,
                              const char *rsname, int mode, int perm);
int encode_str(pbs_attribute *attr, tlist_head *phead, const char *atname,
                             const char *rsname, int mode, int perm);
int encode_time(pbs_attribute *attr, tlist_head *phead, const char *atname,
                             const char *rsname, int mode, int perm);
int encode_arst(pbs_attribute *attr, tlist_head *phead, const char *atname,
                             const char *rsname, int mode, int perm);
int encode_resc(pbs_attribute *attr, tlist_head *phead, const char *atname,
                             const char *rsname, int mode, int perm);
int encode_inter(pbs_attribute *attr, tlist_head *phead, const char *atname,
                              const char *rsname, int mode, int perm);
int encode_unkn(pbs_attribute *attr, tlist_head *phead, const char *atname,
                             const char *rsname, int mode, int perm);
int encode_depend(pbs_attribute *attr, tlist_head *phead, const char *atname,
                               const char *rsname, int mode, int perm);
int encode_hold(pbs_attribute *attr, tlist_head *phead, const char *atname,
                             const char *rsname, int mode, int perm);
int encode_tv(pbs_attribute *attr, tlist_head *phead, const char *atname,
                             const char *rsname, int mode, int perm);
int encode_frequency(pbs_attribute *attr, tlist_head *phead, const char *atname,
                            const char *rsname, int mode, int perm);
void from_frequency(struct cpu_frequency_value *, char *);
int encode_complete_req(pbs_attribute *attr, tlist_head *phead, const char *atname,
                            const char *rsname, int mode, int perm);
int encode_size(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm);




extern int set_b(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_c(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_l(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_ll(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_size(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_str(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_str_csv(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_arst(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_acl_arst(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_resc(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_hostacl(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_uacl(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_unkn(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_depend(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
extern int set_tv(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op);
extern int set_frequency(struct pbs_attribute *attr, struct pbs_attribute *newAttr, enum batch_op op);
extern int set_complete_req(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op op);
int  set_attr_req_info(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op op);
int set_size(struct pbs_attribute *attr, struct pbs_attribute *newAttr, enum batch_op op);

enum compare_types { LESS, EQUAL, GREATER, NOT_COMPARED };

int comp_b(pbs_attribute *, pbs_attribute *);
int comp_c(pbs_attribute *, pbs_attribute *);
int comp_l(pbs_attribute *, pbs_attribute *);
int comp_ll(pbs_attribute *, pbs_attribute *);
int comp_size(pbs_attribute *, pbs_attribute *);
int comp_str(pbs_attribute *, pbs_attribute *);
int comp_nodestr(pbs_attribute *, pbs_attribute *);
int comp_arst(pbs_attribute *, pbs_attribute *);
int comp_resc(pbs_attribute *, pbs_attribute *);
int comp_resc2(pbs_attribute *, pbs_attribute *, int, char *, enum compare_types);
int comp_unkn(pbs_attribute *, pbs_attribute *);
int comp_depend(pbs_attribute *, pbs_attribute *);
int comp_hold(pbs_attribute *, pbs_attribute *);
int comp_tv(struct pbs_attribute *attr, struct pbs_attribute *with);
int comp_frequency(struct pbs_attribute *attr, struct pbs_attribute *with);
int comp_complete_req(pbs_attribute *attr, pbs_attribute *with);
int comp_size(struct pbs_attribute *attr, struct pbs_attribute *with);


int action_depend(pbs_attribute *, void *, int);

void free_str(pbs_attribute *);
void free_arst(pbs_attribute *);
void free_resc(pbs_attribute *);
void free_depend(pbs_attribute *);
void free_unkn(pbs_attribute *);
void free_complete_req(pbs_attribute *);
void free_attr_req_info(pbs_attribute *patr);
int   parse_equal_string(char *, char **, char **);
char *parse_comma_string(char *,char **);

// More for size
int normalize_size(struct size_value *a, struct size_value *b, struct size_value *ta, struct size_value *tb);
int to_size(const char *val, struct size_value *psize);
void from_size(struct size_value *psize, char *cvnbuf);
void create_size_string(char *buf, struct size_value values);

#define NULL_FUNC '\0'

/* other associated funtions */

int   acl_check(pbs_attribute *, char *canidate, int type);
int   acl_check_my_array_string(struct array_strings *, char *, int);
char *arst_string(const char *str, pbs_attribute *pattr);
void  attrl_fixlink(tlist_head *svrattrl);
void  recov_acl(pbs_attribute *, attribute_def *, const char *, const char *);
int   save_acl(pbs_attribute *, attribute_def *,  const char *, const char *);
int   save_attr(attribute_def *, pbs_attribute *, int, int, char *, size_t *, size_t);
int   save_attr_xml(attribute_def *, pbs_attribute *, int, int);
int   write_buffer(char *,int,int);

int      encode_attr_req_info( pbs_attribute *, tlist_head *, const char *, const char *, int, int);
int      encode_state(pbs_attribute *, tlist_head *, const char *, const char *, int, int);
int      encode_power_state(pbs_attribute *, tlist_head *, const char *, const char *, int, int);
int      encode_props(pbs_attribute*, tlist_head*, const char*, const char*, int, int);
int      encode_jobs(pbs_attribute*, tlist_head*, const char*, const char*, int, int);
int      encode_ntype(pbs_attribute*, tlist_head*, const char*, const char*, int, int);
int      decode_state(pbs_attribute*, const char*, const char*, const char*, int);
int      decode_power_state(pbs_attribute*, const char*, const char*, const char*, int);
int      decode_utc(pbs_attribute*, const char*, const char*, const char*, int);
int      decode_props(pbs_attribute*, const char*, const char*, const char*, int);
int      decode_ntype(pbs_attribute*, const char*, const char*, const char*, int);
int      decode_null(pbs_attribute*, const char*, const char*, const char*, int);
int      comp_null(pbs_attribute*, pbs_attribute*);
int      count_substrings(char*, int*);
int      set_node_state(pbs_attribute*, pbs_attribute*, enum batch_op);
int      set_power_state(pbs_attribute*, pbs_attribute*, enum batch_op);
int      set_node_ntype(pbs_attribute*, pbs_attribute*, enum batch_op);
int      set_node_props(pbs_attribute*, pbs_attribute*, enum batch_op);
int      set_null(pbs_attribute*, pbs_attribute*, enum batch_op);
int      node_state(pbs_attribute*, void*, int);
int      node_power_state(pbs_attribute*, void*, int);
int      node_ttl(pbs_attribute*, void*, int);
int      node_acl(pbs_attribute*, void*, int);
int      node_requestid(pbs_attribute*, void*, int);
int      node_np_action(pbs_attribute*, void*, int);
int      node_mom_port_action(pbs_attribute*, void*, int);
int      node_mom_rm_port_action(pbs_attribute*, void*, int);
int      node_numa_action(pbs_attribute*, void*, int);
int      node_gpus_action(pbs_attribute*, void*, int);
int      node_mics_action(pbs_attribute*, void*, int);
int      gpu_str_action(pbs_attribute*, void*, int);
int      numa_str_action(pbs_attribute *, void *,int);
int      node_ntype(pbs_attribute*, void*, int);
int      node_prop_list(pbs_attribute*, void*, int);
int      node_status_list(pbs_attribute*, void*, int);
int      node_gpustatus_list(pbs_attribute*, void*, int);
int      node_micstatus_list(pbs_attribute *, void *, int);
int      node_note(pbs_attribute*, void*, int);
int      node_alt_name(pbs_attribute*, void*, int);
int      set_note_str(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
int      set_alt_name_str(pbs_attribute *attr, pbs_attribute *new_attr, enum batch_op);
void     replace_attr_string(pbs_attribute*, char*);
int      job_radix_action (pbs_attribute *new_attr, void *pobj, int actmode);
int      node_total_socket_action(pbs_attribute *new_attr, void *pobj, int actmode);
int      node_total_chip_action(pbs_attribute *new_attr, void *pobj, int actmode);
int      node_total_core_action(pbs_attribute *new_attr, void *pobj, int actmode);
int      node_total_thread_action(pbs_attribute *new_attr, void *pobj, int actmode);
int      node_available_socket_action(pbs_attribute *new_attr, void *pobj, int actmode);
int      node_available_chip_action(pbs_attribute *new_attr, void *pobj, int actmode);
int      node_available_core_action(pbs_attribute *new_attr, void *pobj, int actmode);
int      node_available_thread_action(pbs_attribute *new_attr, void *pobj, int actmode);

/* Token manipulation functions */

int  decode_tokens(pbs_attribute *, const char *, const char *, const char *, int);

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y);

int size_to_str(struct size_value szv, char *out, int space);
int attr_to_str(std::string &ds, attribute_def *at_def, pbs_attribute attr, bool XML);
int str_to_attr(const char *name, char *val, pbs_attribute *attr, struct attribute_def *padef, int limit);

/* "type" to pass to acl_check() */
#define ACL_Host      1
#define ACL_User      2
#define ACL_Group     3
#define ACL_Gid       4
#define ACL_User_Host 5

#endif  /* ATTRIBUTE_H */
