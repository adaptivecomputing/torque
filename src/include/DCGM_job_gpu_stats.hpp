#include "license_pbs.h"

#include <vector>
#include <string>
#include <dcgm_agent.h>
#include <dcgm_fields.h>


class DCGM_GpuUsageInfo
  {
  std::string this_mom_name;              // This is not part of the dcgmFpuUsage_t structure. Put here so we
                                          // know which MOM gpu information came from.
	unsigned int  gpuId;
  /* All of the following are during the job's lifetime */

  long long energyConsumed;                 //!< Energy consumed in milliwatt-seconds
  dcgmStatSummaryInt64_t pcieRxBandwidth;   //!< PCI-E bytes read from the GPU
  dcgmStatSummaryInt64_t pcieTxBandwidth;   //!< PCI-E bytes written to the GPU 
  long long pcieReplays;                    //!< Count of PCI-E replays that occurred 
  long long startTime;                      //!< User provided job start time in microseconds since 1970
  long long endTime;                        //!< User provided job end time in microseconds since 1970 
  dcgmStatSummaryInt32_t smUtilization;     //!< Utilization of the GPU's SMs in percent 
  dcgmStatSummaryInt32_t memoryUtilization; //!< Utilization of the GPU's memory in percent
  unsigned int eccSingleBit;                //!< Count of ECC single bit errors that occurred 
  unsigned int eccDoubleBit;                //!< Count of ECC double bit errors that occurred 
  dcgmStatSummaryInt32_t memoryClock;       //!< Memory clock in MHz 
  dcgmStatSummaryInt32_t smClock;           //!< SM clock in MHz
  int numXidCriticalErrors;                 //!< Number of valid entries in xidCriticalErrorsTs
  long long xidCriticalErrorsTs[10];        //!< Timestamps of the critical XID errors that occurred
  int numComputePids;                       //!< Count of computePids entries that are valid 
  unsigned int computePids[10];             //!< List of compute processes that ran during the job. 0=no process 
  int numGraphicsPids;                      //!< Count of graphicsPids entries that are valid 
  unsigned int graphicsPids[10];            //!< List of compute processes that ran during the job. 0=no process

  long long maxGpuMemoryUsed;               //!< Maximum amount of GPU memory that was used in bytes

  long long powerViolationTime;             //!< Number of microseconds we were at reduced clocks due to power violation
  long long thermalViolationTime;           //!< Number of microseconds we were at reduced clocks due to thermal violation
  long long reliabilityViolationTime;       //!< Amount of microseconds we were at reduced clocks due to the reliability limit 
  long long boardLimitViolationTime;        //!< Amount of microseconds we were at reduced clocks due to being at the board's max voltage
  long long lowUtilizationTime;             //!< Amount of microseconds we were at reduced clocks due to low utilization
  long long syncBoostTime;

  public:
  DCGM_GpuUsageInfo();
  DCGM_GpuUsageInfo(dcgmGpuUsageInfo_t& gpuUsageInfo);
  ~DCGM_GpuUsageInfo();
  DCGM_GpuUsageInfo &operator = (const DCGM_GpuUsageInfo &other);

  int  set_value(const char *name, const char *value);
  void get_gpuId(unsigned int &gpuId);
  void initializeSummary();
  void initializeSummary(dcgmGpuUsageInfo_t &gpu_usage_info);
  void write_gpu_usage_info(std::string &usage_info);
  int parse_gpu_usage_array(std::string name, std::string value);
  int parse_gpu_usage_object(std::string name, std::string value);
  unsigned int get_gpuId();
  void get_energyConsumed(long long& energyConsumed);
  void get_eccSingleBitErrors(unsigned int& single_bit_errors);
  void get_eccDoubleBitErrors(unsigned int& double_bit_errors);
  std::string get_mom_name(void);
  };


class DCGM_job_gpu_stats
  {
  unsigned int    version;              //!< Version of this message  (dcgmPidInfo_version)
	int numGpus;                          //!< Number of GPUs that are valid in gpus
  long long       total_energy_consumed; // total energy consumed by all GPUs on all nodes in job
  unsigned int    total_eccSingleBit;    // total ECC single bit errors for entire job
  unsigned int    total_eccDoubleBit;    // total ECC double bit errors for entire job
	DCGM_GpuUsageInfo summary;            //!< Summary information for all GPUs listed in gpus[] 
	std::vector<DCGM_GpuUsageInfo> gpus;  //!< Per-GPU information for this PID


	public:

    DCGM_job_gpu_stats();
		DCGM_job_gpu_stats(dcgmJobInfo_t *jobInfo);
    ~DCGM_job_gpu_stats();
		DCGM_job_gpu_stats &operator = (const DCGM_job_gpu_stats &other);

    void initializeGpuJobInfo(dcgmJobInfo_t &gpu_job_info);
		int set_value(const char *name, const char *value);
		void get_values(std::vector<std::string> &names, std::vector<std::string> &values);
    void get_dcgm_version(unsigned int &version);
    void get_dcgm_num_gpus(int &num_gpus);
    void get_summary_gpuId(unsigned int& gpuId);
    void get_totalEnergyConsumed(long long& total_energy_consumed);
    void get_totalECCSingleBitErrors(unsigned int total_ecc_single_bit_errors);
    void get_totalECCDoubleBitErrors(unsigned int total_ecc_double_bit_errors);
    void add_gpu_usage_info(DCGM_GpuUsageInfo *gpu_info);

	};
