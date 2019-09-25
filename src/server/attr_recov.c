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
 * save_attr.c - This file contains the functions to perform a buffered
 * save of an object (structure) and an pbs_attribute array to a file.
 * It also has the function to recover (reload) an pbs_attribute array.
 *
 * Included public functions are:
 *
 * save_struct copy a struct into the save i/o buffer
 * save_attr buffer and write attributes to disk file
 * recov_attr read attributes from disk file
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include "pbs_ifl.h"
#include <assert.h>
#include <errno.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "list_link.h"
#include "attribute.h"
#include "log.h"
#include "../lib/Liblog/pbs_log.h"
#include "lib_ifl.h"
#include "svrfunc.h"
#include "utils.h"
#include "server.h"
#include <string>

/* data items global to functions in this file */

#define ENDATTRIBUTES -711



#if 0

/*
 * save_struct - Copy a structure (as a block)  into the save i/o buffer
 * This is useful to save fixed sized structure without pointers
 * that point outside of the structure itself.
 *
 * Write out buffer as required. Update space_remaining
 *
 * Returns: 0 on success
 *  -1 on error
 */

int save_struct(

  char         *pobj,            /* I */
  unsigned int  objsize,         /* I */
  int           fds,             /* I */
  char         *buf_ptr,         /* M */
  size_t       *space_remaining, /* O */
  size_t        buf_size)        /* I */

  {
  size_t       left_to_copy = objsize;
  size_t       copy_size;
  char        *obj_current = pobj;
  void        *start_buf;

  while (left_to_copy > 0)
    {
    start_buf = buf_ptr + (buf_size - *space_remaining);

    if (left_to_copy > *space_remaining)
      {
      copy_size = *space_remaining;

      if (copy_size > 0)
        {
        memcpy(start_buf, obj_current, copy_size);
        obj_current += copy_size;
        left_to_copy -= copy_size;
        }
    
      /* write the buffer to the file */
      if (write_buffer(buf_ptr, buf_size, fds) != PBSE_NONE)
        {
        /* FAILURE */
        log_err(-1, __func__, "Cannot write the buffer to the file!");
        return(-1);
        }
      else
        {
        *space_remaining = buf_size;
        }
      }
    else
      {
      /* copy the rest */
      memcpy(start_buf, obj_current, left_to_copy);
      *space_remaining -= left_to_copy;
      left_to_copy = 0;
      }
    }

  return(PBSE_NONE);
  }  /* END save_struct() */


#endif

#if 0
/*
 * save_attr() - write set of attributes to disk file
 *
 * Each of the attributes is encoded  into the attrlist form.
 * They are packed and written using save_struct().
 *
 * The final real pbs_attribute is followed by a dummy pbs_attribute with a
 * al_size of ENDATTRIB.  This cannot be mistaken for the size of a
 * real pbs_attribute.
 *
 * Note: attributes of type ATR_TYPE_ACL are not saved with the other
 * pbs_attribute of the parent (queue or server).  They are kept in their
 * own file.
 */

