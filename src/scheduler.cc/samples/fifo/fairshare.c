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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "job_info.h"
#include "constant.h"
#include "fairshare.h"
#include "globals.h"
#include "misc.h"
#include "constant.h"
#include "config.h"


/*
 *
 * add_child - add a group_info to the resource group tree
 *
 *   ginfo - ginfo to add to the tree
 *   parent - parent ginfo
 *
 * returns nothing
 *
 */
void add_child(group_info *ginfo, group_info *parent)
  {
  if (parent != NULL)
    {
    ginfo -> sibling = parent -> child;
    parent -> child = ginfo;
    ginfo -> parent = parent;
    ginfo -> resgroup = parent -> cresgroup;
    }
  }

/*
 *
 * add_unknown - add a ginfo to the "unknown" group
 *
 *   ginfo - ginfo to add
 *
 * returns nothing
 *
 */
void add_unknown(group_info *ginfo)
  {
  group_info *unknown;  /* ptr to the "unknown" group */

  unknown = find_group_info("unknown", conf.group_root);
  add_child(ginfo, unknown);
  calc_fair_share_perc(unknown -> child, UNSPECIFIED);
  }

/*
 *
 * find_group_info - recursive function to find a group_info in the
 *     resgroup tree
 *
 *   name - name of the ginfo to find
 *   root - the root of the current sub-tree
 *
 * returns the found group_info or NULL
 *
 */
group_info *find_group_info(char *name, group_info *root)
  {
  group_info *ginfo;  /* the found group */

  if (root == NULL || !strcmp(name, root -> name))
    return root;

  ginfo = find_group_info(name, root -> sibling);

  if (ginfo == NULL)
    ginfo = find_group_info(name, root -> child);

  return ginfo;
  }

/*
 *
 * find_alloc_ginfo - trys to find a ginfo in the fair share tree.  If it
 *     can not find the ginfo, then allocate a new one and
 *     add it to the "unknown" group
 *
 *   name - name of the ginfo to find
 *
 * returns the found ginfo or the newly allocated ginfo
 *
 */
group_info *find_alloc_ginfo(char *name)
  {
  group_info *ginfo;  /* the found group or allocated group */

  ginfo = find_group_info(name, conf.group_root);

  if (ginfo == NULL)
    {
    if ((ginfo = new_group_info()) == NULL)
      return NULL;

    ginfo-> name = string_dup(name);

    ginfo -> shares = 1;

    add_unknown(ginfo);
    }

  return ginfo;
  }

/*
 *
 * new_group_info - allocate a new group_info struct and initalize it
 *
 * returns a ptr to the new group_info
 *
 */
group_info *new_group_info()
  {
  group_info *new;  /* the new group */

  if ((new = malloc(sizeof(group_info))) == NULL)
    {
    perror("Error allocating memory");
    return NULL ;
    }

  new -> name = NULL;

  new -> resgroup = UNSPECIFIED;
  new -> cresgroup = UNSPECIFIED;
  new -> shares = UNSPECIFIED;
  new -> percentage = 0.0;
  new -> usage = 1;
  new -> temp_usage = 1;
  new -> parent = NULL;
  new -> sibling = NULL;
  new -> child = NULL;

  return new;
  }

/*
 *
 * parse_group - parse the resource group file
 *
 *   fname - name of the file
 *
 * returns success/failure
 *
 *
 * FORMAT:   name cresgrp  grpname  shares
 *
 *   name    - name of user/grp
 *   cresgrp - resource group of the children of this group (if group)
 *   grpname - resource group of this user/group
 *   shares  - the amount of shares the user/group has in its resgroup
 *
 */
