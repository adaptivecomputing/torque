#         OpenPBS (Portable Batch System) v2.3 Software License
# 
# Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
# All rights reserved.
# 
# ---------------------------------------------------------------------------
# For a license to use or redistribute the OpenPBS software under conditions
# other than those described below, or to purchase support for this software,
# please contact Veridian Systems, PBS Products Department ("Licensor") at:
# 
#    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
#                        877 902-4PBS (US toll-free)
# ---------------------------------------------------------------------------
# 
# This license covers use of the OpenPBS v2.3 software (the "Software") at
# your site or location, and, for certain users, redistribution of the
# Software to other sites and locations.  Use and redistribution of
# OpenPBS v2.3 in source and binary forms, with or without modification,
# are permitted provided that all of the following conditions are met.
# After December 31, 2001, only conditions 3-6 must be met:
# 
# 1. Commercial and/or non-commercial use of the Software is permitted
#    provided a current software registration is on file at www.OpenPBS.org.
#    If use of this software contributes to a publication, product, or
#    service, proper attribution must be given; see www.OpenPBS.org/credit.html
# 
# 2. Redistribution in any form is only permitted for non-commercial,
#    non-profit purposes.  There can be no charge for the Software or any
#    software incorporating the Software.  Further, there can be no
#    expectation of revenue generated as a consequence of redistributing
#    the Software.
# 
# 3. Any Redistribution of source code must retain the above copyright notice
#    and the acknowledgment contained in paragraph 6, this list of conditions
#    and the disclaimer contained in paragraph 7.
# 
# 4. Any Redistribution in binary form must reproduce the above copyright
#    notice and the acknowledgment contained in paragraph 6, this list of
#    conditions and the disclaimer contained in paragraph 7 in the
#    documentation and/or other materials provided with the distribution.
# 
# 5. Redistributions in any form must be accompanied by information on how to
#    obtain complete source code for the OpenPBS software and any
#    modifications and/or additions to the OpenPBS software.  The source code
#    must either be included in the distribution or be available for no more
#    than the cost of distribution plus a nominal fee, and all modifications
#    and additions to the Software must be freely redistributable by any party
#    (including Licensor) without restriction.
# 
# 6. All advertising materials mentioning features or use of the Software must
#    display the following acknowledgment:
# 
#     "This product includes software developed by NASA Ames Research Center,
#     Lawrence Livermore National Laboratory, and Veridian Information
#     Solutions, Inc.
#     Visit www.OpenPBS.org for OpenPBS software support,
#     products, and information."
# 
# 7. DISCLAIMER OF WARRANTY
# 
# THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
# ARE EXPRESSLY DISCLAIMED.
# 
# IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
# U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# This license will be governed by the laws of the Commonwealth of Virginia,
# without reference to its choice of law rules.
######################################################################################
# This file contains PBS dependent commands.
######################################################################################
# getdata: Get data from a list of servers and load the widgets: jobsListbox, 
#	   hostsListbox, and queuesListbox.  
proc getdata {server_names {jobs_info_only 0}} {

  global HOSTS_COLUMN_LABEL QUEUES_COLUMN_LABEL JOBS_COLUMN_LABEL \
  	 HOSTS_COLUMN_HEADER QUEUES_COLUMN_HEADER JOBS_COLUMN_HEADER \
	 LINES_TO_IGNORE HOSTS_LISTBOX_KEY QUEUES_LISTBOX_KEY \
	 JOBS_LISTBOX_KEY HOSTS_LISTBOX_FKEY QUEUES_LISTBOX_FKEY \
	 JOBS_LISTBOX_FKEY PBS_QSTATDUMP_CMD \
  	 hostsListbox queuesListbox jobsListbox \
  	 hostsSelected queuesSelected jobsSelected \
  	 hostinfo jobinfo qinfo tk_version timeout_secs

  busy_cursor
  if {$jobs_info_only} { 
    set cmd_pipeline "$PBS_QSTATDUMP_CMD -t $timeout_secs -J [build_sel_options] $server_names"
    catch {unset jobinfo}
    $jobsListbox delete 0 end
  } else {
    set cmd_pipeline "$PBS_QSTATDUMP_CMD -t $timeout_secs [build_sel_options] $server_names"
    catch {unset hostinfo qinfo jobinfo}
    $hostsListbox delete 0 end
    $queuesListbox delete 0 end
    $jobsListbox delete 0 end
  }

  set f [cmdExec_bg $cmd_pipeline]

  if { $f == -1 } {
    remove_busy_cursor
    return
  } 

  set where ""

  set cnt 0
  while {[gets $f line] >= 0} {
    update idletasks
    if {[string compare $HOSTS_COLUMN_HEADER ""] != 0 && \
        [regexp "^:($HOSTS_COLUMN_HEADER)" $line m item]} {
       set where "hosts"
       set HOSTS_COLUMN_LABEL [string trimleft $line ":"]
    } elseif {[string compare $QUEUES_COLUMN_HEADER ""] != 0 && \
              [regexp "^:($QUEUES_COLUMN_HEADER)" $line m item]} {
       set where "queues"
       set QUEUES_COLUMN_LABEL [string trimleft $line ":"]
    } elseif {[string compare $JOBS_COLUMN_HEADER ""] != 0 && \
              [regexp "^:($JOBS_COLUMN_HEADER)" $line m item]} {
       set where "jobs"
       set JOBS_COLUMN_LABEL [string trimleft $line ":"]
    } elseif {[string compare $LINES_TO_IGNORE ""] != "" && \
              [regexp "($LINES_TO_IGNORE)" $line m item]} {
       continue	
    } else {
       set fkey ""
       switch -exact -- $where {
	 "hosts"  { 
	    set fkey $HOSTS_LISTBOX_FKEY
	 }
         "queues" {
	    set fkey $QUEUES_LISTBOX_FKEY
         }
         "jobs"   {
	    set fkey $JOBS_LISTBOX_FKEY
         }
       }
       set fkeyval "" 
       foreach i $fkey {
	 if {[string compare $fkeyval ""] == 0} {
	    set fkeyval [lindex $line $i]
	 } else {
	    set fkeyval "$fkeyval@[lindex $line $i]"
	 }
       }
       switch -exact -- $where {
	 "hosts"  {
	   if [info exists hostinfo($fkeyval)] {
	     set hostinfo($fkeyval) "$hostinfo($fkeyval) \"$line\"" 
	   } else {
	     set hostinfo($fkeyval) \"$line\" 
	   }

           $hostsListbox insert end $line
           set do_select "[lsearch -exact $hostsSelected $fkeyval]"
	   if {$do_select != -1} {
	      if {$tk_version < 4.0} {
                listbox_non_contiguous_selection $hostsListbox \
                                       [$hostsListbox curselect] end
	      } else {
	        $hostsListbox selection anchor end
	        $hostsListbox selection set anchor end
		$hostsListbox see end
	      }
           }
         }
         "queues" {
	   if [info exists qinfo($fkeyval)] {
             set qinfo($fkeyval) "$qinfo($fkeyval) \"$line\""
           } else {
             set qinfo($fkeyval) "\"$line\""
	   }
           set do_add "[lsearch -exact $hostsSelected $fkeyval]"

	   if {$do_add != -1} {
              $queuesListbox insert end $line

              set do_select "[lsearch -exact $queuesSelected \
		             [strget_keyvals $line $QUEUES_LISTBOX_KEY "@"]]"

              if {$do_select != -1} {
	        if {$tk_version < 4.0} {
                  listbox_non_contiguous_selection $queuesListbox \
                                       [$queuesListbox curselect] end
		} else {
		  $queuesListbox selection anchor end
		  $queuesListbox selection set anchor end
		  $queuesListbox see end
		}
              }
           }
         }
         "jobs"   {
	   if [info exists jobinfo($fkeyval)] {
             set jobinfo($fkeyval) "$jobinfo($fkeyval) \"$line\""
           } else {
             set jobinfo($fkeyval) "\"$line\""
	   }
           set do_add "[lsearch -exact $queuesSelected $fkeyval]"

	   if {$do_add != -1} {
             $jobsListbox insert end $line
    	     incr cnt

              set do_select "[lsearch -exact $jobsSelected \
		             [strget_keyvals $line $JOBS_LISTBOX_KEY "@"]]"

              if {$do_select != -1} {
	        if {$tk_version < 4.0} {
                  listbox_non_contiguous_selection $jobsListbox \
                                       [$jobsListbox curselect] end
		} else {
		  $jobsListbox selection anchor end
		  $jobsListbox selection set anchor end
		  $jobsListbox see end
		}
              }
           }
         }
       }
    }
    InfoBox_sendmsg "." 0 1
  }

  if [catch {close $f} errmesg] {
    InfoBox_sendmsg "done: $errmesg" 0 1 5
  } else {
    InfoBox_sendmsg "done." 0 1 5
  }
  remove_busy_cursor
}

