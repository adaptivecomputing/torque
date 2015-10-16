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

#include "log.h"
#include "attribute.h"
#include "server.h"
#include "utils.h"
#include "resource.h"
#include "attr_req_info.hpp"


/*
 * appends str with xml, making sure to escape reserved characters
 *
 * @param xml - to be appended to str
 * @param str - the string object that will be appended with xml
 */

void appendEscapedXML(
    
  const char *xml,
  std::string &str)

  {
  for(;*xml;xml++)
    {
    switch(*xml)
      {
      case '<':
        str += LT_ESCAPED;
        break;
      case '>':
        str += GT_ESCAPED;
        break;
      case '&':
        str += AMP_ESCAPED;
        break;
      case '"':
        str += QUOT_ESCAPED;
        break;
      case '\'':
        str += APOS_ESCAPED;
        break;
      default:
        str += *xml;
        break;
      }
    }
  }



int size_to_str(

  struct size_value  szv,
  char              *out,
  int                space)

  {
  snprintf(out,space,"%lu",szv.atsv_num);
  
  if (space - strlen(out) < 3)
    return(-1);

  /* SUCCESS */
  
  switch (szv.atsv_shift)
    {
    case 10:
      
      strcat(out,"kb");
      
      break;
      
    case 20:
      
      strcat(out,"mb");
      
      break;
      
    case 30:
      
      strcat(out,"gb");
      
      break;
      
    case 40:
      
      strcat(out,"tb");
      
      break;
      
    case 50:
      
      strcat(out,"pb");
    }

  return(PBSE_NONE);
  } /* END size_to_str */





/*
 * attr_to_str
 *
 * @param ds - the dynamic string we're printing the pbs_attribute into
 * @param aindex - the pbs_attribute's index
 * @param attr - the pbs_attribute
 * @param XML - boolean telling us whether to print XML or not
 */
