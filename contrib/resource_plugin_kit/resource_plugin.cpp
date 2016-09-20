
#include <string>
#include <map>
#include <set>

#include <sys/types.h>

// Node reporting plug-in for Torque
//
// Please note that if you use the plugin, you assume all responsibilities for testing
// and debugging your plugin. As the developer of the plugin, you assume all liability
// for maintaining your plugin.
//


// To add a generic resource to a node, add the name as the key to the map, and the desired
// integer value as the value.
//
// To clear a generic resource, report it as 0. If you stop reporting the generic resource
// and you are having a scheduler use that generic resource as a factor, then it will result
// in undefined behavior.
void report_node_generic_resources(
    
  std::map<std::string, unsigned int> &gres_map)

  {
  }



// To add a generic metric to a node, add the name of the metric as the key, and the desired
// numeric value (stored as a double) as the value.
//
// To clear a generic metric, report it as 0 (or some otherwise neutral value). If you stop
// reporting the generic metric and you are having a schedule use that generic metric as a 
// factor, then it will result in undefined behavior.
void report_node_generic_metrics(
    
  std::map<std::string, double> &gmetric_map)

  {
  }



// To add a varattr to a node, add the name of the varattr as the key, and the value of the
// metric set as the value.
//
// To clear a varattr, you may simply stop reporting it.
void report_node_varattrs(
    
  std::map<std::string, std::string> &varattr_map)

  {
  }



// To add a feature to a node, simply add the feature name to this set
// 
// To clear a feature, simply stop reporting it
void report_node_features(
    
  std::set<std::string> &node_features)

  {
  }



// Job resource reporting plug-in for Torque
// Each string in usage_info should be reported in a name=value format
// Only one name and value should be placed in each string
// 
// jobid: the id of the job, provided by pbs_mom. Marked const to show that the plugin cannot
// communicate anything by altering this parameter.
//
// job_pids: a set of the pids in the job. Also marked const to show that the plugin cannot 
// communicate anything by altering this parameter.
//
// usage_info: The plugin is responsible for populating usage_info and managing it. The mom
// will not clear it between runs or otherwise alter the data - it is completely
// up to the plugin how this usage information should be altered each interval.
// NOTE: The intervals are right around the setting for $status_update_time in the mom's
// config file. This value defaults to 45 seconds if it isn't set manually.
void report_job_resources(
    
    const std::string                  &jobid,
    const std::set<pid_t>              &job_pids,
    std::map<std::string, std::string> &usage_info)

  {
  }

