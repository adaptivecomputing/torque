/*
 * mom_heirarchy.h
 *
 *  Created on: Oct 14, 2014
 *      Author: bdaw
 */

#ifndef MOM_HIERARCHY_HANDLER_H_
#define MOM_HIERARCHY_HANDLER_H_

#include <time.h>
#include <pthread.h>
#include <vector>
#include <string>
#include "mom_hierarchy.h"
#include "node_manager.h"


#define MOM_SEND_HIERARCHY_STARTUP_DELAY  600
#define MOM_CHECK_SEND_INTERVAL           600
#define SEND_DELAY_AFTER_RELOAD           10
#define RELOAD_TIME_PADDING               3

class mom_hierarchy_handler
  {
#ifdef TEST_FUNCTION
friend class test_mom_hierarchy_handler;
#endif
private:
  bool           sendOnStartup; //Send the hierarchy to all moms on startup. (Default is true.)
  bool           sendOnDemand;  //Only send the hierarchy if requested.
                      //(Assume all MOMs have it.) (Default is false.)
  time_t          lastReloadTime; //Last time hierarchy was changed.
  bool           filePresent; //true if the mom_hierarchy file is present and used.
  time_t          nextSendTime; //Next time to send out hierarchy.
  int             sendingThreadCount; //Number of threads sending hierarchies. */
  pthread_mutex_t hierarchy_mutex;

  std::vector<std::string> hierarchy_xml;


  void make_default_hierarchy();
  void check_if_in_nodes_file(const char *hostname, int level_index, unsigned short &rm_port);
  void convert_level_to_send_format(mom_nodes &nodes, int level_index);
  void convert_path_to_send_format(mom_levels &levels);
  void add_missing_nodes(void);
  void convert_mom_hierarchy_to_send_format(void);
  int sendHierarchyToNode(const char *name, unsigned short port, bool first_time);
  void loadHierarchy(void);
  pbsnode *nextNode(all_nodes_iterator **iter);

public:

  void *sendHierarchyThreadTask(void *vp);

  bool isHiearchyLoaded(void)
    {
    return lastReloadTime != 0;
    }
  bool isMomHierarchyFilePresent(void)
    {
    return filePresent;
    }
  void dontSendOnStartup(void)
    {
    pthread_mutex_lock(&hierarchy_mutex);
    nextSendTime = time(NULL) + MOM_SEND_HIERARCHY_STARTUP_DELAY;
    pthread_mutex_unlock(&hierarchy_mutex);
    }
  void onlySendOnDemand(void)
    {
    pthread_mutex_lock(&hierarchy_mutex);
    sendOnDemand = true;
    pthread_mutex_unlock(&hierarchy_mutex);
    }

  void initialLoadHierarchy(void); //Called only at startup.
  void reloadHierarchy(void);      //Called any time a node is dynamically added or removed.
  void checkAndSendHierarchy(bool first_time); //Called every iteration of the main loop.
  void sendHierarchyToANode(struct pbsnode *node); //Called when a mom requests the hierarchy

  mom_hierarchy_handler(void):
    sendOnStartup(true),
    sendOnDemand(false),
    lastReloadTime(0),
    filePresent(false),
    sendingThreadCount(0)
    {
    nextSendTime = time(NULL);
    pthread_mutex_init(&hierarchy_mutex,NULL);
    }
  };

extern mom_hierarchy_handler hierarchy_handler;



#endif /* MOM_HEIRARCHY_H_ */