int attr_to_str(

  std::string&      ds,     /* O */
  attribute_def    *at_def, /* I */
  pbs_attribute     attr,   /* I */
  bool              XML)    /* I */

  {
  /* used to print numbers and chars as strings */
  char local_buf[MAXLINE];

  if ((attr.at_flags & ATR_VFLAG_SET) == FALSE)
    return(NO_ATTR_DATA);

  switch (at_def->at_type)
    {
    case ATR_TYPE_LONG:

      snprintf(local_buf, sizeof(local_buf), "%ld", attr.at_val.at_long);
      ds += local_buf;

      break;

    case ATR_TYPE_CHAR:

      sprintf(local_buf, "%c", attr.at_val.at_char);
      ds += local_buf;

      break;

    case ATR_TYPE_STR:

      if (attr.at_val.at_str == NULL)
        return(NO_ATTR_DATA);

      if (strlen(attr.at_val.at_str) == 0)
        return(NO_ATTR_DATA);

      if (XML == true)
        appendEscapedXML(attr.at_val.at_str,ds);
      else
        ds += attr.at_val.at_str;

      break;

    case ATR_TYPE_ARST:
    case ATR_TYPE_ACL:

      {
      int j;
      struct array_strings *arst = attr.at_val.at_arst;

      if (arst == NULL)
        return(NO_ATTR_DATA);

      /* concatenate all of the array strings into one string */
      for (j = 0; j < arst->as_usedptr; j++)
        {
        if (j > 0)
          ds += ",";

        if (XML == true)
          appendEscapedXML(arst->as_string[j],ds);
        else
          ds += arst->as_string[j];
        }
      }

      break;

    case ATR_TYPE_SIZE:

      size_to_str(attr.at_val.at_size, local_buf, sizeof(local_buf));
      ds += local_buf;

      break;

    case ATR_TYPE_RESC:

      {
      resource *current = (resource *)GET_NEXT(attr.at_val.at_list);

      if (current == NULL)
        return(NO_ATTR_DATA);

      /* print all of the resources */
      while (current != NULL)
        {

        /* there are only 3 resource types used */
        switch (current->rs_value.at_type)
          {
          case ATR_TYPE_LONG:

            ds += "\t\t<";
            ds += current->rs_defin->rs_name;
            ds += ">";

            snprintf(local_buf, sizeof(local_buf), "%ld", current->rs_value.at_val.at_long);
            ds += local_buf;

            ds += "</";
            ds += current->rs_defin->rs_name;
            ds += ">";

            break;

          case ATR_TYPE_STR:

            /* Patch provided by Martin Siegert to fix seg-fault
             * when current->rs_value.at_val.at_str is NULL 
             * Bugzilla bug 101 
             */

            if (current->rs_value.at_val.at_str == NULL)
              break;

            if (strlen(current->rs_value.at_val.at_str) == 0)
              break;

            ds += "\t\t<";
            ds += current->rs_defin->rs_name;
            ds += ">";

            
            if (XML == true)
              appendEscapedXML(current->rs_value.at_val.at_str,ds);
            else
              ds += current->rs_value.at_val.at_str;

            ds += "</";
            ds += current->rs_defin->rs_name;
            ds += ">";

            break;

          case ATR_TYPE_SIZE:

            ds += "\t\t<";
            ds += current->rs_defin->rs_name;
            ds += ">";

            size_to_str(current->rs_value.at_val.at_size, local_buf, sizeof(local_buf));

            ds += local_buf;

            ds += "</";
            ds += current->rs_defin->rs_name;
            ds += ">";

            break;
          }

        current = (resource *)GET_NEXT(current->rs_link);
        ds += "\n";
        }
      }

      break;

    case ATR_TYPE_ATTR_REQ_INFO:
      {
      std::vector<std::string> names, values;
      attr_req_info *cr = (attr_req_info *)attr.at_val.at_ptr;

      if (cr == NULL)
        break;

      if (!strcmp(ATTR_req_infomin, at_def->at_name))
        cr->get_min_values(names, values);
      else if (!strcmp(ATTR_req_infomax, at_def->at_name))
        cr->get_max_values(names, values);
      else if (!strcmp(ATTR_req_infodefault, at_def->at_name))
        cr->get_default_values(names, values);
      else
        {
        /* something's not right */
        return(PBSE_BAD_PARAMETER);
        }

      for (unsigned int it = 0; it < names.size(); it++)
        {
        ds += "\n\t\t<";
        ds += names[it].c_str();
        ds += ">";

        ds += values[it].c_str();

        ds += "</";
        ds += names[it].c_str();
        ds += ">";
        }
      ds += "\n";

      }

      break;

    case ATR_TYPE_TV:
      /* Record seconds and milliseconds */
      sprintf(local_buf, "%ld.%ld", attr.at_val.at_timeval.tv_sec, attr.at_val.at_timeval.tv_usec);
      ds += local_buf;
      break;

    /* NYI */
    case ATR_TYPE_LIST:
    case ATR_TYPE_LL:
    case ATR_TYPE_SHORT:
    case ATR_TYPE_JINFOP:

      break;
    } /* END switch pbs_attribute type */

  return(PBSE_NONE);
  } /* END attr_to_str */




/* converts a string to an pbs_attribute 
 * @return PBSE_NONE on success
 */