# loadJobs: loads the jobs listbox with appropriate entries.
proc loadJobs {} {
  global QUEUES_LISTBOX_KEY queuesListbox jobsListbox jobinfo q_selection \
	 queuesSelected jobsSelMode
  
  $jobsListbox delete 0 end
  set jobsSelMode "Select All"
  set jobsSelected ""
  foreach el $queuesSelected {

    catch { eval $jobsListbox insert end $jobinfo($el) }
  }
  update idletasks
}

# loadQueues: loads the queues listbox with appropriate entries.
proc loadQueues {} {
  global HOSTS_LISTBOX_KEY queuesListbox hostsListbox jobsListbox qinfo \
	 h_selection hostsSelected queuesSelected queuesSelMode jobsSelected \
	 jobsSelMode

  $queuesListbox delete 0 end
  set queuesSelMode "Select All"
  set queuesSelected ""
  $jobsListbox delete 0 end
  set jobsSelMode "Select All"
  set jobsSelected ""
  foreach el $hostsSelected {
    catch { eval $queuesListbox insert end $qinfo($el) }
  }
  update idletasks
}

# Obtain detailed information about the selected host server in the HOSTS
# list box.
proc getHostsDetail {} {
  global hostsSelected PBS_HOSTS_DETAIL_CMD

  if {[string compare $hostsSelected ""] == 0} {
    popupInfoBox "" "Please select a server host(s) to query."
    return
  }
  win_cmdExec "" "$PBS_HOSTS_DETAIL_CMD $hostsSelected"
  update idletasks
}

# Obtain detailed information about the selected queue or queues in the QUEUES
# list box.
proc getQueuesDetail {} {
  global queuesSelected PBS_QUEUES_DETAIL_CMD

  if {[string compare $queuesSelected ""] == 0} {
    popupInfoBox "" "Please select a queue(s) to query."
    return
  }

  win_cmdExec "" "$PBS_QUEUES_DETAIL_CMD $queuesSelected"
  update idletasks
}

# Obtain detailed information about the selected job or jobs in the JOBS
# list box.
proc getJobsDetail {} {
  global jobsSelected PBS_JOBS_DETAIL_CMD jobsListbox JOBS_LISTBOX_KEY

  if {[string compare $jobsSelected ""] == 0} {
    popupInfoBox "" "Please select a job(s) to query."
    return
  }
  win_cmdExec "" "$PBS_JOBS_DETAIL_CMD $jobsSelected"
  update idletasks
}

proc runDelete {} {
  global jobsListbox JOBS_LISTBOX_KEY jobsSelected

  if {[string compare $jobsSelected ""] == 0} {
    popupInfoBox "" "Please select a job(s) to delete."
    return
  }
  qdel
}

proc runHold {} {
  global jobsListbox JOBS_LISTBOX_KEY jobsSelected

  if {[string compare $jobsSelected ""] == 0} {
    popupInfoBox "" "Please select a job(s) to hold."
    return
  }
  qhold
}

proc runRelease {} {
  global jobsListbox JOBS_LISTBOX_KEY jobsSelected

  if {[string compare $jobsSelected ""] == 0} {
    popupInfoBox "" "Please select a job(s) to release."
    return
  }
  qrls
}

proc runRerun {} {
  global jobsListbox JOBS_LISTBOX_KEY jobsSelected cmdpath SERVER_HOSTS

  if {[string compare $jobsSelected ""] == 0} {
    popupInfoBox "" "Please select a job(s) to requeue/rerun."
    return
  }
  set exitcode [win_cmdExec "" "$cmdpath(QRERUN) $jobsSelected"]

  if {$exitcode == 0} {
    after 500 [list getdata $SERVER_HOSTS 1]
  }
}

proc runRun {} {
  global jobsListbox JOBS_LISTBOX_KEY jobsSelected cmdpath SERVER_HOSTS

  if {[string compare $jobsSelected ""] == 0} {
    popupInfoBox "" "Please select a job(s) to run."
    return
  }
  set exitcode [win_cmdExec "" "$cmdpath(QRUN) $jobsSelected"]

  if {$exitcode == 0} {
    after 500 [list getdata $SERVER_HOSTS 1]
  }
}

proc runQsig {} {
  global jobsListbox JOBS_LISTBOX_KEY jobsSelected

  if {[string compare $jobsSelected ""] == 0} {
    popupInfoBox "" "Please select a job(s) to signal."
    return
  }
  qsig
}

proc runQmsg {} {
  global jobsListbox JOBS_LISTBOX_KEY jobsSelected

  if {[string compare $jobsSelected ""] == 0} {
    popupInfoBox "" "Please select a job(s) to send message to."
    return
  }
  qmsg
}

proc runQmove {} {
  global jobsListbox queuesListbox JOBS_LISTBOX_KEY QUEUES_LISTBOX_KEY
  global jobsSelected

  if {[string compare $jobsSelected ""] == 0} {
    popupInfoBox "" "Please select a job(s) to move into another queue."
    return
  }
  qmove
}

proc runQstop {} {
  global queuesSelected cmdpath SERVER_HOSTS

  if {[string compare $queuesSelected ""] == 0} {
    popupInfoBox "" "Please select a queue(s) to stop."
    return
  }
  set exitcode [win_cmdExec "" "$cmdpath(QSTOP) $queuesSelected"]
  update idletasks

  if {$exitcode == 0} {
    after 500 [list getdata $SERVER_HOSTS]
  }
}

proc runQstart {} {
  global queuesSelected cmdpath SERVER_HOSTS

  if {[string compare $queuesSelected ""] == 0} {
    popupInfoBox "" "Please select a queue(s) to start."
    return
  }
  set exitcode [win_cmdExec "" "$cmdpath(QSTART) $queuesSelected"]
  update idletasks

  if {$exitcode == 0} {
    after 500 [list getdata $SERVER_HOSTS]
  }
}

proc runQenable {} {
  global queuesSelected cmdpath SERVER_HOSTS

  if {[string compare $queuesSelected ""] == 0} {
    popupInfoBox "" "Please select a queue(s) to enable."
    return
  }
  set exitcode [win_cmdExec "" "$cmdpath(QENABLE) $queuesSelected"]

  if {$exitcode == 0} {
    after 500 [list getdata $SERVER_HOSTS]
  }
}

proc runQdisable {} {
  global queuesSelected cmdpath SERVER_HOSTS

  if {[string compare $queuesSelected ""] == 0} {
    popupInfoBox "" "Please select a queue(s) to disable."
    return
  }
  set exitcode [win_cmdExec "" "$cmdpath(QDISABLE) $queuesSelected"]

  if {$exitcode == 0} {
    after 500 [list getdata $SERVER_HOSTS]
  }
}

proc runQalter {} {
  global jobsListbox JOBS_LISTBOX_KEY jobsSelected


  if {[string compare $jobsSelected ""] == 0} {
    popupInfoBox "" "Please select a job(s) to modify."
    return
  }
  qalter
}

proc runQorder {} {
  global jobsListbox JOBS_LISTBOX_KEY jobsSelected cmdpath SERVER_HOSTS

  if {[llength $jobsSelected] != 2} {
    popupInfoBox "" "Please select 2 jobs to exchange positions first"
    return
  }
  set exitcode [win_cmdExec "" "$cmdpath(QORDER) $jobsSelected"]

  if {$exitcode == 0} {
    after 500 [list getdata $SERVER_HOSTS 1]
  }
}

