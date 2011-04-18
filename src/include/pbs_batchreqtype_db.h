/** @file This file contains the database of PBS Batch request type ids and
 *        their text representations
 *
 * HOWTO USE THIS
 *
 * Add new batch request type:
 *  - add new line before the endif
 *
 * Remove batch request type:
 *  - rewrite the request into a gap (PBS_BATCH_GAPxxx) with (const char*)0
 *    as text part (see PBS_BATCH_GAP029-47 for example)
 *
 *  @see pbs_messages.c and libpbs.h for more information
 */

#ifdef PbsBatchReqType
PbsBatchReqType(PBS_BATCH_Connect = 0,          "Connect")
PbsBatchReqType(PBS_BATCH_QueueJob,             "QueueJob")
PbsBatchReqType(PBS_BATCH_JobCred,              "JobCred")
PbsBatchReqType(PBS_BATCH_jobscript,            "JobScript")
PbsBatchReqType(PBS_BATCH_RdytoCommit,          "ReadyToCommit")
PbsBatchReqType(PBS_BATCH_Commit,               "Commit")
PbsBatchReqType(PBS_BATCH_DeleteJob,            "DeleteJob")
PbsBatchReqType(PBS_BATCH_HoldJob,              "HoldJob")
PbsBatchReqType(PBS_BATCH_LocateJob,            "LocateJob")
PbsBatchReqType(PBS_BATCH_Manager,              "Manager")
PbsBatchReqType(PBS_BATCH_MessJob,              "MessageJob")         /* = 10 */
PbsBatchReqType(PBS_BATCH_ModifyJob,            "ModifyJob")
PbsBatchReqType(PBS_BATCH_MoveJob,              "MoveJob")
PbsBatchReqType(PBS_BATCH_ReleaseJob,           "ReleaseJob")
PbsBatchReqType(PBS_BATCH_Rerun,                "RerunJob")
PbsBatchReqType(PBS_BATCH_RunJob,               "RunJob")
PbsBatchReqType(PBS_BATCH_SelectJobs,           "SelectJobs")
PbsBatchReqType(PBS_BATCH_Shutdown,             "Shutdown")
PbsBatchReqType(PBS_BATCH_SignalJob,            "SignalJob")
PbsBatchReqType(PBS_BATCH_StatusJob,            "StatusJob")
PbsBatchReqType(PBS_BATCH_StatusQue,            "StatusQueue")        /* = 20 */
PbsBatchReqType(PBS_BATCH_StatusSvr,            "StatusServer")
PbsBatchReqType(PBS_BATCH_TrackJob,             "TrackJob")
PbsBatchReqType(PBS_BATCH_AsyrunJob,            "AsyncRunJob")
PbsBatchReqType(PBS_BATCH_Rescq,                "ResourceQuery")
PbsBatchReqType(PBS_BATCH_ReserveResc,          "ReserveResource")
PbsBatchReqType(PBS_BATCH_ReleaseResc,          "ReleaseResource")
PbsBatchReqType(PBS_BATCH_CheckpointJob,        "CheckpointJob")
PbsBatchReqType(PBS_BATCH_AsyModifyJob,         "AsyncModifyJob")
PbsBatchReqType(PBS_BATCH_GAP029,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP030,               "NONE")               /* = 30 */
PbsBatchReqType(PBS_BATCH_GAP031,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP032,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP033,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP034,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP035,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP036,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP037,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP038,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP039,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP040,               "NONE")               /* = 40 */
PbsBatchReqType(PBS_BATCH_GAP041,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP042,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP043,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP044,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP045,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP046,               "NONE")
PbsBatchReqType(PBS_BATCH_GAP047,               "NONE")
PbsBatchReqType(PBS_BATCH_StageIn,              "StageIn")
PbsBatchReqType(PBS_BATCH_AuthenUser,           "AuthenticateUser")
PbsBatchReqType(PBS_BATCH_OrderJob,             "OrderJob")          /* = 50 */
PbsBatchReqType(PBS_BATCH_SelStat,              "SelStat")
PbsBatchReqType(PBS_BATCH_RegistDep,            "RegisterDependency")
PbsBatchReqType(PBS_BATCH_ReturnFiles,          "ReturnFiles")
PbsBatchReqType(PBS_BATCH_CopyFiles,            "CopyFiles")
PbsBatchReqType(PBS_BATCH_DelFiles,             "DeleteFiles")
PbsBatchReqType(PBS_BATCH_JobObit,              "JobObituary")
PbsBatchReqType(PBS_BATCH_MvJobFile,            "MoveJobFile")
PbsBatchReqType(PBS_BATCH_StatusNode,           "StatusNode")
PbsBatchReqType(PBS_BATCH_Disconnect,           "Disconnect")
PbsBatchReqType(PBS_BATCH_AsySignalJob,         "AsyncSignalJob")     /* = 60 */
PbsBatchReqType(PBS_BATCH_AltAuthenUser,        "AlternateUserAuthentication") 
PbsBatchReqType(PBS_BATCH_GpuCtrl,              "GPUControl") 
#endif