int parse_group(char *fname)
  {
  group_info *ginfo;  /* ptr to parent group */
  group_info *new_ginfo; /* used to add each new group */
  char buf[256];  /* used to read each line from the file */
  char *nametok;  /* strtok: name of new group */
  char *grouptok;  /* strtok: parent group name */
  char *cgrouptok;  /* strtok: resgrp of the children of newgrp */
  char *sharestok;  /* strtok: the amount of shares for newgrp */
  FILE *fp;   /* file pointer to the resource group file */
  char error = 0;  /* boolean: is there an error ? */
  int shares;   /* number of shares for the new group */
  int cgroup;   /* resource group of the children of the grp */
  char *endp;   /* used for strtol() */
  int linenum = 0;  /* current line number in the file */

  if ((fp = fopen(fname, "r")) == NULL)
    {
    perror("Warning: resource group file error, fair share will not work");
    return 0;
    }

  while (fgets(buf, 256, fp) != NULL)
    {
    if (buf[strlen(buf)-1] == '\n')
      buf[strlen(buf)-1] = '\0';

    linenum++;

    if (!skip_line(buf))
      {
      nametok = strtok(buf, " \t");
      cgrouptok = strtok(NULL, " \t");
      grouptok = strtok(NULL, " \t");
      sharestok= strtok(NULL,  " \t");

      ginfo = find_alloc_ginfo(grouptok);

      if (ginfo != NULL)
        {
        shares = strtol(sharestok, &endp, 10);

        if (*endp == '\0')
          {
          cgroup = strtol(cgrouptok, &endp, 10);

          if (*endp == '\0')
            {
            if ((new_ginfo = new_group_info()) == NULL)
              return 0;

            new_ginfo -> name = string_dup(nametok);

            new_ginfo -> resgroup = ginfo -> cresgroup;

            new_ginfo -> cresgroup = cgroup;

            new_ginfo -> shares = shares;

            add_child(new_ginfo, ginfo);
            }
          else
            error = 1;
          }
        else
          error = 1;
        }
      else
        {
        error = 1;
        fprintf(stderr, "Parent ginfo of %s doesnt exist.\n", nametok);
        }

      if (error)
        fprintf(stderr, "resgroup: error on line %d.\n", linenum);

      error = 0;
      }
    }

  return 1;
  }

/*
 *
 * free_group_tree - free the data used by a the fair share tree
 *
 *   root - root of subtree
 *
 */
void free_group_tree(group_info *root)
  {
  if (root == NULL)
    return;

  free_group_tree(root -> sibling);

  free_group_tree(root -> child);

  free(root);
  }

/*
 *
 * preload_tree -  load the "root" group into the fair share tree
 *   the "root" group is the entire machine.  Also load
 *   the "unknown" group.  This group is for any user that
 *   is not specified in the resource group file.
 *
 * returns success/failure
 *
 */
int
preload_tree(void)
  {
  group_info *unknown;  /* pointer to the "unknown" group */

  if ((conf.group_root = new_group_info()) == NULL)
    return 0;

  if ((unknown = new_group_info()) == NULL)
    return 0;

  if ((conf.group_root -> name = malloc(5 * sizeof(char))) == NULL)
    {
    perror("Memory allocation error");
    return 0;
    }

  if ((unknown -> name = (char *) malloc(8 * sizeof(char))) == NULL)
    {
    perror("Memory Allocation Error");
    return 0;
    }

  strcpy(conf.group_root -> name, "root");

  conf.group_root -> resgroup = -1;
  conf.group_root -> cresgroup = 0;
  conf.group_root -> percentage = 1.0;

  strcpy(unknown -> name, "unknown");
  unknown -> shares = conf.unknown_shares;
  unknown -> resgroup = 0;
  unknown -> cresgroup = 1;
  unknown -> parent = conf.group_root;
  conf.group_root -> child = unknown;
  return 1;
  }

/*
 *
 * count_shares - count the shares in a resource group
 *         a resource group is a group_info and all of its
 *         siblings
 *
 *  grp - The start of a sibling chain
 *
 * returns the number of shares
 *
 */
int count_shares(group_info *grp)
  {
  int shares = 0;  /* accumulator to count the shares */
  group_info *cur_grp;  /* the current group in a sibling chain */

  cur_grp = grp;

  while (cur_grp != NULL)
    {
    shares += cur_grp -> shares;
    cur_grp = cur_grp -> sibling;
    }

  return shares;
  }

/*
 *
 * calc_fair_share_perc - walk the fair share group tree and calculate
 *          the overall percentage of the machine a user/
 *          group gets if all usage is equal
 *
 *   root - the root of the current subtree
 *   shares - the number of total shares in the group
 *
 * returns success/failure
 *
 */
int calc_fair_share_perc(group_info *root, int shares)
  {
  int cur_shares;  /* total number of shares in the resgrp */

  if (root == NULL)
    return 0;

  if (shares == UNSPECIFIED)
    cur_shares = count_shares(root);
  else
    cur_shares = shares;

  root -> percentage = (float) root -> shares / (float) cur_shares *
                       root -> parent -> percentage;

  calc_fair_share_perc(root -> sibling, cur_shares);

  calc_fair_share_perc(root -> child, UNSPECIFIED);

  return 1;
  }

/*
 *
 * test_perc - a debugging function to check if the fair_share
 *      percentanges calculated add up to 100%
 *
 *   root - root of the current subtree
 *
 * returns total percentage (hopefully 1.0)
 *
 */
float test_perc(group_info *root)
  {
  if (root == NULL)
    return 0;

  return (root -> child == NULL ? root -> percentage : 0) + test_perc(root -> sibling) + test_perc(root -> child);
  }


/*
 *
 * update_usage_on_run - update a users usage information when a
 *         job is run
 *
 *   jinfo - the job which just started running
 *
 * returns nothing
 *
 */
