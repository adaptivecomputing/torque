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
#include <algorithm>


#include "cpu_frequency.hpp"

const char *base_cpu_path = "/sys/devices/system/cpu/";

bool descending(unsigned long i,unsigned long j)
  {
  return i>j;
  }

cpu_frequency::cpu_frequency(int cpu_number):governor(Invalid)
  {
  path = base_cpu_path;
  path += "cpu";
  std::stringstream cnv;
  cnv << cpu_number;
  path += cnv.str();
  path += "/cpufreq/";

  last_error = PBSE_NODE_CANT_MANAGE_FREQUENCY;
  if(!get_number_from_file(path + "cpuinfo_min_freq",cpu_min_frequency))
    {
    valid = false;
    return;
    }
  if(!get_number_from_file(path + "cpuinfo_max_freq",cpu_max_frequency))
    {
    valid = false;
    return;
    }
  std::vector<std::string> govs;
  if(!get_strings_from_file(path + "scaling_available_governors",govs))
    {
    valid = false;
    return;
    }
  for(std::vector<std::string>::iterator i = govs.begin();i != govs.end();i++)
    {
    cpu_frequency_type t = get_governor_type(*i);
    if(t != Invalid)
      {
      available_governors.push_back(t);
      }
    }
  std::string gov;
  if(!get_string_from_file(path + "scaling_governor",gov))
    {
    valid = false;
    return;
    }
  governor = get_governor_type(gov);
  if(governor == Invalid)
    {
    valid = false;
    return;
    }
  if(!get_numbers_from_file(path + "scaling_available_frequencies",available_frequencies))
    {
    valid = false;
    return;
    }
  std::sort(available_frequencies.begin(),available_frequencies.end(),descending);
  valid = true;
  last_error = PBSE_NONE;
  }

bool cpu_frequency::get_frequency(cpu_frequency_type& type,unsigned long& khz, unsigned long& khzUpper, unsigned long& khzLower)
  {
  if(!valid) return false;
  std::string gov;
  if(!get_string_from_file(path + "scaling_governor",gov))
    {
    return false;
    }
  cpu_frequency_type t = get_governor_type(gov);
  if(t == Invalid)
    {
    return false;
    }
  if(!get_number_from_file(path + "cpuinfo_cur_freq",khz))
    {
    return false;
    }
  if(!get_number_from_file(path + "scaling_max_freq",khzUpper))
    {
    return false;
    }
  if(!get_number_from_file(path + "scaling_min_freq",khzLower))
    {
    return false;
    }
  type = t;
  last_error = PBSE_NONE;
  return true;
  }

bool cpu_frequency::set_frequency(cpu_frequency_type type,unsigned long khz,unsigned long khzUpper,unsigned long khzLower)
  {
  if(!valid) return false;
  bool governorAvailable = false;
  bool freqMatched = false;
  bool upperMatched = false;
  bool lowerMatched = false;
  for(std::vector<cpu_frequency_type>::iterator i = available_governors.begin();i != available_governors.end();i++)
    {
    if(*i == type)
      {
      governorAvailable = true;
      break;
      }
    }
  for(std::vector<unsigned long>::iterator i = available_frequencies.begin();i != available_frequencies.end();i++)
    {
    if(*i == khz) freqMatched = true;
    if(*i == khzUpper) upperMatched = true;
    if(*i == khzLower) lowerMatched = true;
    }
  if(!governorAvailable || !upperMatched || !lowerMatched || !freqMatched)
    {
    last_error = PBSE_FREQUENCY_NOT_AVAILABLE;
    return false;
    }
  std::string govString;
  get_governor_string(type,govString);
  if(!set_string_in_file(path + "scaling_governor",govString))
    {
    return false;
    }
  unsigned long currKHz;
  unsigned long currKHzUpper;
  unsigned long currKHzLower;
  cpu_frequency_type currType;
  if(!get_frequency(currType,currKHz,currKHzUpper,currKHzLower))
    {
    return false;
    }
  if(khzUpper < currKHzLower)
    {
    if(!set_number_in_file(path + "scaling_min_freq",khzLower))
      {
      return false;
      }
    if(!set_number_in_file(path + "scaling_max_freq",khzUpper))
      {
      return false;
      }
    }
  else
    {
    if(!set_number_in_file(path + "scaling_max_freq",khzUpper))
      {
      return false;
      }
    if(!set_number_in_file(path + "scaling_min_freq",khzLower))
      {
      return false;
      }
    }
  if(type == UserSpace)
    {
    if(!set_number_in_file(path + "scaling_setspeed",khz))
      {
      return false;
      }
    }
  return true;
  }

void cpu_frequency::get_governor_string(cpu_frequency_type type,std::string& str)
  {
  switch(type)
    {
    case Performance:
      str = "Performance";
      break;
    case PowerSave:
      str = "PowerSave";
      break;
    case OnDemand:
      str = "OnDemand";
      break;
    case Conservative:
      str = "Conservative";
      break;
    case UserSpace:
      str = "UserSpace";
      break;
    default:
      str = "Unknown";
      break;
    }
  }

cpu_frequency_type cpu_frequency::get_governor_type(std::string &freq_type)
  {
  if(!strcasecmp(freq_type.c_str(),"Performance"))
    {
    return Performance;
    }
  if(!strcasecmp(freq_type.c_str(),"PowerSave"))
    {
    return PowerSave;
    }
  if(!strcasecmp(freq_type.c_str(),"OnDemand"))
    {
    return OnDemand;
    }
  if(!strcasecmp(freq_type.c_str(),"Conservative"))
    {
    return Conservative;
    }
  if(!strcasecmp(freq_type.c_str(),"UserSpace"))
    {
    return UserSpace;
    }
  return Invalid;
  }

bool cpu_frequency::get_pstate_frequency(cpu_frequency_type pstate,unsigned long& khz)
  {
  if(!valid) return false;
  if(pstate > P15)
    {
    last_error = PBSE_BAD_PARAMETER;
    return false;
    }
  if(pstate >= (cpu_frequency_type)available_frequencies.size())
    {
    pstate = (cpu_frequency_type)(available_frequencies.size() - 1);
    }
  khz = available_frequencies.at(pstate);
  return true;
  }

bool cpu_frequency::get_nearest_available_frequency(unsigned long reqKhz,unsigned long& actualKhz)
  {
  if(!valid) return false;
  long diff = -1;
  unsigned long curKhz;
  for(std::vector<unsigned long>::iterator i = available_frequencies.begin();i != available_frequencies.end();i++)
    {
    long curDiff = (long)(reqKhz - *i);
    if(curDiff < 0)
      {
      curDiff = -curDiff;
      }
    if((curDiff < diff)||(diff == -1))
      {
      diff = curDiff;
      curKhz = *i;
      }
    else if((curDiff == diff)&&(curKhz > *i))
      {
      curKhz = *i;
      }
    }
  if(diff == -1)
    {
    last_error = PBSE_NO_MATCHING_FREQUENCY;
    return false;
    }
  actualKhz = curKhz;
  return true;
  }

bool cpu_frequency::is_governor_available(cpu_frequency_type governor)
  {
  if(!valid) return false;
  for(std::vector<cpu_frequency_type>::iterator i = available_governors.begin();i != available_governors.end();i++)
    {
    if(*i == governor)
      {
      return true;
      }
    }
  return false;
  }