int str_to_attr(

  const char           *name,   /* I */
  char                 *val,    /* I */
  pbs_attribute        *attr,   /* O */
  struct attribute_def *padef,  /* I */
  int                   limit)  /* I */

  {
  int   index;
  char  buf[MAXLINE<<5];
  char  log_buf[LOCAL_LOG_BUF_SIZE];

  if ((name == NULL) ||
      (val  == NULL) ||
      (attr == NULL))
    {
    return(-10);
    }

  index = find_attr(padef,name,limit);

  if (index < 0)
    return(ATTR_NOT_FOUND);

  switch (padef[index].at_type)
    {
    case ATR_TYPE_LONG:

      attr[index].at_val.at_long = strtol(val, NULL, 10);

      break;

    case ATR_TYPE_CHAR:

      attr[index].at_val.at_char = *val;

      break;

    case ATR_TYPE_STR:

      unescape_xml(val,buf,sizeof(buf));

      attr[index].at_val.at_str = strdup(buf);

      if (attr[index].at_val.at_str == NULL)
        {
        log_err(PBSE_SYSTEM, __func__, "Cannot allocate memory\n");

        return(PBSE_SYSTEM);
        }

      break;

    case ATR_TYPE_ARST:
    case ATR_TYPE_ACL:

      {
      int   rc;

      unescape_xml(val,buf,sizeof(buf));

      if ((rc = decode_arst(attr + index,name,NULL,buf,0)))
        return(rc);
      }

      break;

    case ATR_TYPE_SIZE:

      {
      unsigned long number;

      char *unit;

      number = strtol(val, NULL, 10);

      attr[index].at_val.at_size.atsv_units = ATR_SV_BYTESZ;
      attr[index].at_val.at_size.atsv_num = number;
      attr[index].at_val.at_size.atsv_shift = 0;

      /* the string always ends with kb,mb if it has units */
      unit = val + strlen(val) - 2;

      if (unit < val)
        break;
      else if (isdigit(*val))
        break;

      switch (*unit)
        {
        case 'k':

          attr[index].at_val.at_size.atsv_shift = 10;

          break;

        case 'm':

          attr[index].at_val.at_size.atsv_shift = 20;

          break;

        case 'g':

          attr[index].at_val.at_size.atsv_shift = 30;

          break;

        case 't':

          attr[index].at_val.at_size.atsv_shift = 40;

          break;

        case 'p':

          attr[index].at_val.at_size.atsv_shift = 50;

          break;

        }
      }

      break;

    case ATR_TYPE_RESC:

      {
      char *resc_parent;
      char *resc_child;
      char *resc_ptr = val;

      int   len = strlen(resc_ptr);
      int   rc;
      int   errFlg = 0;

      while (resc_ptr - val < len)
        {
        if (get_parent_and_child(resc_ptr,&resc_parent,&resc_child,
              &resc_ptr))
          {
          errFlg = TRUE;

          break;
          }
        
        if ((rc = decode_resc(&(attr[index]),name,resc_parent,resc_child,ATR_DFLAG_ACCESS)))
          {
          snprintf(log_buf,sizeof(log_buf),
            "Error decoding resource %s, %s = %s\n",
            name,
            resc_parent,
            resc_child);
          
          errFlg = TRUE;

          log_err(rc, __func__, log_buf);
          }
        }

      if (errFlg == TRUE)
        return(-1);

      }

      break;

    case ATR_TYPE_ATTR_REQ_INFO:
      {
      char *resc_parent;
      char *resc_child;
      char *resc_ptr = val;

      int   len = strlen(resc_ptr);
      int   rc;
      int   errFlg = 0;

      while (resc_ptr - val < len)
        {
        if (get_parent_and_child(resc_ptr,&resc_parent,&resc_child,
              &resc_ptr))
          {
          errFlg = TRUE;

          break;
          }
        
        if ((rc = decode_attr_req_info(&(attr[index]),name,resc_parent,resc_child,ATR_DFLAG_ACCESS)))
          {
          snprintf(log_buf,sizeof(log_buf),
            "Error decoding resource %s, %s = %s\n",
            name,
            resc_parent,
            resc_child);
          
          errFlg = TRUE;

          log_err(rc, __func__, log_buf);
          }
        }

      if (errFlg == TRUE)
        return(-1);


      }
      break;

    /* NYI */
    case ATR_TYPE_LIST:
    case ATR_TYPE_LL:
    case ATR_TYPE_SHORT:
    case ATR_TYPE_JINFOP:

      break;
    } /* END switch (pbs_attribute type) */

  attr[index].at_flags |= ATR_VFLAG_SET;

  return(PBSE_NONE);
  } /* END str_to_attr */
