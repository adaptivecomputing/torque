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


#include "sys_file.hpp"



bool sys_file::get_number_from_file(std::string path, unsigned long& num)
  {
  std::string line;
  std::ifstream file(path.c_str(),std::ios::in);
  if(!file.is_open())
    {
    last_error = PBSE_CAN_NOT_OPEN_FILE;
    return false;
    }
  if(getline(file,line))
    {
    try
    {
      unsigned long n = boost::lexical_cast<unsigned long>(line);
      num = n;
      file.close();
      return true;
    }
    catch(...)
      {
      last_error = PBSE_UNEXPECTED_DATA_IN_FILE;
      file.close();
      return false;
      }
    }
  else
    {
    last_error = PBSE_CAN_NOT_READ_FILE;
    file.close();
    return false;
    }
  }

bool sys_file::get_numbers_from_file(std::string path, std::vector<unsigned long>& nums)
  {
  std::vector<std::string> strs;
  nums.clear();

  if(!get_strings_from_file(path,strs))
    {
    return false;
    }
  if(strs.size() == 0)
    {
    last_error = PBSE_UNEXPECTED_DATA_IN_FILE;
    return false;
    }
  for(std::vector<std::string>::iterator i = strs.begin();i != strs.end();i++)
    {
    try
    {
      unsigned long n = boost::lexical_cast<unsigned long>(*i);
      nums.push_back(n);
    }
    catch(...)
      {
      last_error = PBSE_UNEXPECTED_DATA_IN_FILE;
      nums.clear();
      return false;
      }
    }
  return true;
  }

bool sys_file::set_number_in_file(std::string path,unsigned long num)
  {
  std::string line;
  std::ofstream file(path.c_str(),std::ios::out|std::ios::trunc);
  if(!file.is_open())
    {
    last_error = PBSE_CAN_NOT_OPEN_FILE;
    return false;
    }
  file << num;
  file.close();
  return true;
  }

bool sys_file::get_string_from_file(std::string path,std::string& str)
  {
  std::string line;
  std::ifstream file(path.c_str(),std::ios::in);
  if(!file.is_open())
    {
    last_error = PBSE_CAN_NOT_OPEN_FILE;
    return false;
    }
  if(getline(file,line))
    {
    boost::algorithm::trim(line);
    str = line;
    file.close();
    return true;
    }
  else
    {
    last_error = PBSE_CAN_NOT_READ_FILE;
    file.close();
    return false;
    }
  }

bool sys_file::get_strings_from_file(std::string path,std::vector<std::string>& strs)
  {
  std::string line;
  strs.clear();
  std::ifstream file(path.c_str(),std::ios::in);
  if(!file.is_open())
    {
    last_error = PBSE_CAN_NOT_OPEN_FILE;
    return false;
    }
  while(getline(file,line))
    {
    std::stringstream ss(line);
    std::string buf;
    while(ss >> buf)
      {
      strs.push_back(buf);
      }
    }
  file.close();
  if(strs.size() == 0)
    {
    last_error = PBSE_UNEXPECTED_DATA_IN_FILE;
    return false;
    }
  return true;
  }

bool sys_file::set_string_in_file(std::string path,std::string str)
  {
  std::string line;
  std::ofstream file(path.c_str(),std::ios::out|std::ios::trunc);
  if(!file.is_open())
    {
    last_error = PBSE_CAN_NOT_OPEN_FILE;
    return false;
    }
  file << str;
  file.close();
  return true;
  }

const char *sys_file::get_last_error_string()
  {
  return pbse_to_txt(last_error);
  }
int sys_file::get_last_error()
  {
  return last_error;
  }


