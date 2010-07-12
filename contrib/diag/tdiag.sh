#!/bin/bash

#
#
# TORQUE diagnostic script - grabs and tars information about TORQUE
# 
# written by David Beer - Adaptive Computing
#
#

# initial setup of variables

torque_home="/var/spool/torque"
today=`date +%Y%m%d`
tmpdir="/tmp"
outdir="tdiag"
outtmpdir="$tmpdir/$outdir"
outfile="torque_diag$today.tar.gz"
helpme=""

while getopts "d:ho:t:" flag; do
  case $flag in
    d ) today=$OPTARG;;
    h ) helpme="a";;
    o ) outfile=$OPTARG;;
    t ) torque_home=$OPTARG;;
  esac
done

if [[ -n $helpme ]]; then
  echo "USAGE: ./tdiag [-d DATE] [-h] [-o OUTPUT_FILE] [-t TORQUE_HOME]"
  echo ""
  echo "DATE should be in the format YYYYmmdd, for example"
  echo "  20091130 would be the date for November 30th, 2009"
  echo "  If no date is specified, today's date is used"
  echo ""
  echo "Specifying -h leads to the help output, i.e. this information"
  echo ""
  echo "OUTPUT_FILE is the optional name of the output file"
  echo "  The default output file will be torque_diag<Today's date>.tar.gz,"
  echo "  with Today's date being in the format YYYYmmdd as explained above"
  echo "  TORQUE_HOME should be the path to your TORQUE directory"
  echo "  If no directory is speecified, /var/spool/torque as this is the default"
  echo ""
  echo "Remember, this command needs to be run by a user with access to all"
  echo "TORQUE directories and the ability to run TORQUE commands"
  exit
fi

mom_logs="$torque_home/mom_logs"
server_logs="$torque_home/server_logs"
nodes_file="$torque_home/server_priv/nodes"
mom_file="mom.$today"
server_file="server.$today"
curdir=`pwd`

# actual information gathering 
# create temporary directory, copy files there, tar, and delete directory

mkdir $outtmpdir

cp "$mom_logs/$today" "$outtmpdir/$mom_file"
cp "$server_logs/$today" "$outtmpdir/$server_file"
qmgr -c 'p s' > "$outtmpdir/qmgr.txt"
pbsnodes -a > "$outtmpdir/pbsnodes.txt"
cp $nodes_file "$outtmpdir/nodes"

cd $tmpdir
tar -czf "$curdir/$outfile" $outdir
rm -rf $outdir

