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
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

#include "cpu_frequency.hpp"
#include "node_frequency.hpp"

node_frequency nd_frequency;

static const char *baseCpuFrequency = "/BaseCpuFrequency.xml";
static const char *rootFrequencyName = "CpuFrequencies";
static const char *cpuNodeName = "CpuNode";
static const char *cpuNodeNumber = "NodeNumber";
static const char *frequencyTypeName = "type";
static const char *frequencyName = "KHz";
static const char *upperFrequencyName = "upperKHz";
static const char *lowerFrequencyName = "lowerKHz";


node_frequency::node_frequency()

  {
  last_error = PBSE_NONE;
  int i = 0;
  bool is_valid = true;

  while (is_valid)
    {
    cpu_frequency cpu_freq(i);

    if (cpu_freq.is_valid())
      {
      cpus.push_back(cpu_freq);
      i++;
      }
    else
      is_valid = false;
    }
  }



node_frequency::~node_frequency() 
  {
  // make this empty so that if the desctructor is called nothing happens
  }



//Save the current cpu frequency to an xml file in the mom_priv directory.
//If the file is already there don't overwrite it.

bool node_frequency::get_base_frequencies(
    
  const char *dir)

  {
  struct stat bf;
  std::string path = dir;
  path += baseCpuFrequency;
  if(cpus.size() == 0)
    {
    //We can't manage the frequency on this node.
    return true;
    }
  if(stat(path.c_str(),&bf) == 0)
    {
    //File already exists. load it up and return;
    if(load_base_frequencies(path))
      {
      return true;
      }
    base_frequencies.clear();
    }
  return save_base_frequencies(path);
  }



bool node_frequency::load_base_frequencies(
    
  std::string &path)

  {
  xmlDoc *doc = xmlReadFile(path.c_str(),NULL,0);
  if(doc == NULL)
    {
    return false;
    }
  xmlNode *root = xmlDocGetRootElement(doc);
  if(strcmp((const char *)root->name,rootFrequencyName))
    {
    xmlFreeDoc(doc);
    return false;
    }
  unsigned long i = 0;
  for(xmlNode *node = root->children;node != NULL;node = node->next)
    {
    if(strcmp((const char *)node->name,cpuNodeName))
      {
      continue;
      }
    char *cpuNum = (char *)xmlGetProp(node,(xmlChar *)cpuNodeNumber);
    xmlNode *typeNode = NULL;
    xmlNode *freqNode = NULL;
    xmlNode *maxFreqNode = NULL;
    xmlNode *minFreqNode = NULL;
    for(xmlNode *freq = node->children;freq != NULL;freq = freq->next)
      {
      if(freq->children == NULL)
        {
        continue;
        }
      if(freq->children->content == NULL)
        {
        continue;
        }
      if(!strcmp((const char *)freq->name,frequencyTypeName))
        {
        typeNode = freq;
        }
      if(!strcmp((const char *)freq->name,frequencyName))
        {
        freqNode = freq;
        }
      if(!strcmp((const char *)freq->name,upperFrequencyName))
        {
        maxFreqNode = freq;
        }
      if(!strcmp((const char *)freq->name,lowerFrequencyName))
        {
        minFreqNode = freq;
        }
      }
    if((cpuNum == NULL)||
        (typeNode == NULL)||
        (maxFreqNode == NULL)||
        (minFreqNode == NULL))
      {
      xmlFreeDoc(doc);
      last_error = PBSE_INVALID_FREQUENCY_FILE;
      free(cpuNum);
      return false;
      }

    unsigned long cpu = boost::lexical_cast<int>(cpuNum);
    free(cpuNum);

    if((cpu >= cpus.size())||(cpu != i))
      {
      xmlFreeDoc(doc);
      last_error = PBSE_INVALID_FREQUENCY_FILE;
      return false;
      }
    cpu_frequency_save saver;
    std::string typeStr = (const char *)typeNode->children->content;
    saver.type = cpus.at(cpu).get_governor_type(typeStr);
    saver.currFrequency = boost::lexical_cast<unsigned int>(freqNode->children->content);
    saver.maxFrequency = boost::lexical_cast<unsigned int>(maxFreqNode->children->content);
    saver.minFrequency = boost::lexical_cast<unsigned int>(minFreqNode->children->content);
    base_frequencies.push_back(saver);
    i++;
    }
  xmlFreeDoc(doc);
  if(base_frequencies.size() != cpus.size())
    {
    last_error = PBSE_INVALID_FREQUENCY_FILE;
    return false;
    }
  return true;
  }



