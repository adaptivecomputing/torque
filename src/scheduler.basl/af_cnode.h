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
  CNodeResMomGet A_((CNode *node));

extern char *
  CNodeNameGet A_((CNode *node));

extern char *
  CNodePropertiesGet A_((CNode *node));

extern char *
  CNodeVendorGet A_((CNode *node));

extern char *
  CNodeOsGet A_((CNode *node));

extern int
  CNodeNumCpusGet A_((CNode *node));

extern Size
  CNodeMemTotalGet A_((CNode *node, char *type));

extern Size
  CNodeMemAvailGet A_((CNode *node, char *type));

extern int
  CNodeStateGet A_((CNode *node));

extern int
  CNodeTypeGet A_((CNode *node));

extern int
  CNodeIdletimeGet A_((CNode *node));

extern double
  CNodeLoadAveGet A_((CNode *node));

extern int
  CNodeQueryMomGet A_((CNode *node));

extern int
  CNodeMultiplicityGet A_((CNode *node));

extern int
  CNodeNetworkBwGet A_((CNode *node, char *type));

extern Size
  CNodeDiskSpaceTotalGet A_((CNode *node, char *name));

extern Size
  CNodeDiskSpaceAvailGet A_((CNode *node, char *name));

extern Size
  CNodeDiskSpaceReservedGet A_((CNode *node, char *name));

extern int
  CNodeDiskInBwGet A_((CNode *node, char *name));

extern int
  CNodeDiskOutBwGet A_((CNode *node, char *name));

extern Size
  CNodeSwapSpaceTotalGet A_((CNode *node, char *name));

extern Size
  CNodeSwapSpaceAvailGet A_((CNode *node, char *name));

extern Size
  CNodeSwapSpaceReservedGet A_((CNode *node, char *name));

extern int
  CNodeSwapInBwGet A_((CNode *node, char *name));

extern int
  CNodeSwapOutBwGet A_((CNode *node, char *name));

extern Size
  CNodeTapeSpaceTotalGet A_((CNode *node, char *name));

extern Size
  CNodeTapeSpaceAvailGet A_((CNode *node, char *name));

extern Size
  CNodeTapeSpaceReservedGet A_((CNode *node, char *name));

extern int
  CNodeTapeInBwGet A_((CNode *node, char *name));

extern int
  CNodeTapeOutBwGet A_((CNode *node, char *name));

extern Size
  CNodeSrfsSpaceTotalGet A_((CNode *node, char *name));

extern Size
  CNodeSrfsSpaceAvailGet A_((CNode *node, char *name));

extern Size
  CNodeSrfsSpaceReservedGet A_((CNode *node, char *name));

extern int
  CNodeSrfsInBwGet A_((CNode *node, char *name));

extern int
  CNodeSrfsOutBwGet A_((CNode *node, char *name));

extern int
  CNodeCpuPercentIdleGet A_((CNode *node));

extern int
  CNodeCpuPercentSysGet A_((CNode *node));

extern int
  CNodeCpuPercentUserGet A_((CNode *node));

extern int
  CNodeCpuPercentGuestGet A_((CNode *node));

/* PUT FUNCTIONS */
extern void
  CNodeResMomPut A_((CNode *node, ResMom *name));

extern void
  CNodePropertiesPut A_((CNode *node, char *properties));

extern void
  CNodeVendorPut A_((CNode *node, char *vendor));

extern void
  CNodeOsPut A_((CNode *node, char *os));

extern void
  CNodeNumCpusPut A_((CNode *node, int ncpus));

extern void
  CNodeMemTotalPut A_((CNode *node, char *type, Size pmem));

extern void
  CNodeMemAvailPut A_((CNode *node, char *type, Size pmem));

extern void
  CNodeQueryMomPut A_((CNode *node, int queryMom));

extern void
  CNodeMultiplicityPut A_((CNode *node, int nodect));

extern void
  CNodeStatePut A_((CNode *node, int state));