proc runQterm {} {
  global hostsListbox HOSTS_LISTBOX_KEY hostsSelected

  if { [string compare $hostsSelected ""] == 0 } {
    popupInfoBox "" "Please select a server host(s) to shutdown."
    return
  }

  qterm
}

proc runQsub {} {
  global hostsListbox HOSTS_LISTBOX_KEY hostsSelected submitHost

  if { [string compare $hostsSelected ""] == 0 } {
    popupInfoBox "" "Please select the server host to submit job(s) to."
  } elseif { [llength $hostsSelected] != 1 } {
    serverSelect
    update idletasks
    after 250 qsub
  } else {
    set submitHost $hostsSelected
    qsub
  } 

}

# build_opt: procedure that builds the options list for submitting a PBS
# 	job. The argument 'cmdline' says that if set to 1, then command line options
# 	will be created; otherwise, PBS directive lines will be created using
# 	'pbsdir'.
# OPTIONS:
#	pbsdir - the PBS directive line parse string.
#	do_qalter - if options are built for qalter (boolean).
proc build_opt {cmdline {pbsdir "#PBS"} {do_qalter 0}} { 

  global options

  if {$do_qalter} {
     global qalterv def_qalter
     set matchDbox ".qalter"
     set ARR qalterv
     set default def_qalter
  } else {
     global qsubv def_qsub
     set matchDbox ".qsub"
     set ARR qsubv
     set default def_qsub
  }
 
  foreach el [array names options] {
    set dbox [lindex $options($el) 0]
    set optLetter [lindex $options($el) 1]
    set opt_spec  [lindex $options($el) 2]

    if { [lsearch -exact $dbox $matchDbox] != -1 && \
	 [info exists ${ARR}($el)]  && \
	 [string compare [set ${ARR}($el)] ""] != 0 && \
	 ( ![info exists ${default}($el)] || \
           [string compare [set ${ARR}($el)] [set ${default}($el)]] != 0 ) } {

	   if {![info exists opt($optLetter)]} {

	     if { [string compare $opt_spec "toggle"] == 0 } {
               append opt($optLetter) ""
	     } else {
               append opt($optLetter) "[set ${ARR}($el)]"
	     }	

	   } else {
             append opt($optLetter) "," "[set ${ARR}($el)]"
	   }
    }
  }   

  set opts "" 
  if [info exists opt] {
    foreach el [array names opt] {
      if {$cmdline} {	
        append opts "$el $opt($el) "
      } else {
        append opts "$pbsdir $el $opt($el)\n" 
      }
    }
  }
  return $opts
}
# set_opt_default: set qsub/qalter widget default options.
proc set_opt_default {array {do_main 1} {do_depend 0} {do_staging 0} \
		    {do_misc 0} {do_qalter 0} {do_email 0} } {
  upvar $array ARR

  global default

  if {!$do_qalter} {
    set def_idx 0
  } else {
    set def_idx 1
  }

  foreach el [array names default] {

    set defval [lindex $default($el) $def_idx]
    set dbox [lindex $default($el) 2]
    if { ( $do_main && \
	   [lsearch -regexp $dbox ".qsub|.qalter|.dateTime"] != -1) || \
         ( $do_depend && [lsearch -exact $dbox ".depend"] != -1 ) || \
         ( $do_staging && [lsearch -exact $dbox ".staging"] != -1 ) || \
         ( $do_misc && [lsearch -exact $dbox ".misc"] != -1 ) || \
         ( $do_email && [lsearch -exact $dbox ".email"] != -1 ) } {
         set ARR($el) $defval
    }
  }
}