bool node_frequency::save_base_frequencies(
    
  std::string &path)

  {
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;
  if((doc = xmlNewDoc((const xmlChar *)"1.0")) == NULL)
    {
    last_error = PBSE_SYSTEM;
    return false;
    }
  if ((root = xmlNewNode((xmlNsPtr)NULL,(const xmlChar *)rootFrequencyName)) == NULL)
    {
    xmlFreeDoc(doc);
    last_error = PBSE_SYSTEM;
    return false;
    }
  base_frequencies.clear();
  xmlDocSetRootElement(doc,root);
  int index = 0;

  for (unsigned int i = 0; i < cpus.size(); i++)
    {
    xmlNodePtr child = xmlNewChild(root,(xmlNsPtr)NULL,(const xmlChar *)cpuNodeName,(const xmlChar *)NULL);
    std::string n;
    n = boost::lexical_cast<std::string>(index);
    xmlSetProp(child,(const xmlChar *)cpuNodeNumber,(const xmlChar *)n.c_str());

    cpu_frequency_type type;
    unsigned long freq;
    unsigned long upperFreq;
    unsigned long lowerFreq;

    cpus[i].get_frequency(type,freq,upperFreq,lowerFreq);
    std::string typeStr;

    cpus[i].get_governor_string(type,typeStr);
    xmlNewChild(child,(xmlNsPtr)NULL,(const xmlChar *)frequencyTypeName,(const xmlChar *)typeStr.c_str());
    n = boost::lexical_cast<std::string>(freq);
    xmlNewChild(child,(xmlNsPtr)NULL,(const xmlChar *)frequencyName,(const xmlChar *)n.c_str());
    n = boost::lexical_cast<std::string>(upperFreq);
    xmlNewChild(child,(xmlNsPtr)NULL,(const xmlChar *)upperFrequencyName,(const xmlChar *)n.c_str());
    n = boost::lexical_cast<std::string>(lowerFreq);
    xmlNewChild(child,(xmlNsPtr)NULL,(const xmlChar *)lowerFrequencyName,(const xmlChar *)n.c_str());

    cpu_frequency_save saver;
    saver.type = type;
    saver.currFrequency = freq;
    saver.maxFrequency = upperFreq;
    saver.minFrequency = lowerFreq;
    base_frequencies.push_back(saver);
    }
  xmlSaveFormatFileEnc(path.c_str(),doc,NULL,1);
  xmlFreeDoc(doc);
  return true;
  }



bool node_frequency::restore_frequency()
  {
  if ((base_frequencies.size() == 0)||(base_frequencies.size() != cpus.size()))
    {
    last_error = PBSE_INVALID_FREQUENCY_FILE;
    return false;
    }

  unsigned int frequency_index = 0;
  for (unsigned int cpu_index = 0; cpu_index != cpus.size(); cpu_index++, frequency_index++)
    {
    cpu_frequency_save &cfs = base_frequencies[frequency_index];

    cpus[cpu_index].set_frequency(cfs.type, cfs.currFrequency, cfs.maxFrequency, cfs.minFrequency);
    }
  return true;
  }