void update_usage_on_run(job_info *jinfo)
  {
  jinfo -> ginfo -> temp_usage += calculate_usage_value(jinfo -> resreq);
  }

/*
 *
 * calculate_usage_value - calcualte a value that represents the usage
 *    information
 *
 *   resreq - a resource_req list that holds the resource info
 *
 * returns the calculated value
 *
 * NOTE: currently it will only return the number of cpu seconds used.
 *       This function can be more complicated
 *
 */
usage_t calculate_usage_value(resource_req *resreq)
  {
  resource_req *tmp;

  if (resreq != NULL)
    {
    tmp = find_resource_req(resreq, "cput");

    if (tmp != NULL)
      return tmp -> amount;
    }

  return 0L;
  }

/*
 *
 * decay_fairshare_tree - decay the usage information kept in the fair
 *          share tree
 *
 *   root - the root of the fairshare tree
 *
 * returns nothing
 *
 */
void decay_fairshare_tree(group_info *root)
  {
  if (root == NULL)
    return;

  decay_fairshare_tree(root -> sibling);

  decay_fairshare_tree(root -> child);

  root -> usage /= 2;

  if (root -> usage == 0)
    root -> usage = 1;
  }

/*
 *
 * extract_fairshare - extract the first job from the user with the
 *       least percentage / usage ratio
 *
 *   jobs - array of jobs to search through
 *
 * return the found job or NULL on error
 *
 */
job_info *extract_fairshare(job_info **jobs)
  {
  job_info *max = NULL;  /* job with the max shares / percentage */
  float max_value;  /* max shares / percentage */
  float cur_value;  /* the current shares / percentage value */
  int i;

  if (jobs != NULL)
    {
    max_value = -1;

    for (i = 0; jobs[i] != NULL; i++)
      {
      cur_value = jobs[i] -> ginfo -> percentage / jobs[i] -> ginfo -> temp_usage;

      if (max_value < cur_value && !jobs[i] -> is_running &&
          !jobs[i] -> can_not_run)
        {
        max = jobs[i];
        max_value = cur_value;
        }
      }
    }

  return max;
  }

/*
 *
 * print_fairshare - print out the fair share tree
 *
 *   root - root of subtree
 *
 * returns nothing
 *
 */
void print_fairshare(group_info *root)
  {
  if (root == NULL)
    return;

  if (root -> child == NULL)
    printf("User: %-10s Grp: %d cgrp: %d Usage %ld Perc: %f\n", root -> name,
           root -> resgroup, root -> cresgroup, root -> usage, root -> percentage);

  print_fairshare(root -> sibling);

  print_fairshare(root -> child);
  }

/*
 *
 * write_usage - write the usage information to the usage file
 *        This fuction uses a recursive helper function
 *
 *
 * returns success/failure
 *
 */
int
write_usage(void)
  {
  FILE *fp;  /* file pointer to usage file */

  if ((fp = fopen(USAGE_FILE, "wb")) == NULL)
    {
    perror("Error opening file " USAGE_FILE);
    return 0;
    }

  rec_write_usage(conf.group_root, fp);

  fclose(fp);
  return 1;
  }

/*
 *
 * rec_write_usage - recursive helper function which will write out all
 *     the group_info structs of the resgroup tree
 *
 *   root - the root of the current subtree
 *   fp   - the file to write the ginfo out to
 *
 * returns nothing
 *
 */
void rec_write_usage(group_info *root, FILE *fp)
  {

  struct group_node_usage grp;  /* used to write out usage info */

  if (root == NULL)
    return;

  if (root -> usage != 1)   /* usage defaults to 1 */
    {
    strcpy(grp.name, root -> name);
    grp.usage = root -> usage;

    if (!fwrite(&grp, sizeof(struct group_node_usage), 1, fp))
      return;
    }

  rec_write_usage(root -> sibling, fp);

  rec_write_usage(root -> child, fp);
  }

/*
 *
 * read_usage - read the usage information and load it into the
 *       resgroup tree.
 *
 * returns success/failure
 *
 */
int
read_usage(void)
  {
  FILE *fp;    /* file pointer to usage file */

  struct group_node_usage grp;  /* struct used to read in usage info */
  group_info *ginfo;   /* ptr to current group usage */


  if ((fp = fopen(USAGE_FILE, "r")) == NULL)
    {
    perror("Can not open file " USAGE_FILE);
    return 0;
    }

  while (fread(&grp, sizeof(struct group_node_usage), 1, fp))
    {
    ginfo = find_alloc_ginfo(grp.name);

    if (ginfo != NULL)
      ginfo -> usage = grp.usage;
    }

  fclose(fp);

  return 1;
  }
