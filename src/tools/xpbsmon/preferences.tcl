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
################################################################################
# This file contains a set of procedures for defining and manipulating 
# X resources for xpbs. This is a piece code taken from Brent B. Welch's,
# "Practical Programming in Tcl and Tk", Prentice Hall, 1995, pp. 329-338.
proc Pref_Init { userDefaults appDefaults } {
  global pref

  set pref(uid) 0; # for a unique identifier for widgets
  set pref(userDefaults) $userDefaults
  set pref(appDefaults) $appDefaults

  PrefReadFile $appDefaults startup
  if [file exists $userDefaults] {
    PrefReadFile $userDefaults user
  }
}
proc PrefReadFile {basename level} {
  if [catch {option readfile $basename $level} err] {
    InfoBox_sendmsg "Error in $basename: $err" 1
  }
 
  if { [catch {tk colormodel .}] == 0 } {
    if { [string compare [tk colormodel .] "color"] == 0 } {
      if [file exists $basename-color] {
        if [catch {option $basename-color $level} err] {
	   InfoBox_sendmsg "Error in $basename-color: $err" 1
        }	
      }
    } else {
      if [file exists $basename-mono] {
        if [catch {option readfile $basename-mono $level} err] {
           InfoBox_sendmsg "Error in $basename-mono: $err" 1
        }
      }
    }
  }
}
proc PrefVar { item } { lindex $item 0 }
proc PrefXres { item } { lindex $item 1 }
proc PrefDefault { item } { lindex $item 2 }
proc PrefComment { item } { lindex $item 3 }
proc PrefHelp { item } { lindex $item 4 }

proc Pref_Add { prefs } {

  global pref init
  append pref(items) $prefs
  foreach item $prefs {
    set varName [PrefVar $item]
    set xresName [PrefXres $item]
    set value [PrefValue $varName $xresName]

    if {$value == {}} {
      set default [PrefDefault $item]
      if {[llength $default] > 1} {
	if { [string compare [lindex $default 0] "CHOICE"] == 0 } {
          PrefValueSet $varName [lindex $default 1]
        } else {
	  PrefValueSet $varName $default
        }
      } else {
	if {[string compare $default "OFF"] == 0} {
	  PrefValueSet $varName 0
	} elseif {[string compare $default "ON"] == 0} {
	  PrefValueSet $varName 1
        } else {
	  PrefValueSet $varName $default
        }
      }
    }
    set init($xresName) [PrefValue $varName $xresName]
  }
}

# PrefValue returns the X resource database value.
proc PrefValue { varName xres } {
  upvar #0 $varName var
  if [info exists var] {
    return $var
  }
  set var [option get . $xres {}]
}
# PrefValueSet defines a variable in the global scope.
proc PrefValueSet { varName value } {
  upvar #0 $varName var
  set var $value
}
proc PrefSave {} {
  global pref preferences

  if [catch { 
	set old [open $pref(userDefaults) r]
	set oldValues [split [read $old] \n]
	close $old
  }] {
    set oldValues {}
  }

  if [catch {open $pref(userDefaults).new w} out] {
    InfoBox_sendmsg "Cannot save in $pref(userDefaults).new: $out" 1
    return 
  }
  foreach line $oldValues {
    if {[string compare $line \
	  "!!! Lines below here are automatically added"] == 0} {
	break
    } else {
        puts $out $line
    }
  }
  puts $out "!!! Lines below here are automatically added"
  puts $out "!!! [exec date]"
  foreach item $preferences {
    set varName [PrefVar $item]
    set xresName [PrefXres $item]
    set value [PrefValue $varName $xresName]
    puts $out [format "%s\t%s" *${xresName}: $value]
  }
  close $out
  set new [glob $pref(userDefaults).new]
  set old [file root $new]
  if [catch {exec mv $new $old} err] {
    InfoBox_sendmsg "Cannot install $new: $err"
  }
}
# prefDoIt: returns 1 if a change in the list of preference parameters is
#           detected and thus, necessitating saving current settings in
#           some preferences file.   
proc prefDoIt {} {
  global init preferences

  foreach pref $preferences {
    set varName [PrefVar $pref]
    set resName [PrefXres $pref]
    upvar #0 $varName var
    if {[string compare $resName "sitesInfo"] == 0} {
	  if {[listcomp $var $init($resName)] != 0} {
      		flush stdout
      		return 1
	  }
    } else {
	  if {[string compare $var $init($resName)] != 0} {
      		flush stdout
      		return 1
	  }	
    }
  }
  return 0
}