# load_qsub_input: Load the widget values with input as obtained from the string 
# 'parseList'.
# RETURNS 0 if successful; 1 otherwise.
proc load_qsub_input {fd} {
    global qsubv default tk_version qsubDialogBox \
           qsubv_res qsubv_val qsubv_evar qsubv_eval qsubv_notify_addr \
	   qsubv_after_parent_id qsubv_afterok_parent_id \
	   qsubv_afternotok_parent_id qsubv_afterany_parent_id \
	   qsubv_before_parent_id qsubv_beforeok_parent_id \
	   qsubv_beforenotok_parent_id qsubv_beforeany_parent_id \
	   qsubv_stagein_host qsubv_stagein_input qsubv_stagein_local \
	   qsubv_stageout_local qsubv_stageout_host qsubv_stageout_output \
	   qsubv_shellp qsubv_shellh qsubv_groupg qsubv_grouph \
	   qsubv_useru qsubv_userh

    set attrlist ""
    set stageinList ""
    set stageoutList ""
    set userList ""
    set shellList ""
    set groupList ""
    set dependList ""
    set emailList ""
    set queueName ""
    set serverName ""

    while {[gets $fd data] >= 0} {

      
       if {[regexp "^$" $data]} { 
	  continue
       }
       if {[regexp "(\[^ ]+) = (.+)$" $data line attribute value]} {

	 switch -regexp -- $attribute {

	 "Execution_Time" {
	   set qsubv(qtime) "later" 
	   set_dateTime qsubv(qtimeMon) qsubv(qtimeDay) qsubv(qtimeYear) \
			 qsubv(qtimeHH) qsubv(qtimeMM) qsubv(qtimeSS) 0 \
			 $value
	 }

	 "Job_Name" {	
	   set qsubv(jobName) $value
	 }

	 "Priority" {
	   set qsubv(priority) [string trim $value "+"]
	 }

	 "Checkpoint" {

	   if {[string compare $value "u"] != 0} {
	     set qsubv(checkpoint_job) 1

	     if [regexp "^c=(\[^ ]+)" $value match cpu_min] {
	        set qsubv(checkpoint_option) "c="
	        set qsubv(checkpoint_min) $cpu_min
	     } else {
	        set qsubv(checkpoint_option) $value
	     }
	   }
	 }

	 "queue" {
	   set queueName $value
	 }
	 "server" {
	   set serverName $value
         }

	 "Resource_List" {

	   if {[string compare $attrlist ""] == 0} {	;# If first one
	     set attrlist $value	
	   } else {
	       append attrlist ",$value"
	   }
	 }

	 "Hold_Types" {
	   if [string match $value "u"] {
	     set qsubv(holdjob) 1
	   } else {
	     set qsubv(holdjob) 0
	   }
	 }

	 "Rerunable" {
	   if {[string compare $value ""] != 0 && \
	       [string compare $value "True"] == 0} {
	      set qsubv(rerun) 1
	      set qsubv(rerunnable) y
	   } elseif {[string compare $value ""] != 0} {
	      set qsubv(rerun) 1 
	      set qsubv(rerunnable) n
	   }
	 }

	 "Join_Path" {
	   set qsubv(merge) $value
	 }

	 "Keep_Files" {
	   if [regexp "o" $value] { 
		set qsubv(stdoutRet) o
	   }
	   if [regexp "e" $value]  { 
		set qsubv(stderrRet) e
	   }
	   if [regexp "n" $value]  { 
		set qsubv(stdoutRet) ""
		set qsubv(stderrRet) ""
	   }
	 }
	 "Output_Path" {
	   regexp "(\[^:]+):(\[^ ]+)" $value match host file
	   set qsubv(stdoutHost) $host
	   set qsubv(stdoutFile) $file
	 }
	 "Error_Path" {
	   regexp "(\[^:]+):(\[^ ]+)" $value match host file
	   set qsubv(stderrHost) $host
	   set qsubv(stderrFile) $file
	 }
	 "Export_Current_Env" {
	   set qsubv(export_current_var) $value
	 }
	 "Variable_List" {
	   set qsubv(exportVar) 1
	   load_argstr $value , {qsubv_evar qsubv_eval} {=}
	 }
	 "depend" {
	   if {[string compare $dependList ""] == 0} {	;# If first one
	     set dependList $value
	   } else {
	       append dependList ",$value"
	   }
	 }
	 "stagein" {

	   if {[string compare $stageinList ""] == 0} {	;# If first one
	     set stageinList $value	
	   } else {
	       append stageinList ",$value"
	   }
	 }
	 "stageout" {
	   if {[string compare $stageoutList ""] == 0} { ;# If first one
	     set stageoutList $value	
	   } else {
	       append stageoutList ",$value"
	   }
	 }
	 "Mail_Points" {
	   if [regexp "a" $value] { 
		set qsubv(notify_opt(abort)) "a"
	   } else {
		set qsubv(notify_opt(abort)) ""
	   }
	   if [regexp "b" $value]  { 
		set qsubv(notify_opt(begin)) "b"
	   } else {
		set qsubv(notify_opt(begin)) ""
	   }
	   if [regexp "e" $value]  { 
		set qsubv(notify_opt(end)) "e"
	   } else {
		set qsubv(notify_opt(end)) ""
	   }
	   if [regexp "n" $value]  { 
		set qsubv(notify_opt(abort)) ""
		set qsubv(notify_opt(begin)) ""
		set qsubv(notify_opt(end)) ""
	   }
	 }
	 "Mail_Users" {
	     if {[string compare $emailList ""] == 0} { ;# If first one
		set emailList $value
	     } else {
		append emailList ",$value"
	     }
	 }
	 "Shell_Path_List" {
	     if {[string compare $shellList ""] == 0} { ;# If first one
		set shellList $value
	     } else {
		append shellList ",$value"
	     }
	 }
	 "User_List" {
	     if {[string compare $userList ""] == 0} { ;# If first one
		set userList $value	
	     } else {
		append userList ",$value"
	     }
	 }
	 "group_list" {
	     if {[string compare $groupList ""] == 0} { ;# If first one
		set groupList $value
	     } else {
		append groupList ",$value"
	     }
	 }
	 "Account_Name" {
	   set qsubv(acctName) $value
	 }

	 "Buffer_File" {
##########  Load the contents
	   set qsubv(bufferFile) $value
	 }

	 } ;# switch


       } ;# if
    
    } ;# while

    if {[string compare $attrlist ""] != 0} {
	   load_argstr $attrlist , {qsubv_res qsubv_val} {=}
    }
    if {[string compare $stageinList ""] != 0} {
	   load_argstr $stageinList , \
	     {qsubv_stagein_local qsubv_stagein_host qsubv_stagein_input} {@ :}
    }
    if {[string compare $stageoutList ""] != 0} {
	   load_argstr $stageoutList , \
	     {qsubv_stageout_local qsubv_stageout_host qsubv_stageout_output} \
	     {@ :}
    } 

    if {[string compare $userList ""] != 0} {
	     load_argstr $userList , {qsubv_useru qsubv_userh} {@}
    }

    if {[string compare $shellList ""] != 0} {
	     load_argstr $shellList , {qsubv_shellp qsubv_shellh} {@}
    }

    if {[string compare $groupList ""] != 0} {
	     load_argstr $groupList , {qsubv_groupg qsubv_grouph} {@}
    }

    if {[string compare $emailList ""] != 0} {
	     load_argstr $emailList , {qsubv_notify_addr} {}
    }

    if { [string compare $dependList ""] != 0 } {
	   deconstruct_array_args $dependList dependency_list ","
	   foreach i [array names dependency_list] {

	     if [regexp "after:(.+)" $dependency_list($i) match joblist] {

	       if { [string compare $qsubv(afterJobs) ""] == 0 } {
	       		set qsubv(afterJobs) $joblist
	       } else {
			append qsubv(afterJobs) ":$joblist"
	       }
	     } elseif [regexp "afterok:(.+)" $dependency_list($i) match \
								    joblist] {

	       if { [string compare $qsubv(afterokJobs) ""] == 0 } {
	       		set qsubv(afterokJobs) $joblist
	       } else {
			append qsubv(afterokJobs) ":$joblist"
	       }
	     } elseif [regexp "afternotok:(.+)" $dependency_list($i) match \
								    joblist] {

	       if { [string compare $qsubv(afternotokJobs) ""] == 0 } {
	       		set qsubv(afternotokJobs) $joblist
	       } else {
			append qsubv(afternotokJobs) ":$joblist"
	       }
	     } elseif [regexp "afterany:(.+)" $dependency_list($i) match \
								    joblist] {

	       if { [string compare $qsubv(afteranyJobs) ""] == 0 } {
	       		set qsubv(afteranyJobs) $joblist
	       } else {
			append qsubv(afteranyJobs) ":$joblist"
	       }
	     } elseif [regexp "before:(.+)" $dependency_list($i) match \
								    joblist] {

	       if { [string compare $qsubv(beforeJobs) ""] == 0 } {
	       		set qsubv(beforeJobs) $joblist
	       } else {
			append qsubv(beforeJobs) ":$joblist"
	       }
	     } elseif [regexp "beforeok:(.+)" $dependency_list($i) match \
								    joblist] {

	       if { [string compare $qsubv(beforeokJobs) ""] == 0 } {
	       		set qsubv(beforeokJobs) $joblist
	       } else {
			append qsubv(beforeokJobs) ":$joblist"
	       }
	     } elseif [regexp "beforenotok:(.+)" $dependency_list($i) match \
								    joblist] {

	       if { [string compare $qsubv(beforenotokJobs) ""] == 0 } {
	       		set qsubv(beforenotokJobs) $joblist
	       } else {
			append qsubv(beforenotokJobs) ":$joblist"
	       }
	     } elseif [regexp "beforeany:(.+)" $dependency_list($i) match \
								    joblist] {

	       if { [string compare $qsubv(beforeanyJobs) ""] == 0 } {
	       		set qsubv(beforeanyJobs) $joblist
	       } else {
			append qsubv(beforeanyJobs) ":$joblist"
	       }
	     } elseif [regexp "on:(.+)" $dependency_list($i) match count] {
	       set qsubv(oncount) $count
	     } elseif [regexp "synccount:(.+)" $dependency_list($i) match \
								    count] {
	       set qsubv(concur) "synccount"	
	       set qsubv(synccountCnt) $count
	     } elseif [regexp "syncwith:(.+)" $dependency_list($i) match \
								    jobid] {
	       set qsubv(concur) "syncwith"	
	       set qsubv(syncwithJob) $jobid
	
	     }
	   }
    }

    if {[string compare $queueName ""] != 0} { 
	set quelist [get_keyvals $qsubv(destLbox) 0 "" " " "all"]
        if { [string compare [string index $queueName 0] "@"] != 0 } {
	  set dest_idx [lsearch -regexp $quelist "^($queueName)$|^($queueName)@"]
        } else {
	  set dest_idx [lsearch -regexp $quelist "^$queueName"]
        }
	if {[string compare $quelist ""] == 0 || $dest_idx == -1} {
	   if { [string compare $serverName ""] != 0 } {
	       $qsubv(destLbox) insert 0 $queueName@$serverName
 	   } else {
	       $qsubv(destLbox) insert 0 $queueName
           }
	   
	   if { $tk_version < 4.0 } {
	     $qsubv(destLbox) select from 0
	   } else {
 	     $qsubv(destLbox) select clear 0 end  ;# since  it is a single
						  ;# single selection listbox 
	     $qsubv(destLbox) select anchor 0
	     $qsubv(destLbox) select set anchor 0
	   }
	   $qsubv(destLbox) yview 0	
	 } else {
	   if { $tk_version < 4.0 } {
	     $qsubv(destLbox) select from $dest_idx
	   } else {
 	     $qsubv(destLbox) select clear 0 end  ;# since  it is a single
						  ;# single selection listbox 
	     $qsubv(destLbox) select anchor $dest_idx
	     $qsubv(destLbox) select set anchor $dest_idx
	   } 
	   $qsubv(destLbox) yview $dest_idx	
	     
	 }
    }
    if [catch {close $fd} errmesg] {
      popupInfoBox $qsubDialogBox "Loading of script failed: $errmesg"
      return 1
    }
    return 0
}