int save_attr(

  struct attribute_def *padef,           /* pbs_attribute definition array */
  pbs_attribute        *pattr,           /* ptr to pbs_attribute value array */
  int                   numattr,         /* number of attributes in array */
  int                   fds,
  char                 *buf_ptr,         /* M */
  size_t               *space_remaining, /* O */
  size_t                buf_size)        /* I */

  {
  svrattrl    dummy;
  int         errct = 0;
  tlist_head  lhead;
  int         i;
  int         resc_access_perm = ATR_DFLAG_ACCESS;
  svrattrl   *pal;
  int         rc;

  /* encode each pbs_attribute which has a value (not non-set) */

  CLEAR_HEAD(lhead);

  for (i = 0;i < numattr;i++)
    {
    if ((padef + i)->at_type != ATR_TYPE_ACL)
      {
      /* NOTE: access lists are not saved this way */

      rc = (padef + i)->at_encode(
          pattr + i,
          &lhead,
          (padef + i)->at_name,
          NULL,
          ATR_ENCODE_SAVE,
          resc_access_perm);

      if (rc < 0)
        errct++;

      (pattr + i)->at_flags &= ~ATR_VFLAG_MODIFY;

      /* now that it has been encoded, block and save it */

      while ((pal = (svrattrl *)GET_NEXT(lhead)) != NULL)
        {
        if (save_struct((char *)pal, pal->al_tsize, fds, buf_ptr, space_remaining, buf_size) < 0)
          errct++;

        delete_link(&pal->al_link);

        free(pal);
        }
      }
    }  /* END for (i) */

  /* indicate last of attributes by writing dummy entry */

  memset(&dummy, 0, sizeof(dummy));

  dummy.al_tsize = ENDATTRIBUTES;

  if (save_struct((char *)&dummy, sizeof(dummy), fds, buf_ptr, space_remaining, buf_size) < 0)
    errct++;

  if (errct != 0)
    {
    return(-1);
    }

  /* SUCCESS */

  return(0);
  }  /* END save_attr() */
#endif



#ifndef PBS_MOM
int save_attr_xml(

  struct attribute_def *padef,   /* pbs_attribute definition array */
  pbs_attribute        *pattr,   /* ptr to pbs_attribute value array */
  int                   numattr, /* number of attributes in array */
  int                   fds)     /* file descriptor where attributes are written */

  {
  int             i;
  int             rc;
  char            buf[MAXLINE<<8];
  char            log_buf[LOCAL_LOG_BUF_SIZE];
  std::string      ds = "";

  /* write the opening tag for attributes */
  snprintf(buf,sizeof(buf),"<attributes>\n");
  if ((rc = write_buffer(buf,strlen(buf),fds)) != 0)
    {
    return(rc);
    }

  for (i = 0; i < numattr; i++)
    {
    if (pattr[i].at_flags & ATR_VFLAG_SET)
      {
      buf[0] = '\0';
      ds.clear();

      if ((rc = attr_to_str(ds, padef+i, pattr[i], true)) != 0)
        {
        if (rc != NO_ATTR_DATA)
          {
          /* ERROR */
          snprintf(log_buf,sizeof(log_buf),
              "Not enough space to print pbs_attribute %s",
              padef[i].at_name);

          return(rc);
          }
        }
      else
        {
        snprintf(buf,sizeof(buf),"<%s>%s</%s>\n",
            padef[i].at_name,
            ds.c_str(),
            padef[i].at_name);

        if ((rc = write_buffer(buf,strlen(buf),fds)) != 0)
          {
          return(rc);
          }
        }
      }
    } /* END for each pbs_attribute */

  /* close the attributes */
  snprintf(buf,sizeof(buf),"</attributes>\n");
  rc = write_buffer(buf,strlen(buf),fds);

  /* we can just return this since its the last write */
  return(rc);
  } /* END save_attr_xml() */
#endif /* ndef PBS_MOM */



/*
 * recov_attr() - read attributes from disk file
 *
 * Recover (reload) pbs_attribute from file written by save_attr().
 * Since this is not often done (only on server initialization),
 * Buffering the reads isn't done.
 */

