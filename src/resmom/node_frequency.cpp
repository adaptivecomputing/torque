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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>


#include "cpu_frequency.hpp"
#include "node_frequency.hpp"

node_frequency nd_frequency;

node_frequency::node_frequency()
  {
  int i = 0;
  cpu_frequency *cpu_freq;
  do
    {
    cpu_freq = new cpu_frequency(i);
    if(cpu_freq->is_valid())
      {
      cpus.push_back(cpu_freq);
      }
    else
      {
      delete cpu_freq;
      }
    i++;
    }while(cpu_freq->is_valid());
  }

bool node_frequency::get_frequency(cpu_frequency_type& type,unsigned long& currMhz,unsigned long& maxMhz,unsigned long& minMhz)
  {
  if(cpus.size() == 0)
    {
    last_error = "Node does not support managing cpu frequency.";
    return false;
    }
  if(!cpus.at(0).get_frequency(type,currMhz,maxMhz,minMhz))
    {
    last_error = cpus.at(0).get_last_error();
    return false;
    }
  return true;
  }

bool node_frequency::get_frequency_string(std::string& str)
  {
  cpu_frequency_type type;
  unsigned long currMhz,maxMhz,minMhz;
  if(!get_frequency(type,currMhz,maxMhz,minMhz))
    {
    return false;
    }
  std::stringstream strm;
  std::string gov;
  cpus.at(0).get_governor_string(type,gov);
  strm << gov.c_str();
  strm << ":";
  strm << currMhz;
  strm << "MHz";
  str = strm.str();
  return true;
  }

bool node_frequency::set_frequency(cpu_frequency_type type,unsigned long maxMhz,unsigned long minMhz)
  {
  if(cpus.size() == 0)
    {
    last_error = "Node does not support managing cpu frequency.";
    return false;
    }
  std::vector<unsigned long> actualMaxFrequencies;
  std::vector<unsigned long> actualMinFrequencies;
  cpu_frequency_type actualType;
  switch(type)
    {
    case P0:
    case P1:
    case P2:
    case P3:
    case P4:
    case P5:
    case P6:
    case P7:
    case P8:
    case P9:
    case P10:
    case P11:
    case P12:
    case P13:
    case P14:
    case P15:
      for(boost::ptr_vector<cpu_frequency>::iterator i = cpus.begin();i != cpus.end();i++)
        {
        unsigned long freq;
        if(!(*i).get_pstate_frequency(type,freq))
          {
          last_error = (*i).get_last_error();
          return false;
          }
        actualMaxFrequencies.push_back(freq);
        actualMinFrequencies.push_back(freq);
        }
      actualType = Performance;
      break;
    case Performance:
    case PowerSave:
    case OnDemand:
    case Conservative:
      for(boost::ptr_vector<cpu_frequency>::iterator i = cpus.begin();i != cpus.end();i++)
        {
        unsigned long freq;
        if(!(*i).get_nearest_available_frequency(maxMhz,freq))
          {
          last_error = (*i).get_last_error();
          return false;
          }
        actualMaxFrequencies.push_back(freq);
        if(!(*i).get_nearest_available_frequency(minMhz,freq))
          {
          last_error = (*i).get_last_error();
          return false;
          }
        actualMinFrequencies.push_back(freq);
        }
      actualType = type;
      break;
    //case UserSpace:
    case AbsoluteFrequency:
      for(boost::ptr_vector<cpu_frequency>::iterator i = cpus.begin();i != cpus.end();i++)
        {
        unsigned long freq;
        if(!(*i).get_nearest_available_frequency(maxMhz,freq))
          {
          last_error = (*i).get_last_error();
          return false;
          }
        actualMaxFrequencies.push_back(freq);
        if(!(*i).get_nearest_available_frequency(minMhz,freq))
          {
          last_error = (*i).get_last_error();
          return false;
          }
        actualMinFrequencies.push_back(freq);
        }
      actualType = Performance;
      break;
    default:
      last_error = "Unrecognized or unsupported frequency governor.";
      return false;
    }
  for(boost::ptr_vector<cpu_frequency>::iterator i = cpus.begin();i != cpus.end();i++)
    {
    if(!(*i).is_governor_available(actualType))
      {
      last_error = "Requested frequency governor is not available on this node.";
      return false;
      }
    }
  std::vector<unsigned long>::iterator iMins = actualMinFrequencies.begin();
  std::vector<unsigned long>::iterator iMaxs = actualMaxFrequencies.begin();
  for(boost::ptr_vector<cpu_frequency>::iterator i = cpus.begin();
      i != cpus.end();
      i++,iMins++,iMaxs++)
    {
    if(!(*i).set_frequency(actualType,*iMaxs,*iMins))
      {
      last_error = (*i).get_last_error();
      return false;
      }
    }
  return true;
  }