extern void
  CNodeTypePut A_((CNode *node, int type));

extern void
  CNodeIdletimePut A_((CNode *node, int idletime));

extern void
  CNodeLoadAvePut A_((CNode *node, double loadave));

extern void
  CNodeNetworkBwPut A_((CNode *node, char *type, int bw));

extern void
  CNodeDiskSpaceTotalPut A_((CNode *node, char *name, Size size));

extern void
  CNodeDiskSpaceAvailPut A_((CNode *node, char *name, Size size));

extern void
  CNodeDiskSpaceReservedPut A_((CNode *node, char *name, Size size));

extern void
  CNodeDiskInBwPut A_((CNode *node, char *name, int bw));

extern void
  CNodeDiskOutBwPut A_((CNode *node, char *name, int bw));

extern void
  CNodeSwapSpaceTotalPut A_((CNode *node, char *name, Size swaptot));

extern void
  CNodeSwapSpaceAvailPut A_((CNode *node, char *name, Size swapavail));

extern void
  CNodeSwapSpaceReservedPut A_((CNode *node, char *name, Size swapres));

extern void
  CNodeSwapInBwPut A_((CNode *node, char *name, int bw));

extern void
  CNodeSwapOutBwPut A_((CNode *node, char *name, int bw));

extern void
  CNodeTapeSpaceTotalPut A_((CNode *node, char *name, Size size));

extern void
  CNodeTapeSpaceAvailPut A_((CNode *node, char *name, Size size));

extern void
  CNodeTapeSpaceReservedPut A_((CNode *node, char *name, Size size));

extern void
  CNodeTapeInBwPut A_((CNode *node, char *name, int bw));

extern void
  CNodeTapeOutBwPut A_((CNode *node, char *name, int bw));

extern void
  CNodeSrfsSpaceTotalPut A_((CNode *node, char *name, Size size));

extern void
  CNodeSrfsSpaceAvailPut A_((CNode *node, char *name, Size size));

extern void
  CNodeSrfsSpaceReservedPut A_((CNode *node, char *name, Size size));

extern void
  CNodeSrfsInBwPut A_((CNode *node, char *name, int bw));

extern void
  CNodeSrfsOutBwPut A_((CNode *node, char *name, int bw));

extern void
  CNodeCpuPercentIdlePut A_((CNode *node, int percent));

extern void
  CNodeCpuPercentSysPut A_((CNode *node, int percent));

extern void
  CNodeCpuPercentUserPut A_((CNode *node, int percent));

extern void
  CNodeCpuPercentGuestPut A_((CNode *node, int percent));

/* MISC FUNCTIONS */
extern void
  CNodeFree A_((CNode *node));

extern void
  CNodeInit A_((CNode *node));

extern void
  CNodePrint A_((CNode *node));

extern void
  CNodeStateRead A_((CNode *node, int typeOfData));

/* Set stuff */
extern void
  SetCNodeInit A_((SetCNode *scn));

extern void
  SetCNodeAdd A_((SetCNode *scn, CNode *cn));

extern void
  SetCNodeFree A_((SetCNode *scn));

extern CNode *
  SetCNodeFindCNodeByName A_((SetCNode *scn, char *node_name));

extern void
  SetCNodePrint A_((SetCNode *scn));

extern int
  inSetCNode A_((CNode *cn, SetCNode *scn));

extern void
  CNodeStateRead A_((CNode *node, int typeOfData));

extern int
  SetCNodeSortInt A_((SetCNode *s, int (*key)(), int order));

extern int
  SetCNodeSortStr A_((SetCNode *s, char *(*key)(), int order));

extern int
  SetCNodeSortDateTime A_((SetCNode *s, DateTime(*key)(), int order));

extern int
  SetCNodeSortSize A_((SetCNode *s, Size(*key)(), int order));

extern int
  SetCNodeSortFloat A_((SetCNode *s, double(*key)(), int order));

#endif /* _AF_CNODE_H */