# prefLoadSitesInfo: any badly formed sites info is automatically ignored.
proc prefLoadSitesInfo {} {
	global sitesinfo sysview sysnodes queryTable sysinfo

	foreach s $sitesinfo {
		set s_list [split $s $sysinfo(rcSiteInfoDelimeterChar)]

		if {[llength $s_list] != 7} {
			continue	;# badly-formed lines   
		}
		set sitename [lindex $s_list 0]

		set sitevtype [lindex $s_list 1]
		if {[string compare $sitevtype "ICON"] != 0 && \
		    	[string compare $sitevtype "FULL"] != 0} {
			continue
		}
		set servername [lindex $s_list 2]
		set serverlabel [lindex $s_list 3]
		set nodename [lindex $s_list 4]

		if {[string compare $nodename ""] == 0} {
			set sysview($sitename) $sitevtype
			set sysnodes($sitename,$servername) $serverlabel
			continue
		}

		set nodetype [lindex $s_list 5]
		if {[string compare $nodetype "MOM"] != 0 && \
		  	[string compare $nodetype "NOMOM"] != 0} {
			continue
		}
		set querylist [lindex $s_list 6]

		set sysview($sitename) $sitevtype
		if {![info exists sysnodes($sitename,$servername)]} {
			lappend sysnodes($sitename,$servername) \
					     $serverlabel "$nodename $nodetype"
		} else {
			set newval [lreplace $sysnodes($sitename,$servername) \
							       0 0 $serverlabel]
			set foundMatch 0
			foreach v [lrange $newval 1 end] {
				set ninfo [join $v " "]
				set newninfo "$nodename $nodetype"
				if {[string compare $ninfo $newninfo] == 0} {
					set foundMatch 1
					break
				}
			}
			if {!$foundMatch} {
				lappend newval "$nodename $nodetype"
			}
			set sysnodes($sitename,$servername) $newval
		}
		foreach qi $querylist {
			set queryexpr [list [lindex $qi 0]]
			set querylabel [list [lindex $qi 1]]
			set querytype [list [lindex $qi 2]]

			if {[string compare $querytype "SCALE"] != 0 && \
			    [string compare $querytype "TEXT"] != 0 } {
				continue
			}

			if {![info exists queryTable($sitename,$nodename)]} { 
				lappend queryTable($sitename,$nodename) \
			 	      	     "$queryexpr $querylabel $querytype"
			} else {
				set foundMatch 0
				foreach q  $queryTable($sitename,$nodename) {
					set qexpr [lindex $q 0]
					set qtype [lindex $q 2]
					set qinfo "$qexpr $qtype"
					set newqinfo "$queryexpr $querytype"
					if {[string compare $qinfo $newqinfo] \
									 == 0} {
						set foundMatch 1
						break
					}
				}
			  	if {!$foundMatch} {
				      lappend  queryTable($sitename,$nodename) \
			 	             "$queryexpr $querylabel $querytype"
			  	}
			}
		}
	}
}
proc prefSaveSitesInfo {} {
	global sysview sysnodes queryTable sitesinfo sysinfo

	set SEP $sysinfo(rcSiteInfoDelimeterChar)

	set i 0
	foreach s [array names sysview] {

		foreach srv [array names sysnodes] {
			set srv_list [split $srv ","]
			set sitename [lindex $srv_list 0]
			set srvname  [lindex $srv_list 1]
			set srvlabel [lindex $sysnodes($srv) 0]
			set srvnodes [lrange $sysnodes($srv) 1 end]
			if {[string compare $s $sitename] == 0} {
				foreach ninfo $srvnodes {
					set nname [lindex $ninfo 0]
					set ntype [lindex $ninfo 1]
					if [info exists queryTable($s,$nname)] {
						set line($i) "$s$SEP$sysview($s)$SEP$srvname$SEP$srvlabel$SEP$nname$SEP$ntype$SEP$queryTable($s,$nname)"
					} else {
						set line($i) "$s$SEP$sysview($s)$SEP$srvname$SEP$srvlabel$SEP$nname$SEP$ntype$SEP"
					}
					incr i
				}
				if {[llength $srvnodes] == 0} {
					set line($i) "$s$SEP$sysview($s)$SEP$srvname$SEP$srvlabel$SEP$SEP$SEP"
					incr i
				}
			}
		}
	}

	set sitesinfo ""
	for {set i 0} {$i < [array size line]} {incr i} {
		lappend sitesinfo $line($i)
	}
}