int recov_attr(

  int                   fd,
  void                 *parent,
  struct attribute_def *padef,
  pbs_attribute        *pattr,
  int                   limit,
  int                   unknown,
  int					do_actions)

  {
  int       amt;
  int       i;
  int       index;
  int       palsize = 0;
  int       resc_access_perm = ATR_DFLAG_ACCESS;

  svrattrl *pal = NULL;
  svrattrl  tempal;
  char     *endPal;
#ifndef PBS_MOM
  bool      exec_host_found = false;
  char      job_state = 0;
#endif

  /* set all privileges (read and write) for decoding resources */
  /* This is a special (kludge) flag for the recovery case, see */
  /* decode_resc() in lib/Libattr/attr_fn_resc.c                        */


  /* For each pbs_attribute, read in the attr_extern header */

  while (1)
    {
    i = read_ac_socket(fd, (char *) & tempal, sizeof(tempal));

    if (i != sizeof(tempal))
      {
      log_err(errno, __func__, "read1");

      return(-1);
      }

    if (tempal.al_tsize == ENDATTRIBUTES)
      break;            /* hit dummy pbs_attribute that is eof */

    if (tempal.al_tsize <= (int)sizeof(tempal))
      {
      log_err(-1, __func__, "attr size too small");

      return(-1);
      }

    /* read in the pbs_attribute chunk (name and encoded value) */

    palsize = tempal.al_tsize;

    pal = (svrattrl *)calloc(1, palsize);

    if (pal == NULL)
      {
      log_err(errno, __func__, "calloc failed");

      return(-1);
      }
    endPal = (char *)pal + palsize;

    memcpy(pal, &tempal, sizeof(svrattrl));
  
    CLEAR_LINK(pal->al_link);

    /* read in the actual pbs_attribute data */

    amt = pal->al_tsize - sizeof(svrattrl);

    i = read_ac_socket(fd, (char *)pal + sizeof(svrattrl), amt);

    if (i != amt)
      {
      log_err(errno, __func__, "read2");

      free(pal);

      return(-1);
      }

    /* the pointer into the data are of course bad, so reset them */

    pal->al_name = (char *)pal + sizeof(svrattrl);

    if (pal->al_rescln)
      {
      pal->al_resc = pal->al_name + pal->al_nameln;
      if(((char *)pal->al_resc > endPal)||((char *)pal->al_resc < (char *)pal))
        {
        //Bad size in file.
        free(pal);
        return(-1);
        }
      }
    else
      pal->al_resc = NULL;

    if (pal->al_valln)
      {
      pal->al_value = pal->al_name + pal->al_nameln + pal->al_rescln;
      if(((char *)pal->al_value > endPal)||((char *)pal->al_value < (char *)pal))
        {
        //Bad size in file.
        free(pal);
        return(-1);
        }
      }
    else
      pal->al_value = NULL;

    if((pal->al_name + pal->al_nameln + pal->al_rescln + pal->al_valln) > endPal)
      {
      //Bad size in file.
      free(pal);
      return(-1);
      }

    /* find the pbs_attribute definition based on the name */

    index = find_attr(padef, pal->al_name, limit);

    if (index < 0)
      {
      /*
       * There are two ways this could happen:
       * 1. if the (job) pbs_attribute is in the "unknown" list -
       * keep it there;
       * 2. if the server was rebuilt and an pbs_attribute was
       * deleted, -  the fact is logged and the pbs_attribute
       * is discarded (system,queue) or kept (job)
       *
       */

      if (unknown > 0)
        {
        index = unknown;
        }
      else
        {
        log_err(-1, __func__, "unknown attribute discarded");

        free(pal);

        continue;
        }
      }    /* END if (index < 0) */

#ifndef PBS_MOM
    if (!strcmp(pal->al_name, ATTR_exechost))
      {
      exec_host_found = true;
      }

    if ((!strcmp(pal->al_name, ATTR_state))&&(pal->al_value != NULL))
      {
      job_state = *pal->al_value;
      }
#endif


    (padef + index)->at_decode(
        pattr + index,
        pal->al_name,
        pal->al_resc,
        pal->al_value,
        resc_access_perm);

    if ((do_actions) && (padef + index)->at_action != NULL)
	  {
      (padef + index)->at_action(pattr + index, parent, ATR_ACTION_RECOV);
	  }

    (pattr + index)->at_flags = pal->al_flags & ~ATR_VFLAG_MODIFY;

    free(pal);
    }  /* END while (1) */


#ifndef PBS_MOM
  if ((exec_host_found == false) && 
      ((job_state == 'R') ||
       (job_state == 'E')))
    {   
    return(-1);
    }
#endif

  return(0);
  }  /* END recov_attr() */

/* END attr_recov.c */

