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

#ifndef FAIR_SHARE_H
#define FAIR_SHARE_H

#include "data_types.h"
/*
 *      add_child - add a ginfo to the resource group tree
 */
void add_child(group_info *ginfo, group_info *parent);

/*
 *      find_group_info - recursive function to find a ginfo in the
     resgroup tree
 */
group_info *find_group_info(char *name, group_info *root);

/*
 *      find_alloc_ginfo - trys to find a ginfo in the fair share tree.  If it
 *                        can not find the ginfo, then allocate a new one and
 *                        add it to the "unknown" group
 */
group_info *find_alloc_ginfo(char *name);


/*
 *      new_group_info - allocate a new group_info struct and initalize it
 */
group_info * new_group_info();

/*
 *      parse_group - parse the resource group file
 */
int parse_group(char *fname);

/*
 *      preload_tree -  load the "root" group into the fair share tree
 *                      the "root" group is the entire machine.  Also load
 *                      the "unknown" group.  This group is for any user that
 *                      is not specified in the resource group file.
 */
int preload_tree();


/*
 *      count_shares - count the shares in the current resource group
 */
int count_shares(group_info *grp);

/*
 *      calc_fair_share_perc - walk the fair share group tree and calculate
 *                             the overall percentage of the machine a user/
 *                             group gets if all usage is equal
 */
int calc_fair_share_perc(group_info *root, int shares);

/*
 *      update_usage_on_run - update a users usage information when a
 *                            job is run
 */
void update_usage_on_run(job_info *jinfo);

/*
 *      calculate_usage_value - calcualte a value that represents the usage
 *                              information
 */
usage_t calculate_usage_value(resource_req *resreq);


/*
 *      decay_fairshare_tree - decay the usage information kept in the fair
 *                             share tree
 */
void decay_fairshare_tree(group_info *root);

/*
 *      extract_fairshare - extract the first job from the user with the
 *                          least percentage / usage ratio
 */
job_info *extract_fairshare(job_info **jobs);

/*
 *
 *      print_fairshare - print out the usage for all the users
 *
 *        root - root of subtree
 *
 *      returns nothing
 *
 */
void print_fairshare(group_info *root);

/*
 *      write_usage - write the usage information to the usage file
 *                    This fuction uses a recursive helper function
 */
int write_usage();

/*
 *      rec_write_usage - recursive helper function which will write out all
 *                        the group_info structs of the resgroup tree
 */
void rec_write_usage(group_info *root, FILE *fp);

/*
 *      read_usage - read the usage information and load it into the
 *                   resgroup tree.
 */
int read_usage();

/*
 *      free_group_tree - free the data used by a the fair share tree
 */
void free_group_tree(group_info *root);

#endif
