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
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
* 
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
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

/*
 * @(#) $Id$
 */

/* $I$ */


/* symbolic constants */
#define ALL_SERVERS -1  /* all servers for connect_servers() */
/* server name used for the default PBS server ("") */
#define DEFAULT_SERVER "default"
/* server name used for all the active servers */
#define ACTIVE_SERVER "active"
/* max word length in the reqest */
#define MAX_REQ_WORD_LEN 32768  /* increased from 1024 */
 
/* there can be three words before the attribute list
 * command object name <attribute list> */
#define MAX_REQ_WORDS 3
#define IND_CMD         0
#define IND_OBJ         1
#define IND_NAME        2
#define IND_FIRST       IND_CMD
#define IND_LAST        IND_NAME

/* types of attributes: read only, public, all */
#define TYPE_ATTR_READONLY      1
#define TYPE_ATTR_PUBLIC        2
#define TYPE_ATTR_ALL           TYPE_ATTR_READONLY | TYPE_ATTR_PUBLIC




/* Macros */
 
/* This macro will determine if the char it is passed is a qmgr operator. */
#define Oper(x)         ( ( *(x) == '=') || \
                          ( *(x) == '+' && *( (x) + 1 ) == '=' ) || \
                          ( *(x) == '-' && *( (x) + 1 ) == '=' ) \
                        )
 
/* This macro will determine if the char it is passed is white space. */
#define White(x)        ( isspace( (int) (x) ) )
 
/* This macro will determine if the char is the end of a line. */
#define EOL(x)          (x == '\0')
 
/* This macro will allocate memory for a character string */
#define Mstring(x,y)    if ( (x=(char *)malloc(y)) == NULL ) { \
                            if( ! zopt ) fprintf(stderr,"qmgr: Out of memory\n"); \
                            clean_up_and_exit(5); \
                        }
/* This macro will allocate memory for some fixed size object */
#define Mstruct(x,y)    if ( (x=(y *)malloc(sizeof(y))) == NULL ) { \
                            if( ! zopt ) fprintf(stderr,"qmgr: Out of memory\n"); \
                            clean_up_and_exit(5); \
                        }
/* server name: "" is the default server and NULL is all active servers */
#define Svrname(x)      ( ( (x) == NULL ) ? ACTIVE_SERVER : \
                        ( ( strlen((x) -> s_name) ) ? \
                        (x) -> s_name : DEFAULT_SERVER) )
/* print an input line and then a caret under where the error has occured */
#define CaretErr(x, y)  if( ! zopt ) fprintf(stderr,"%s\n", (x)); blanks((y)); if( ! zopt ) fprintf(stderr,"^\n");

/* structures */

/* this struct is for the open servers */
struct server {
    char *s_name;               /* name of server */
    int s_connect;              /* PBS connection descriptor to server */
    int ref;                    /* number of references to server */
    struct server *next;        /* next server in list */
};
 
/* objname - name of an object with a possible server associated with it
 * i.e. batch@server1   -> queue batch at server server1
 */
struct objname
{
  int obj_type;                 /* type of object */
  char *obj_name;               /* name of object */
  char *svr_name;               /* name of server associated with object */
  struct server *svr;           /* short cut to server associated with object */
  struct objname *next;         /* next object in list */
};



/* prototypes */
struct objname *commalist2objname();
struct server *find_server();
struct server *make_connection();
struct server *new_server();
struct objname *new_objname();
struct objname *strings2objname( );
struct objname *default_server_name();
struct objname *temp_objname();
int parse_request( );
void clean_up_and_exit();
void freeattrl();
void freeattropl();
void pstderr_big();
void free_objname_list();
void free_server();
void free_objname();
void close_non_ref_servers();
int connect_servers();
int set_active();
int get_request();
int parse();
int execute();
int is_attr();
int is_valid_object();
void disconnect_from_server();


/* help messages */
/* some of these are split into multiple strings to keep the string within
 * the 509 character ISO C89 limit */

#define HELP_DEFAULT \
"General syntax: command [object][@server] [name attribute[.resource] OP value]\n" \
"To get help on any topic or subtopic, type help <topic>\n" \
"Help is available on all commands and topics.\n" \
"Available commands: active, create, delete, set, unset, list, print, quit\n" \
"Other topics are attributes, operators, names, and values .\n"

#define HELP_ACTIVE \
"Syntax active object [name [,name...]]\n" \
"Objects can be \"server\" \"queue\" or \"node\"\n" \
"The active command will set the active objects.  The active objects are used\n" \
"when no name is specified for different commands.\n" \
"If no server is specified for nodes or queues, the command will be sent\n" \
"to all active servers.\n"