# oper: retains the appropriate comparison operator.
proc oper {operator} {
     switch -exact -- $operator {
        "="     {return ".eq."}
        "!="    {return ".ne."}
        ">="    {return ".ge."}
        ">"     {return ".gt."}
        "<="    {return ".le."}
        "<"     {return ".lt."}
        default {return ""}
      }
}
# oper_invert: the inverse of the 'oper'.
proc oper_invert {str} {
     switch -exact -- $str {
        "eq"     {return "="}
        "ne"    {return "!="}
        "ge"    {return ">="}
        "gt"     {return ">"}
        "le"    {return "<="}
        "lt"     {return "<"}
        default {return ""}
      }
}

# cvtdatetime_arg: converts to datetime argument suitable for PBS commands execution,
#	given the month (mon), day (day), year (year), hour (hh), minute (mm), and
#	seconds (ss) .
proc cvtdatetime_arg {mon day year hh mm ss} {

     switch -exact -- $mon {
        "Jan"     {set cmon 01}
        "Feb"     {set cmon 02}
        "Mar"     {set cmon 03}
        "Apr"     {set cmon 04}
        "May"     {set cmon 05}
        "Jun"     {set cmon 06}
        "Jul"     {set cmon 07}
        "Aug"     {set cmon 08}
        "Sep"     {set cmon 09}
        "Oct"     {set cmon 10}
        "Nov"     {set cmon 11}
        "Dec"     {set cmon 12}
	default   {set cmon ""}
     }

     if {[string length $day] < 2 && $day >= 0 && $day <= 9} {
       set cday "0$day"
     } else {
       set cday "$day"
     }

     set cyear $year  

     if {[string length $hh] < 2 && $hh >= 0 && $hh <= 9} {
       set chh "0$hh"
     } else {
       set chh "$hh"
     }
          
     if {[string length $mm] < 2 && $mm >= 0 && $mm <= 9} {
       set cmm "0$mm"
     } else {
       set cmm "$mm"
     }
  
     if {[string length $ss] < 2 && $ss >= 0 && $ss <= 9} {
       set css "0$ss"
     } else {
       set css "$ss"
     }
     return "${cyear}${cmon}${cday}${chh}${cmm}.${css}"
}

# build_sel_options: builds the options to be used for running PBS comand qselect.
proc build_sel_options {} {

  global selv select_opt

  set options ""
  foreach el [array names select_opt] {
    set val    [lindex $selv($el) 1]
    if {[string compare $val ""] != 0 && \
        [string compare $val "-ANY-"] != 0} {
      append options "$select_opt($el) $val "  
    }
  }
  return $options
}
# xpbs_help: brings up an help information box displaying things related to
# 'help_category'.
proc xpbs_help {help_category callerDialogBox} {
  global helpdir

  if { [string compare $callerDialogBox ""] != 0 } {
    win_cmdExec $callerDialogBox "cat $helpdir/${help_category}.hlp"
  } else {
    win_cmdExec "" "cat $helpdir/${help_category}.hlp"
  }
}
# resources_help: brings up an help information box displaying things related 
# to PBS resources.
proc resources_help {callerDialogBox suffix} {

  if { [string compare $callerDialogBox ""] != 0 } {
    win_cmdExec2 $callerDialogBox "man pbs_resources_$suffix | col -b"
  } else {
    win_cmdExec2 "" "man pbs_resources_$suffix | col -b"
  }
}
# set_default_qsub_main: set the default widget values for qsub main window 
proc set_default_qsub_main {} {

  global qsubv env qsubv_res qsubv_val qsubv_evar qsubv_eval tk_version \
         def_qsub

  if {$tk_version < 4.0} {
     $qsubv(destLbox) select from 0
  } else {
     $qsubv(destLbox) select clear 0 end  ;# since  it is a single
					  ;# single selection listbox 
     $qsubv(destLbox) select anchor 0
     $qsubv(destLbox) select anchor 0
     $qsubv(destLbox) select set anchor 0
  }
  $qsubv(destLbox) yview 0

  set qsubv(script_name) 	$def_qsub(script_name)
  set qsubv(jobName)		$def_qsub(jobName)
  set qsubv(acctName)		$def_qsub(acctName)
  set qsubv(destination)	$def_qsub(destination)
  set qsubv(notify_opt(begin))  $def_qsub(notify_opt\(begin\))
  set qsubv(notify_opt(end))    $def_qsub(notify_opt\(end\))

  clear_array qsubv_res
  clear_array qsubv_val
  clear_array qsubv_evar
  clear_array qsubv_eval
 
  set qsubv(pbs_prefix)		$def_qsub(pbs_prefix) 

  set qsubv(priority) 		$def_qsub(priority)
  set qsubv(qtime) 		$def_qsub(qtime)

  set qsubv(notify_opt(abort))  $def_qsub(notify_opt\(abort\))
  set qsubv(holdjob) 0
  set qsubv(export_current_var) $def_qsub(export_current_var)
  set qsubv(exportVar)          $def_qsub(exportVar)
  set qsubv(merge)		$def_qsub(merge)

  set qsubv(stdoutRet)		$def_qsub(stdoutRet)
  set qsubv(stderrRet)		$def_qsub(stderrRet)
  set qsubv(stdoutFile)		$def_qsub(stdoutFile)
  set qsubv(stdoutHost)		$def_qsub(stdoutHost)
  set qsubv(stderrFile)		$def_qsub(stderrFile)
  set qsubv(stderrHost)		$def_qsub(stderrHost)

  init_qsub_main_argstr 
}
proc init_qsub_main_argstr {} {
  global def_qsub qsubv

  set qsubv(res_args)		$def_qsub(res_args)
  set qsubv(env_args)		$def_qsub(env_args)
  set qsubv(keep_args)        	$def_qsub(keep_args)
  set qsubv(stdoutPath)		$def_qsub(stdoutPath)
  set qsubv(stderrPath)		$def_qsub(stderrPath)
  set qsubv(mail_option)	$def_qsub(mail_option)
}
proc set_default_qsub_concur {} {
  global qsubv def_qsub concurJob

  set qsubv(concur) 		$def_qsub(concur)
  set qsubv(synccountCnt)       $def_qsub(synccountCnt)	
  set qsubv(syncwithJob)	$def_qsub(syncwithJob)
  set concurJob 		$qsubv(syncwithJob)
}

proc set_default_qsub_after {} {
  global qsubv def_qsub afterJob

  set qsubv(afterJobs) 		$def_qsub(afterJobs)
  set qsubv(afterokJobs)        $def_qsub(afterokJobs)	
  set qsubv(afternotokJobs)	$def_qsub(afternotokJobs)
  set qsubv(afteranyJobs)	$def_qsub(afteranyJobs)
  set qsubv(oncount)		$def_qsub(oncount)
  set afterJob			""
}