bool node_frequency::get_frequency(
    
  cpu_frequency_type &type,
  unsigned long      &currMhz,
  unsigned long      &maxMhz,
  unsigned long      &minMhz)

  {
  if (cpus.size() == 0)
    {
    last_error = PBSE_NODE_CANT_MANAGE_FREQUENCY;
    return false;
    }

  if (!cpus.at(0).get_frequency(type,currMhz,maxMhz,minMhz))
    {
    last_error = cpus.at(0).get_last_error();
    return false;
    }
  currMhz /= 1000;
  maxMhz /= 1000;
  minMhz /= 1000;
  return true;
  }



bool node_frequency::get_frequency_string(std::string& str,bool full)
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
  if(full)
    {
    strm << ", max = ";
    strm << maxMhz;
    strm << "MHz, min = ";
    strm << minMhz;
    strm << "MHz";
    }
  str = strm.str();
  return true;
  }



bool node_frequency::set_frequency(
    
  cpu_frequency_type type,
  unsigned long      maxMhz,
  unsigned long      minMhz)

  {
  if (cpus.size() == 0)
    {
    last_error = PBSE_NODE_CANT_MANAGE_FREQUENCY;
    return false;
    }

  std::vector<unsigned long> actualMaxFrequencies;
  std::vector<unsigned long> actualMinFrequencies;
  cpu_frequency_type actualType;

  switch (type)
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

      for (unsigned int i = 0; i < cpus.size(); i++)
        {
        unsigned long freq;
        if (!cpus[i].get_pstate_frequency(type,freq))
          {
          last_error = cpus[i].get_last_error();
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

      if (maxMhz == 0) //Special case. Leave the frequencies unchanged and just change the governor.
        {
        for (unsigned int i = 0; i < cpus.size(); i++)
          {
          unsigned long freq,max,min;
          cpu_frequency_type t;

          if (!cpus[i].get_frequency(t,freq,max,min))
            {
            last_error = cpus[i].get_last_error();
            return false;
            }

          actualMaxFrequencies.push_back(max);
          actualMinFrequencies.push_back(min);
          }
        }
      else
        {
        for (unsigned int i = 0; i < cpus.size(); i++)
          {
          unsigned long freq;

          if (!cpus[i].get_nearest_available_frequency(maxMhz*1000,freq))
            {
            last_error = cpus[i].get_last_error();
            return false;
            }

          actualMaxFrequencies.push_back(freq);
          if (!cpus[i].get_nearest_available_frequency(minMhz*1000,freq))
            {
            last_error = cpus[i].get_last_error();
            return false;
            }
          actualMinFrequencies.push_back(freq);
          }
        }
      actualType = type;

      break;

      //case UserSpace:
    case AbsoluteFrequency:

      for (unsigned int i = 0; i < cpus.size(); i++)
        {
        unsigned long freq;

        if (!cpus[i].get_nearest_available_frequency(maxMhz*1000,freq))
          {
          last_error = cpus[i].get_last_error();
          return false;
          }

        actualMaxFrequencies.push_back(freq);
        if (!cpus[i].get_nearest_available_frequency(minMhz*1000,freq))
          {
          last_error = cpus[i].get_last_error();
          return false;
          }

        actualMinFrequencies.push_back(freq);
        }
      actualType = Performance;
      break;

    default:
      last_error = PBSE_NO_MATCHING_FREQUENCY;
      return false;
    }

  for (unsigned int i = 0; i != cpus.size(); i++)
    {
    if (!cpus[i].is_governor_available(actualType))
      {
      last_error = PBSE_NO_MATCHING_FREQUENCY;
      return false;
      }
    }
  
  std::vector<unsigned long>::iterator iMins = actualMinFrequencies.begin();
  std::vector<unsigned long>::iterator iMaxs = actualMaxFrequencies.begin();

  for (unsigned int i = 0; i < cpus.size(); i++, iMins++, iMaxs++)
    {
    if (!cpus[i].set_frequency(actualType,*iMaxs,*iMaxs,*iMins))
      {
      last_error = cpus[i].get_last_error();
      return false;
      }
    }
  return true;
  }

