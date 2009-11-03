#!/bin/bash
###   torque-2.5-cygwin  ver 1.0   03 nov 2009   ###

mkpasswd -l > /etc/passwd
mkgroup -l -u > /etc/group

editrights -a SeAssignPrimaryTokenPrivilege -u $1 
editrights -a SeCreateTokenPrivilege -u $1 
editrights -a SeTcbPrivilege -u $1            
editrights -a SeIncreaseQuotaPrivilege -u $1 
editrights -a SeServiceLogonRight -u $1 

if [ ! -d "/dev" ]; then
  mkdir "/dev"
fi
