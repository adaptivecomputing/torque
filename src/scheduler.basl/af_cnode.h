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
#ifndef _AF_CNODE_H
#define _AF_CNODE_H
/* Feature test switches */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif  /* _POSIX_SOURCE */

/* System headers */

/* Local Headers */
#include "af_resmom.h"

/* Macros */
/* Node States */
#define CNODE_UNKNOWN     -2
#define CNODE_OFFLINE     -1 /* Node is taken offline */
#define CNODE_DOWN     0
#define CNODE_FREE          1
#define CNODE_RESERVE       2
#define CNODE_INUSE_EXCLUSIVE 3
#define CNODE_INUSE_SHARED 4

/* Node Types */
#define CNODE_TIMESHARED    5
#define CNODE_CLUSTER      6

struct IODevice
  {

  struct IODevice *nextptr;
  char *name;   /* unique identity of the device */
  Size spaceTotal;  /* total space on the device */
  Size spaceAvail;  /* space available on the device */
  Size spaceReserved;  /* space reserved for jobs */
  int  inBw;   /* (bytes/sec) multiple meanings: */
  /* read bandwidth  or swap out rate */
  int  outBw;   /* (bytes/sec) write bandwidth or */
  /* swap in rate */
  };

struct Network
  {

  struct Network *nextptr;
  char   *type;   /* type of network - hippi, fddi, etc... */
  int    bw;    /* network bandwidth - in bytes/secs */
  };

struct Memory
  {

  struct Memory *nextptr;
  char   *type;   /* physMem (mainMem) or virtualMem */
  Size  total;   /* total memory */
  Size  avail;   /* available memory */
  };

struct cnode_struct
  {

  struct  cnode_struct *nextptr;
  ResMom  mom;    /* mom respresenting the node */
  char  *properties;   /* comma-separated list of node properties */
  int state;    /* node state */
  int type;    /* node type */
  char    *vendor;   /* system name */
  char *os;    /* string describing the OS version */
  /* this is always overwritten by arch query*/
  int numCpus;   /* number of processors */
  int idletime;   /* time since last keystroke/mouse movement*/
  int cpuPercentIdle;   /* % of idletime experienced by all processo*/
  int cpuPercentSys;   /* % of time that all processors have spent */
  /* running kernel code */
  int cpuPercentUser;   /* % of time that all processors have spent */
  /* running user code */
  int cpuPercentGuest;  /* % of time that all processors have spent */
  /* running a guest operating system */
  double  loadAve;   /* load average of all cpus in the node */

  struct Memory  *mem;   /* memory */

  struct  Network  *network; /* dynamic array of network devices */

  struct  IODevice *swap;   /* dynamic array of swap devices */

  struct  IODevice *disk;   /* dynamic array of disk devices */

  struct  IODevice *tape;   /* dynamic array of tape devices */

  struct  IODevice *srfs;   /* dynamic array of srfs devices */
  int queryMom;   /* if set, go ahead and query mom directly */
  int multiplicity;   /* during node requests, this is the # */
  /* of nodes of this type requested */
  };

typedef struct cnode_struct CNode;

struct SetCNode_type
  {
  CNode     *head;
  CNode     *tail;
  int   numAvail;
  int   numAlloc;
  int   numRsvd;
  int   numDown;
  };

typedef struct SetCNode_type SetCNode;

/* External Functions */

/* GET FUNCTIONS */
extern ResMom *
  CNodeResMomGet(CNode *node);

extern char *
  CNodeNameGet(CNode *node);

extern char *
  CNodePropertiesGet(CNode *node);

extern char *
  CNodeVendorGet(CNode *node);

extern char *
  CNodeOsGet(CNode *node);

extern int
  CNodeNumCpusGet(CNode *node);

extern Size
  CNodeMemTotalGet(CNode *node, char *type);

extern Size
  CNodeMemAvailGet(CNode *node, char *type);

extern int
  CNodeStateGet(CNode *node);

extern int
  CNodeTypeGet(CNode *node);

extern int
  CNodeIdletimeGet(CNode *node);

extern double
  CNodeLoadAveGet(CNode *node);

extern int
  CNodeQueryMomGet(CNode *node);

extern int
  CNodeMultiplicityGet(CNode *node);

extern int
  CNodeNetworkBwGet(CNode *node, char *type);

extern Size
  CNodeDiskSpaceTotalGet(CNode *node, char *name);

extern Size
  CNodeDiskSpaceAvailGet(CNode *node, char *name);

extern Size
  CNodeDiskSpaceReservedGet(CNode *node, char *name);

extern int
  CNodeDiskInBwGet(CNode *node, char *name);

extern int
  CNodeDiskOutBwGet(CNode *node, char *name);

extern Size
  CNodeSwapSpaceTotalGet(CNode *node, char *name);

extern Size
  CNodeSwapSpaceAvailGet(CNode *node, char *name);

extern Size
  CNodeSwapSpaceReservedGet(CNode *node, char *name);

extern int
  CNodeSwapInBwGet(CNode *node, char *name);

extern int
  CNodeSwapOutBwGet(CNode *node, char *name);

extern Size
  CNodeTapeSpaceTotalGet(CNode *node, char *name);

extern Size
  CNodeTapeSpaceAvailGet(CNode *node, char *name);

extern Size
  CNodeTapeSpaceReservedGet(CNode *node, char *name);

