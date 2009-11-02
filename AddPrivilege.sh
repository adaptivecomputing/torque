#!/bin/bash
###   torque-2.4.2b1-cygwin  ver 0.8   26 oct 2009   ###

mkpasswd -l > /etc/passwd
mkgroup -l -u > /etc/group

editrights -a SeAssignPrimaryTokenPrivilege -u $1 
editrights -a SeCreateTokenPrivilege -u $1 
editrights -a SeTcbPrivilege -u $1            
editrights -a SeIncreaseQuotaPrivilege -u $1 
editrights -a SeServiceLogonRight -u $1 