proc set_default_qsub_before {} {
  global qsubv def_qsub beforeJob

  set qsubv(beforeJobs) 	$def_qsub(beforeJobs)
  set qsubv(beforeokJobs)       $def_qsub(beforeokJobs)	
  set qsubv(beforenotokJobs)	$def_qsub(beforenotokJobs)
  set qsubv(beforeanyJobs)	$def_qsub(beforeanyJobs)
  set beforeJob		        ""
}
proc init_qsub_depend_argstr {} {
  global qsubv def_qsub

  set qsubv(depend) $def_qsub(depend)
}
# set_default_qsub_staging: set the default widget values for the qsub staging
#			    window 
proc set_default_qsub_staging {} {
  global qsubv qsubv_stagein_host qsubv_stagein_input qsubv_stagein_local \
	 qsubv_stageout_local qsubv_stageout_host qsubv_stageout_output \
	 def_qsub

  clear_array qsubv_stagein_host
  clear_array qsubv_stagein_input
  clear_array qsubv_stagein_local
  clear_array qsubv_stageout_local
  clear_array qsubv_stageout_host
  clear_array qsubv_stageout_output

  init_qsub_staging_argstr
}
proc init_qsub_staging_argstr {} {
  global qsubv def_qsub

  set qsubv(stagein_filelist)    $def_qsub(stagein_filelist)
  set qsubv(stageout_filelist)   $def_qsub(stageout_filelist)
}
# set_default_qsub_misc: set the default widget values for the qsub misc
#			 window 
proc set_default_qsub_misc {} {
  global qsubv qsubv_shellp qsubv_shellh qsubv_groupg qsubv_grouph \
	 qsubv_useru qsubv_userh def_qsub
  
  set qsubv(checkpoint_job)     $def_qsub(checkpoint_job)
  set qsubv(checkpoint_option)  $def_qsub(checkpoint_option)
  set qsubv(rerun) 		$def_qsub(rerun)
  set qsubv(rerunnable)		$def_qsub(rerunnable)

  set qsubv(checkpoint_min) 	$def_qsub(checkpoint_min)

  clear_array qsubv_shellp
  clear_array qsubv_shellh
  clear_array qsubv_groupg
  clear_array qsubv_grouph
  clear_array qsubv_useru
  clear_array qsubv_userh

  init_qsub_misc_argstr
}
proc init_qsub_misc_argstr {} {
  global qsubv def_qsub

  set qsubv(checkpoint_arg)     $def_qsub(checkpoint_arg)
  set qsubv(rerun_args)         $def_qsub(rerun_args)
  set qsubv(group_args)	        $def_qsub(group_args)
  set qsubv(shell_args)		$def_qsub(shell_args)
  set qsubv(user_args)          $def_qsub(user_args) 
}
# set_default_qsub_datetime: set the default widget values for the qsub
#				datetime window 
proc set_default_qsub_datetime {} {
  global qsubv def_qsub

  set qsubv(qtimeMon)           $def_qsub(qtimeMon)
  set qsubv(qtimeDay)           $def_qsub(qtimeDay)
  set qsubv(qtimeYear)          $def_qsub(qtimeYear)
  set qsubv(qtimeHH)            $def_qsub(qtimeHH)
  set qsubv(qtimeMM)            $def_qsub(qtimeMM)
  set qsubv(qtimeSS)            $def_qsub(qtimeSS)

  init_qsub_datetime_argstr
}
proc init_qsub_datetime_argstr {} {
  global qsubv def_qsub

  set qsubv(exec_time)          $def_qsub(exec_time)
}
# set_default_qsub_email: set the default widget values for the qsub email
#			window 
proc set_default_qsub_email {} {
  global qsubv qsubv_notify_addr def_qsub

  clear_array qsubv_notify_addr

  init_qsub_email_argstr
  load_argstr $qsubv(email_args) , {qsubv_notify_addr} {}
}
proc init_qsub_email_argstr {} {
  global qsubv def_qsub qsubv_notify_addr

  set qsubv(email_args)    $def_qsub(email_args)
}
# set_default_qalter_main: set the default widget values for the qalter main
#			window 
proc set_default_qalter_main {} {
  global qalterv qalterv_res qalterv_val def_qalter

  set qalterv(jobName)            $def_qalter(jobName)
  set qalterv(priority)		  $def_qalter(priority)
  set qalterv(acctName) 	  $def_qalter(acctName)
  set qalterv(destination)	  $def_qalter(destination)
  
  set qalterv(priority) 	  $def_qalter(priority)
  set qalterv(qtime) 		  $def_qalter(qtime)
  set qalterv(notify_opt(abort))  $def_qalter(notify_opt\(abort\))
  set qalterv(notify_opt(begin))  $def_qalter(notify_opt\(begin\))
  set qalterv(notify_opt(end))    $def_qalter(notify_opt\(end\))
  set qalterv(notify)		  $def_qalter(notify)
  set qalterv(hold_action)	  $def_qalter(hold_action)
  set qalterv(merge)		  $def_qalter(merge)
  set qalterv(retain)		  $def_qalter(retain)

  set qalterv(qhold(user))	  $def_qalter(qhold\(user\))
  set qalterv(qhold(other))	  $def_qalter(qhold\(other\))
  set qalterv(qhold(system))	  $def_qalter(qhold\(system\))
  set qalterv(stdoutRet)	  $def_qalter(stdoutRet)
  set qalterv(stderrRet)	  $def_qalter(stderrRet)
  set qalterv(stdoutFile)	  $def_qalter(stdoutFile)
  set qalterv(stdoutHost)	  $def_qalter(stdoutHost)
  set qalterv(stderrFile)	  $def_qalter(stderrFile)
  set qalterv(stderrHost)	  $def_qalter(stderrHost)
  
  clear_array qalterv_res
  clear_array qalterv_val

  init_qalter_main_argstr
}
proc init_qalter_main_argstr {} {
  global qalterv def_qalter

  set qalterv(res_args)		  $def_qalter(res_args)
  set qalterv(keep_args)	  $def_qalter(keep_args)
  set qalterv(stdoutPath)	  $def_qalter(stdoutPath)
  set qalterv(stderrPath)	  $def_qalter(stderrPath)
  set qalterv(hold_args)	  $def_qalter(hold_args)
  set qalterv(mail_option)	  $def_qalter(mail_option)
}
# set_default_qalter_concur: set the default widget values for the qalter
#		Concur window 
proc set_default_qalter_concur {} {
  global qalterv def_qalter concurJob

  set qalterv(concur)		  $def_qalter(concur)
  set qalterv(synccountCnt)       $def_qalter(synccountCnt)
  set qalterv(syncwithJob)        $def_qalter(syncwithJob)
  set qalterv(oncount)		  $def_qalter(oncount)
  set concurJob			  $qalterv(syncwithJob)

}

# set_default_qalter_after: set the default widget values for the qalter
#		After Depend window 
proc set_default_qalter_after {} {
  global qalterv def_qalter afterJob

  set qalterv(afterJobs)        $def_qalter(afterJobs)
  set qalterv(afterokJobs)      $def_qalter(afterokJobs)
  set qalterv(afternotokJobs)   $def_qalter(afternotokJobs)
  set qalterv(afteranyJobs)     $def_qalter(afteranyJobs)
  set qalterv(oncount)		$def_qalter(oncount)
  set afterJob			""
}

# set_default_qalter_before: set the default widget values for the qalter
#		Before Depend window 
proc set_default_qalter_before {} {
  global qalterv def_qalter beforeJob

  set qalterv(beforeJobs)        $def_qalter(beforeJobs)
  set qalterv(beforeokJobs)      $def_qalter(beforeokJobs)
  set qalterv(beforenotokJobs)   $def_qalter(beforenotokJobs)
  set qalterv(beforeanyJobs)     $def_qalter(beforeanyJobs)
  set beforeJob			""
}