extern int
  CNodeTapeInBwGet(CNode *node, char *name);

extern int
  CNodeTapeOutBwGet(CNode *node, char *name);

extern Size
  CNodeSrfsSpaceTotalGet(CNode *node, char *name);

extern Size
  CNodeSrfsSpaceAvailGet(CNode *node, char *name);

extern Size
  CNodeSrfsSpaceReservedGet(CNode *node, char *name);

extern int
  CNodeSrfsInBwGet(CNode *node, char *name);

extern int
  CNodeSrfsOutBwGet(CNode *node, char *name);

extern int
  CNodeCpuPercentIdleGet(CNode *node);

extern int
  CNodeCpuPercentSysGet(CNode *node);

extern int
  CNodeCpuPercentUserGet(CNode *node);

extern int
  CNodeCpuPercentGuestGet(CNode *node);

/* PUT FUNCTIONS */
extern void
  CNodeResMomPut(CNode *node, ResMom *name);

extern void
  CNodePropertiesPut(CNode *node, char *properties);

extern void
  CNodeVendorPut(CNode *node, char *vendor);

extern void
  CNodeOsPut(CNode *node, char *os);

extern void
  CNodeNumCpusPut(CNode *node, int ncpus);

extern void
  CNodeMemTotalPut(CNode *node, char *type, Size pmem);

extern void
  CNodeMemAvailPut(CNode *node, char *type, Size pmem);

extern void
  CNodeQueryMomPut(CNode *node, int queryMom);

extern void
  CNodeMultiplicityPut(CNode *node, int nodect);

extern void
  CNodeStatePut(CNode *node, int state);

extern void
  CNodeTypePut(CNode *node, int type);

extern void
  CNodeIdletimePut(CNode *node, int idletime);

extern void
  CNodeLoadAvePut(CNode *node, double loadave);

extern void
  CNodeNetworkBwPut(CNode *node, char *type, int bw);

extern void
  CNodeDiskSpaceTotalPut(CNode *node, char *name, Size size);

extern void
  CNodeDiskSpaceAvailPut(CNode *node, char *name, Size size);

extern void
  CNodeDiskSpaceReservedPut(CNode *node, char *name, Size size);

extern void
  CNodeDiskInBwPut(CNode *node, char *name, int bw);

extern void
  CNodeDiskOutBwPut(CNode *node, char *name, int bw);

extern void
  CNodeSwapSpaceTotalPut(CNode *node, char *name, Size swaptot);

extern void
  CNodeSwapSpaceAvailPut(CNode *node, char *name, Size swapavail);

extern void
  CNodeSwapSpaceReservedPut(CNode *node, char *name, Size swapres);

extern void
  CNodeSwapInBwPut(CNode *node, char *name, int bw);

extern void
  CNodeSwapOutBwPut(CNode *node, char *name, int bw);

extern void
  CNodeTapeSpaceTotalPut(CNode *node, char *name, Size size);

extern void
  CNodeTapeSpaceAvailPut(CNode *node, char *name, Size size);

extern void
  CNodeTapeSpaceReservedPut(CNode *node, char *name, Size size);

extern void
  CNodeTapeInBwPut(CNode *node, char *name, int bw);

extern void
  CNodeTapeOutBwPut(CNode *node, char *name, int bw);

extern void
  CNodeSrfsSpaceTotalPut(CNode *node, char *name, Size size);

extern void
  CNodeSrfsSpaceAvailPut(CNode *node, char *name, Size size);

extern void
  CNodeSrfsSpaceReservedPut(CNode *node, char *name, Size size);

extern void
  CNodeSrfsInBwPut(CNode *node, char *name, int bw);

extern void
  CNodeSrfsOutBwPut(CNode *node, char *name, int bw);

extern void
  CNodeCpuPercentIdlePut(CNode *node, int percent);

extern void
  CNodeCpuPercentSysPut(CNode *node, int percent);

extern void
  CNodeCpuPercentUserPut(CNode *node, int percent);

extern void
  CNodeCpuPercentGuestPut(CNode *node, int percent);

/* MISC FUNCTIONS */
extern void
  CNodeFree(CNode *node);

extern void
  CNodeInit(CNode *node);

extern void
  CNodePrint(CNode *node);

extern void
  CNodeStateRead(CNode *node, int typeOfData);

/* Set stuff */
extern void
  SetCNodeInit(SetCNode *scn);

extern void
  SetCNodeAdd(SetCNode *scn, CNode *cn);

extern void
  SetCNodeFree(SetCNode *scn);

extern CNode *
  SetCNodeFindCNodeByName(SetCNode *scn, char *node_name);

extern void
  SetCNodePrint(SetCNode *scn);

extern int
  inSetCNode(CNode *cn, SetCNode *scn);

extern void
  CNodeStateRead(CNode *node, int typeOfData);

extern int
  SetCNodeSortInt(SetCNode *s, int (*key)(), int order);

extern int
  SetCNodeSortStr(SetCNode *s, char *(*key)(), int order);

extern int
  SetCNodeSortDateTime(SetCNode *s, DateTime(*key)(), int order);

extern int
  SetCNodeSortSize(SetCNode *s, Size(*key)(), int order);

extern int
  SetCNodeSortFloat(SetCNode *s, double(*key)(), int order);

#endif /* _AF_CNODE_H */