#define HELP_ACTIVE2 \
"Examples:\n" \
"active queue q1,batch@server1\n" \
"active server server2,server3\n" \
"Now if the following command is typed:\n" \
"set queue max_running = 10\n" \
"The attribute max_running will be set to ten on the batch queue on server1\n" \
"and the q1 queue on server2 and server3.\n\n" \
"active server s1, s2\n" \
"active node @active\n" \
"This would specify all nodes at all servers.\n\n" \
"active queue @s2\n" \
"This would specify all queues at server s2\n"

#define HELP_CREATE \
"Syntax: create object name[,name...] \n" \
"Objects can be \"queue\" or \"node\"\n" \
"The create command will create the specified object on the PBS server(s).\n" \
"For multiple names, use a comma separated list with no intervening whitespace.\n" \
"Examples:\n" \
"create queue q1,q2,q3\n"

#define HELP_DELETE \
"Syntax: delete object name[,name...]\n" \
"Objects can be \"queue\" or \"node\"\n" \
"The delete command will delete the specified object from the PBS server(s)." \
"\nExamples:\n" \
"delete queue q1\n"

#define HELP_SET \
"Syntax: set object [name,][,name...] attribute[.resource] OP value\n" \
"Objects can be \"server\" or \"queue\", \"node\"\n" \
"The \"set\" command sets the value for an attribute on the specified object.\n" \
"If the object is \"server\" and name is not specified, the attribute will be\n" \
"set on all the servers specified on the command line.\n" \
"For multiple names, use a comma separated list with no intervening whitespace.\n"

#define HELP_SET2 \
"Examples:\n" \
"set server s1 max_running = 5\n" \
"set server managers = root\n" \
"set server managers += susan\n" \
"set node n1,n2 state=down\n" \
"set queue q1@s3 resources_max.mem += 5mb\n" \
"set queue @s3 default_queue = batch\n"

#define HELP_UNSET \
"Syntax: unset object [name][,name...]\n" \
"Objects can be \"server\" or \"queue\", \"node\"\n" \
"The unset command will unset an attribute on the specified object.\n" \
"If the object is \"server\" and name is not specified, the attribute will be\n" \
"unset on all the servers specified on the command line.\n" \
"For multiple names, use a comma separated list with no intervening whitespace.\n" \
"Examples:\n" \
"unset server s1 max_running\n" \
"unset server managers\n" \
"unset queue enabled\n"

#define HELP_LIST \
"Syntax: list object [name][,name...]\n" \
"Object can be \"server\" or \"queue\" or \"node\"\n" \
"The list command will list out all the attributes for the specified object.\n" \
"If the object is \"server\" and name is not specified, all the servers\n" \
"specified on the command line will be listed.\n" \
"For multiple names, use a comma separated list with no intervening whitespace.\n" \
"Examples:\n" \
"list server\n" \
"list queue q1\n" \
"list node n1,n2,n3\n"

#define HELP_PRINT \
"Syntax: print object [name][,...]\n" \
"Object can be \"server\", or \"queue\", or \"node\"\n" \
"The print command's output can be fed back into qmgr as input.\n" \
"If the object is \"server\", all the queues and nodes associated \n" \
"with the server are printed as well as the server information.\n" \
"For multiple names, use a comma separated list with no intervening whitespace.\n" \
"Examples:\n" \
"print server\n" \
"print node n1\n" \
"print queue q3\n" \

#define HELP_QUIT \
"Syntax: quit\n" \
"The quit command will exit from qmgr.\n"

#define HELP_EXIT \
"Syntax: exit\n" \
"The exit command will exit from qmgr.\n"

#define HELP_OPERATOR \
"Syntax: ... attribute OP new value\n" \
"Qmgr accepts three different operators for its commands.\n" \
"\t=\tAssign value into attribute.\n" \
"\t+=\tAdd new value and old value together and assign into attribute.\n" \
"\t-=\tSubtract new value from old value and assign into attribute.\n" \
"These operators are used in the \"set\" and the \"unset\" commands\n"

#define HELP_VALUE \
"Syntax ... OP value[multiplier]\n" \
"A multipler can be added to the end of a size in bytes or words.\n" \
"The multipliers are: tb, gb, mb, kb, b, tw, gw, mw, kw, w.  The second letter\n" \
"stands for bytes or words.  b or w is the default multiplier.\n" \
"The multipliers are case insensitive i.e. gw is the same as GW.\n" \
"Examples:\n" \
"100mb\n" \
"2gw\n" \
"10\n"

#define HELP_NAME \
"Syntax: [name][@server]\n" \
"Names can be in several parts.  There can be the name of an object, \n" \
"the name of an object at a server, or just at a server.\n" \
"The name of an object specifys a name.  A name of an object at a server\n" \
"specifys the name of an object at a specific server.  Lastly, at a server\n" \
"specifys all objects of a type at a server\n" \
"Examples:\n" \
"batch     - An object called batch\n" \
"batch@s1  - An object called batch at the server s1\n" \
"@s1       - All the objects of a cirtain type at the server s1\n"