proc init_qalter_depend_argstr {} {
  global qalterv def_qalter

  set qalterv(depend)		  $def_qalter(depend)
}
# set_default_qalter_staging: set the default widget values for the qalter
#			staging window 
proc set_default_qalter_staging {} {
  global qalterv qalterv_stagein_host qalterv_stagein_input \
	 qalterv_stagein_local qalterv_stageout_local qalterv_stageout_host \
	 qalterv_stageout_output def_qalter


  clear_array qalterv_stagein_host
  clear_array qalterv_stagein_input
  clear_array qalterv_stagein_local
  clear_array qalterv_stageout_local
  clear_array qalterv_stageout_host
  clear_array qalterv_stageout_output

  init_qalter_staging_argstr
}
proc init_qalter_staging_argstr {} {
  global qalterv def_qalter

  set qalterv(stagein_filelist)    $def_qalter(stagein_filelist)
  set qalterv(stageout_filelist)   $def_qalter(stageout_filelist)
}
# set_default_qalter_misc: set the default widget values for the qalter misc
#			window 
proc set_default_qalter_misc {} {
  global qalterv qalterv_shellp qalterv_shellh qalterv_groupg qalterv_grouph \
	 qalterv_useru qalterv_userh def_qalter

  set qalterv(checkpoint_job) 	 $def_qalter(checkpoint_job)
  set qalterv(checkpoint_option) $def_qalter(checkpoint_option) 
  set qalterv(rerun)		 $def_qalter(rerun)
  set qalterv(rerunnable)	 $def_qalter(rerunnable)

  set qalterv(checkpoint_min)	 $def_qalter(checkpoint_min)


  clear_array qalterv_shellp
  clear_array qalterv_shellh
  clear_array qalterv_groupg
  clear_array qalterv_grouph
  clear_array qalterv_useru
  clear_array qalterv_userh

  init_qalter_misc_argstr
}
proc init_qalter_misc_argstr {} {
  global qalterv def_qalter

  set qalterv(checkpoint_arg)	 $def_qalter(checkpoint_arg)
  set qalterv(rerun_args)	 $def_qalter(rerun_args)
  set qalterv(group_args) 	 $def_qalter(group_args)
  set qalterv(shell_args) 	 $def_qalter(shell_args)
  set qalterv(user_args)         $def_qalter(user_args) 
}
# set_default_qalter_datetime: set the default widget values for the qalter
#			datetime window 
proc set_default_qalter_datetime {} {
  global qalterv def_qalter

  set qalterv(qtimeMon)           $def_qalter(qtimeMon)
  set qalterv(qtimeDay)           $def_qalter(qtimeDay)
  set qalterv(qtimeYear)          $def_qalter(qtimeYear)
  set qalterv(qtimeHH)            $def_qalter(qtimeHH)
  set qalterv(qtimeMM)            $def_qalter(qtimeMM)
  set qalterv(qtimeSS)            $def_qalter(qtimeSS)

  init_qalter_datetime_argstr
}
proc init_qalter_datetime_argstr {} {
  global qalterv def_qalter

  set qalterv(exec_time)          $def_qalter(exec_time)
}
# set_default_qalter_email: set the default widget values for the qalter email
#			window 
proc set_default_qalter_email {} {
  global USER qalterv_notify_addr def_qalter

  clear_array qalterv_notify_addr
  init_qalter_email_argstr
}
proc init_qalter_email_argstr {} {
  global qalterv def_qalter

  set qalterv(email_args)    $def_qalter(email_args)
}
proc set_pbs_commands {} {

  global PBS_HOSTS_DETAIL_CMD PBS_QUEUES_DETAIL_CMD PBS_JOBS_DETAIL_CMD \
	 PBS_QSTATDUMP_CMD LINES_TO_IGNORE PBS_SCRIPTLOAD_CMD cmdpath \
	 bindir xpbs_datadump xpbs_scriptload

# COMMANDS
  set PBS_HOSTS_DETAIL_CMD "qstat -B -f"
  set PBS_QUEUES_DETAIL_CMD "qstat -Q -f"
  set PBS_JOBS_DETAIL_CMD "qstat -f"
  if [file exists $bindir/$xpbs_datadump] {
  	set PBS_QSTATDUMP_CMD "$bindir/$xpbs_datadump"
  } else {
  	set PBS_QSTATDUMP_CMD "./Ccode/$xpbs_datadump"
  }
  set LINES_TO_IGNORE "Unknown Job Id"
  if [file exists $bindir/$xpbs_scriptload] {
  	set PBS_SCRIPTLOAD_CMD "$bindir/$xpbs_scriptload"
  } else {
  	set PBS_SCRIPTLOAD_CMD "./Ccode/$xpbs_scriptload"
  }
  set cmdpath(QRUN) "qrun"
  set cmdpath(QRERUN) "qrerun"
  set cmdpath(QSIG) "qsig"
  set cmdpath(QMSG) "qmsg"
  set cmdpath(QMOVE) "qmove"
  set cmdpath(QRLS) "qrls"
  set cmdpath(QHOLD) "qhold"
  set cmdpath(QDEL) "qdel"
  set cmdpath(QSUB) "qsub"
  set cmdpath(QALTER) "qalter"
  set cmdpath(QSTOP) "qstop"
  set cmdpath(QSTART) "qstart"
  set cmdpath(QDISABLE) "qdisable"
  set cmdpath(QENABLE) "qenable"
  set cmdpath(QTERM) "qterm"
  set cmdpath(QORDER) "qorder"
}
##################  OPTIONS LIST  ###########################################
#   FORMAT: options(name) = { {.qsub .qalter} optionLetter <option-specif> }
#                    or
#           options(name) = { {.qsub .qalter} optionLetter <option-specif>}
# NOTE: You can put things "toggle" in option-specific that says it a non-arg
# option if set.
#############################################################################
proc set_pbs_options {} {
  global options select_opt

  set options(jobName)            { {.qsub .qalter} "-N" }
  set options(priority)           { {.qsub .qalter} "-p" }
  set options(acctName)           { {.qsub .qalter} "-A" }
  set options(destination)        { {.qsub} "-q" }
  set options(exec_time)          { {.qsub .qalter} "-a" }
  set options(holdjob)            { {.qsub} "-h" "toggle" }
  set options(checkpoint_arg)     { {.qsub .qalter} "-c" }
  set options(rerun_args)         { {.qsub .qalter} "-r" }
  set options(res_args)           { {.qsub .qalter} "-l" }
  set options(merge)              { {.qsub .qalter} "-j" }
  set options(keep_args)          { {.qsub .qalter} "-k" }
  set options(stdoutPath)         { {.qsub .qalter} "-o" }
  set options(stderrPath)         { {.qsub .qalter} "-e" }
  set options(export_current_var) { {.qsub} "-V" "toggle" }
  set options(env_args)           { {.qsub} "-v" }
  set options(hold_args)          { {.qalter} "-h" }
  set options(depend)             { {.qsub .qalter} "-W" }
  set options(stagein_filelist)   { {.qsub .qalter} "-W" }
  set options(stageout_filelist)  { {.qsub .qalter} "-W" }
  set options(group_args)         { {.qsub .qalter} "-W" }
  set options(mail_option)        { {.qsub .qalter} "-m" }
  set options(email_args)         { {.qsub .qalter} "-M" }
  set options(shell_args)         { {.qsub .qalter} "-S" }
  set options(user_args)          { {.qsub .qalter} "-u" }

  ### FORMAT: select_args(argname) {default_value option}
  set select_opt(owners_list) "-u"
  set select_opt(states)      "-s"
  set select_opt(reslist)     "-l"
  set select_opt(exec_time)   "-a"
  set select_opt(acctname)    "-A"
  set select_opt(checkpoint)  "-c"
  set select_opt(hold_list)   "-h"
  set select_opt(priority)    "-p"
  set select_opt(rerun)       "-r"
  set select_opt(jname)       "-N"
}
proc set_pbs_defaults {} {
  global env def_qsub def_qalter USER

  set def_qsub(script_name) 	   ""
  set def_qsub(jobName)		   ""
  set def_qsub(acctName)	   ""
  set def_qsub(destination)	   ""
  set def_qsub(notify_opt(begin))  ""
  set def_qsub(notify_opt(end))    ""

  if {[info exists env(PBS_DPREFIX)] && \
     [string compare $env(PBS_DPREFIX)  ""] != 0} {
    set def_qsub(pbs_prefix) 	$env(PBS_DPREFIX)
  } else {
    set def_qsub(pbs_prefix) 	"#PBS"
  }
  set def_qsub(priority) 	0
  set def_qsub(qtime) 		now

  set def_qsub(notify_opt(abort))  a
  set def_qsub(holdjob) 0
  set def_qsub(export_current_var) 0
  set def_qsub(exportVar)          0
  set def_qsub(merge)		   n

  set def_qsub(stdoutRet)	""
  set def_qsub(stderrRet)	""
  set def_qsub(stdoutFile)	""
  set def_qsub(stdoutHost)	""
  set def_qsub(stderrFile)	""
  set def_qsub(stderrHost)	""

  set def_qsub(res_args)	""
  set def_qsub(env_args)	""
  set def_qsub(keep_args)       ""
  set def_qsub(stdoutPath)	""
  set def_qsub(stderrPath)	""
  set def_qsub(mail_option)	""
  
  set def_qsub(concur) 		"syncnone"
  set def_qsub(synccountCnt)	""
  set def_qsub(syncwithJob)	""
  set def_qsub(afterJobs)       ""
  set def_qsub(afterokJobs)     ""
  set def_qsub(afternotokJobs)  ""
  set def_qsub(afteranyJobs)    ""
  set def_qsub(afterJob)	""
  set def_qsub(beforeJobs)      ""
  set def_qsub(beforeokJobs)    ""
  set def_qsub(beforenotokJobs) ""
  set def_qsub(beforeanyJobs)   ""
  set def_qsub(oncount)		""
  set def_qsub(depend)	      ""

  set def_qsub(stagein_filelist)    ""
  set def_qsub(stageout_filelist)   ""

  set def_qsub(checkpoint_job) 0
  set def_qsub(checkpoint_option) "c="
  set def_qsub(rerun) 1
  set def_qsub(rerunnable) y

  set def_qsub(checkpoint_min) ""

  set def_qsub(checkpoint_arg)     ""
  set def_qsub(rerun_args)         "y"
  set def_qsub(group_args)         "" 
  set def_qsub(shell_args)         "" 
  set def_qsub(user_args)          "" 

  set def_qsub(qtimeMon)           "Jan"
  set def_qsub(qtimeDay)           "01"
  set def_qsub(qtimeYear)          "1970"
  set def_qsub(qtimeHH)            "00"
  set def_qsub(qtimeMM)            "00"
  set def_qsub(qtimeSS)            "00"
  set def_qsub(exec_time)          "197001010000.00"

  set def_qsub(email_args)    	  $USER

  set def_qalter(jobName)          ""
  set def_qalter(priority)	   ""
  set def_qalter(acctName) 	   ""	
  set def_qalter(destination)	   ""
  
  set def_qalter(priority) 	   ""
  set def_qalter(qtime) 	   now
  set def_qalter(notify_opt(abort))  ""
  set def_qalter(notify_opt(begin))  ""
  set def_qalter(notify_opt(end))    ""
  set def_qalter(notify)	     0
  set def_qalter(hold_action)	  place	
  set def_qalter(merge)		  nc
  set def_qalter(retain)		  retain

  set def_qalter(qhold(user))	  ""
  set def_qalter(qhold(other))	  ""
  set def_qalter(qhold(system))	  ""
  set def_qalter(stdoutRet)	  ""
  set def_qalter(stderrRet)	  ""
  set def_qalter(stdoutFile)	  ""
  set def_qalter(stdoutHost)	  ""
  set def_qalter(stderrFile)	  ""
  set def_qalter(stderrHost)	  ""
  
  set def_qalter(res_args)	  ""
  set def_qalter(keep_args)	  ""
  set def_qalter(stdoutPath)	  ""
  set def_qalter(stderrPath)	  ""
  set def_qalter(hold_args)	  ""
  set def_qalter(mail_option)	  ""

  set def_qalter(concur)	 	"syncnone"
  set def_qalter(afterJobs)      	""
  set def_qalter(afterokJobs)    	""
  set def_qalter(afternotokJobs) 	""
  set def_qalter(afteranyJobs)   	""
  set def_qalter(beforeJobs)     	""
  set def_qalter(beforeokJobs)   	""
  set def_qalter(beforenotokJobs)	""
  set def_qalter(beforeanyJobs)  	""

  set def_qalter(synccountCnt)        ""
  set def_qalter(syncwithJob)         ""
  set def_qalter(oncount)	      ""

  set def_qalter(depend)	      ""

  set def_qalter(stagein_filelist)    ""
  set def_qalter(stageout_filelist)   ""

  set def_qalter(checkpoint_job) 0
  set def_qalter(checkpoint_option) "c="
  set def_qalter(rerun) 0
  set def_qalter(rerunnable) y

  set def_qalter(checkpoint_min) ""

  set def_qalter(checkpoint_arg)     ""
  set def_qalter(rerun_args)         ""
  set def_qalter(group_args)         "" 
  set def_qalter(shell_args)         "" 
  set def_qalter(user_args)          "" 

  set def_qalter(qtimeMon)           "Jan"
  set def_qalter(qtimeDay)           "01"
  set def_qalter(qtimeYear)          "1970"
  set def_qalter(qtimeHH)            "00"
  set def_qalter(qtimeMM)            "00"
  set def_qalter(qtimeSS)            "00"
  set def_qalter(exec_time)          "197001010000.00"

  set def_qalter(email_args)         ""
}
proc about {} {
 
  global bitmap_dir LABELFONT aboutDialogBox XPBS_VERSION

  set aboutDialogBox ".about"
  set dbox [popupDialogBox $aboutDialogBox "About..."]
  set dbox_top    [lindex $dbox 0]
  set dbox_bottom [lindex $dbox 1]


  label $dbox_top.l -bitmap @$bitmap_dir/logo.bmp
  message $dbox_top.m -font $LABELFONT -justify center -aspect 1000 -text "


XPBS $XPBS_VERSION


written by Albeaus Bayucan


The GUI toolkit used is Tcl/Tk.

For more information about PBS, please visit us at:
  
	http://www.openpbs.org"

  set buttons [buildCmdButtons $dbox_bottom {{{ok ok} {notes "release notes"}}}\
			x 20m 14 3]
  set ok_button [lindex $buttons 1]
  set notes_button [lindex $buttons 2]

  $ok_button configure -command [list destroy $aboutDialogBox]
  $notes_button configure -command {xpbs_help notes $aboutDialogBox}

  register_default_action $aboutDialogBox $ok_button
  pack $dbox_top.l $dbox_top.m -side left -anchor nw
  catch {tkwait window $aboutDialogBox}

}

