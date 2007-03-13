#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More tests => 1;

ok(1,'');

__END__

$compute1 = `pbsnodes -a | grep <FIRSTHOSTNAME>`

$upnodes = `pbsnodes -a | grep down,unknown | wc`;

if ($upnodes == "0")
  {
  die "Compute nodes have not contacted pbs_server, verify the following:
    - pbs_mom daemons are running on hosts
    - compute hostnames match names listed in $HOMEDIR/server_priv/nodes
    - compute hosts are network-accessible from the head node
    - the command 'momctl -d3 $compute1' does not report any errors"
  }

print "TORQUE compute nodes reporting - $nodecount nodes reporting"

# verify queues

$correctQC = "0";

$queue = `qmgr -c 'p queues' | grep create | wc`
$xqueue = `qmgr -c 'p queues' | grep 'Type = execution' | wc`

for ($qc = $queue;$qc < queuecount;qc++)
  {
  if (qc is started and qc is enabled)
    {
    $correctQC++;

    printf "Queue $qc is correctly configured";
    }
  else
    {
    printf "Queue $qc is configured but not enabled/started - see section XXX"
    }
  }

if ($correctQC eq "0")
  {
  die "TORQUE queues have not been correctly configured - run torque.setup or see section XXX";
  }

# verify scheduler

if ($MauiIsRunning)
  { 
  printf "Maui Scheduler Detected";
  }
else if ($MoabIsRunning)
  {
  printf "Moab Workload Manager Detected";
  }
else if ($FIFOIsRunning)
  {
  printf "pbs_sched Detected";
  }
else
  {
  die "no scheduler is running - jobs will not start, start Moab, Maui, or pbs_sched and rerun test - see section XXX";
  }

print "basic TORQUE setup validated"

# verify job submission

if ($TestUser eq "")
  {
  printf "to run advanced setup, rerun test with TestUser specified";

  exit 0;
  }

$jobCount = `qstat | wc`;

if ($jobCount ne "0")
  {
  die "queue must be empty to run advanced tests - cancel all jobs and rerun test";
  }

$jobID = `echo "sleep 10" | qsub -l nodes=1,walltime=100"`;

if ($jobID eq "error")
  {
  die "cannot submit job, see section XXX";
  }

qstat = `qstat | grep $jobID`;

if ($qstat eq "")
  {
  die "job does not appear in queue";
  }

# job submitted - wait up to 45 seconds for job to start

for (i = 0;i < 45;i++)
  {
  if (i % 5 == 0)
    printf "waiting for job to start";

  $qstat = `qstat | grep $jobID`;

  if ($qstat contains " R ")
    {
    # job is running

    print "job is running";

    break;
    }
  else if ($qstat contains " I ")
    {
    continue;
    }
  else
    {
    die "job in unexpected state $jobState" - see section XXX";
    }
  }

if ($i >= "45")
  {
  die "job not started after 45 seconds - check scheduler - see section XXX";
  }

if (file does not exist "$jobid.o")
  {
  die "job output file not copied - check data staging - see section XXX";
  }

if (file does not exist "$jobid.e")
  {
  die "job error file not copied - check data staging - see section XXX";
  }

# run parallel job

# NYI

# run multiple jobs

# verify deafult queue