#define HELP_ATTRIBUTE \
"The help for attributes are broken up into the following help subtopics:\n" \
"\tserverpublic\t- Public server attributes\n" \
"\tserverro\t- Read only server attributes\n" \
"\tqueuepublic\t- Public queue attributes\n" \
"\tqueueexec\t- Attributes specific to execution queues\n" \
"\tqueueroute\t- Attributes specified to routing queues\n" \
"\tqueuero \t- Read only queue attributes\n" \
"\tnodeattr\t- Node Attributes\n"

#define HELP_SERVERPUBLIC \
"Server Public Attributes:\n" \
"acl_host_enable - enables host level access control\n" \
"acl_user_enable - enables user level access control\n" \
"acl_users - list of users allowed/denied access to server\n" \
"comment - informational text string about the server\n" \
"default_queue - default queue used when a queue is not specified\n" \
"log_events - a bit string which specfiies what is logged\n"

#define HELP_SERVERPUBLIC2 \
"mail_uid - uid of sender of mail which is sent by the server\n" \
"managers - list of users granted administrator privledges\n" \
"max_running - maximum number of jobs that can run on the server\n" \
"max_user_run - maximum number of jobs that a user can run on the server\n" \
"max_grou_run - maximum number of jobs a UNIX group can run on the server\n" \
"operators - list of users granted operator privledges\n" \
"query_other_jobs - when true users can query jobs owned by other users\n"

#define HELP_SERVERPUBLIC3 \
"resources_available - amount of resources which are available to the server\n" \
"resources_cost - the cost factors of resources.  Used for sync. job starting\n" \
"resources_default - the default resource value when the job does not specify\n" \
"resource_max - the maximum amount of resources that are on the system\n" \
"scheduler_iteration - the amount of seconds between timed scheduler iterations\n" \
"scheduling - when true the server should tell the scheduler to run\n" \
"system_cost - arbitirary value factored into resource costs\n" \

#define HELP_SERVERRO \
"Server Read Only Attributes:\n" \
"resources_assigned - total amount of resources allocated to running jobs\n" \
"server_name - the name of the server and possibly a port number\n" \
"server_state - the current state of the server\n" \
"state_count - total number of jobs in each state\n" \
"total_jobs - total number of jobs managed by the server\n" \
"PBS_version - the release version of PBS\n" \

#define HELP_QUEUEPUBLIC \
"Queue Public Attributes:\n" \
"acl_group_enable - enables group level access control on the queue\n" \
"acl_groups - list of groups which have been allowed or denied access\n" \
"acl_host_enable - enables host level access control on the queue\n" \
"acl_hosts - list of hosts which have been allowed or denied access\n" \
"acl_user_enable - enables user level access control on the queue\n"

#define HELP_QUEUEPUBLIC2 \
"acl_users - list of users which have been allowed or denied access\n" \
"enabled - when true users can enqueue jobs\n" \
"from_route_only - when true queue only accepts jobs when routed by servers\n" \
"max_queueable - maximum number of jobs allowed to reside in the queue\n" \
"max_running - maximum number of jobs in the queue that can be routed or running\n" \
"priority - the priority of the queue\n"

#define HELP_QUEUEPUBLIC3 \
"queue_type - type of queue: execution or routing\n" \
"resources_max - maximum amount of a resource which can be requested by a job\n" \
"resources_min - minimum amount of a resource which can be requested by a job\n" \
"resources_default - the default resource value when the job does not specify\n" \
"started - when true jobs can be scheduled for execution\n"

#define HELP_QUEUEEXEC \
"Attributes for Execution queues only:\n" \
"checkpoint_min - min. number of mins. of CPU time allowed bwtween checkpointing\n" \
"resources_available - amount of resources which are available to the queue\n" \
"kill_delay - amount of time between SIGTERM and SIGKILL when deleting a job\n" \
"max_user_run - maximum number of jobs a user can run in the queue\n" \
"max_group_run - maximum number of jobs a UNIX group can run in a queue\n"

#define HELP_QUEUEROUTE \
"Attributes for Routing queues only:\n" \
"route_destinations - list of destinations which jobs may be routed to\n" \
"alt_router - when true a alternate routing function is used to route jobs\n" \
"route_held_jobs - when true held jobs may be routed from this queue\n" \
"route_waiting_jobs - when true waiting jobs may be routed from this queue\n" \
"route_retry_time - time delay between route retries.\n" \
"route_lifetime - maximum amount of time a job can be in this routing queue\n"

#define HELP_QUEUERO \
"Queue read only attributes:\n" \
"total_jobs - total number of jobs in queue\n" \
"state_count - total number of jobs in each state in the queue\n" \
"resources_assigned - amount of resources allocated to jobs running in queue\n"

#define HELP_NODEATTR \
"Node attributes:\n" \
"state - the current state of a node\n" \
"properties - the properties the node has\n"