# pre_build_depend_opt: builds the dependency options suitable for processing
#	by qsub and qalter. RETURNS: 1 if an error is detected; 0 otherwise.
proc pre_build_depend_opt {array def_array} {
    upvar $array ARR
    upvar $def_array DEF

    global dependDialogBox

    if [regexp "qsub" $array] {
      init_qsub_depend_argstr
    } else {
      init_qalter_depend_argstr
    }

    set j 0
    switch -exact -- $ARR(concur) {
      synccount {
        if {[string compare $ARR(synccountCnt) $DEF(synccountCnt)] != 0} {
                set dependency_list($j) "synccount:$ARR(synccountCnt)"
    		incr j
        }
      }
      syncwith {
          if {[string compare $ARR(syncwithJob) $DEF(syncwithJob)] != 0} {
                set dependency_list($j) "syncwith:$ARR(syncwithJob)"
		incr j	
          }
      }
    }
    if {[string compare $ARR(afterJobs) $DEF(afterJobs)] != 0} {
	set dependency_list($j) "after:$ARR(afterJobs)"
        incr j
    }

    if {[string compare $ARR(afterokJobs) $DEF(afterokJobs)] != 0} {
          set dependency_list($j) "afterok:$ARR(afterokJobs)"
          incr j
    }

    if {[string compare $ARR(afternotokJobs) $DEF(afternotokJobs)] != 0} { 
          set dependency_list($j) "afternotok:$ARR(afternotokJobs)"
          incr j
    }

    if {[string compare $ARR(afteranyJobs) $DEF(afteranyJobs)] != 0} { 
          set dependency_list($j) "afterany:$ARR(afteranyJobs)"
    	  incr j
    }

    if {[string compare $ARR(oncount) $DEF(oncount)] != 0} {
        set dependency_list($j) "on:$ARR(oncount)"
	incr j
    }

    if {[string compare $ARR(beforeJobs) $DEF(beforeJobs)] != 0} {
	set dependency_list($j) "before:$ARR(beforeJobs)"
        incr j
    }

    if {[string compare $ARR(beforeokJobs) $DEF(beforeokJobs)] != 0} {
          set dependency_list($j) "beforeok:$ARR(beforeokJobs)"
          incr j
    }

    if {[string compare $ARR(beforenotokJobs) $DEF(beforenotokJobs)] != 0} { 
          set dependency_list($j) "beforenotok:$ARR(beforenotokJobs)"
          incr j
    }

    if {[string compare $ARR(beforeanyJobs) $DEF(beforeanyJobs)] != 0} { 
          set dependency_list($j) "beforeany:$ARR(beforeanyJobs)"
    	  incr j
    }

    set ARR(depend) [construct_array_args dependency_list "," "depend="] 
    catch {unset dependency_list}
    return 0
}
