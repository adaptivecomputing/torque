#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More tests => 5;
$| = 1;


#==============================================================================
#
print "\n"
    . '-' x 24, "\n"
    . "- Torque Testing Suite -\n"
    . '-' x 24, "\n";
#
#------------------------------------------------------------------------------
#
TESTS:
  {
  my $count = 0;
  exit unless ok
    (
    &check_environment(++$count, 'Environment'),
    'Environment'
    );
  exit unless ok
    (
    &verify_install(++$count, 'TORQUE Installation'),
    'TORQUE Installation'
    );
  exit unless ok
    (
    &verify_compute_nodes(++$count, 'Compute Nodes'),
    'Compute Nodes'
    );
  exit unless ok
    (
    &verify_queues(++$count, 'Queues'),
    'Queues'
    );
  exit unless ok
    (
    &verify_scheduler(++$count, 'Scheduler'),
    'Schedular'
    );
  }
#
#==============================================================================


#------------------------------------------------------------------------------
#
sub info ($)
  {
  my $info = $_[0];
     $info =~ s/\s+/ /g;
  print ' ' x 5 . "- - $info\n";
  }
#
#------------------------------------------------------------------------------
#
sub failure ($)
  {
  my $message = $_[0];
     $message =~ s/\s+/ /g;
  print "\n!!\n!! ERROR: $message\n!!\n\n";
  }
#
#------------------------------------------------------------------------------
#
sub check_environment
  {
  print "\n## $_[0] - $_[1]\n";

  info 'Checking environment';

  unless (0 == $>) # Effective User ID
    {
    failure q|You must be 'root' to run this test|;
    return 0;
    }

  return 1;
  }
#
#------------------------------------------------------------------------------
#
sub verify_install
  {
  print "\n## $_[0] - $_[1]\n";

  # pbs_server - TORQUE Install
  my $pbs_server = `which pbs_server` || undef;
  unless (defined $pbs_server)
    {
    failure 'Cannot locate TORQUE server daemon - install TORQUE or add TORQUE install directory to $PATH';
    return 0;
    }

  # qstat - Client Tools
  my $qstat = `which qstat` || undef;
  unless (defined $qstat)
    {
    failure 'Cannot locate TORQUE clients, install TORQUE or add TORQUE install directory to $PATH';
    return 0;
    }

  # version - TORQUE version
  my $version = `qstat --version 2>&1`;
     $version =~ /^version:\s*([\d.]+)\s*$/i;
     $version = $1 || undef;
  unless (defined $version)
    {
    failure 'Cannot determine TORQUE version';
    return 0;
    }

  info "Version: $version";

  # sbin/ - Install Location
  my $sbindir = $pbs_server;
     $sbindir =~ s/pbs_server\s*$//;
  unless ($sbindir ne '')
    {
    failure 'Cannot determine SBIN directory';
    return 0;
    }
  unless (-d $sbindir)
    {
    failure "$sbindir does not seem to exist";
    return 0;
    }

  # PID - TORQUE Currently Running
  my $pid = `ps -ef | grep pbs_server | grep -v grep`;
     $pid =~ /^\S+\s+(\d+)\s+/;
     $pid = $1 || undef;
  unless (defined $pid)
    {
    failure 'pbs_server not running, start pbs_server and rerun test - see Section XXX';
    return 0;
    }

  info "Daemon currently running (PID: $pid)";

  # qmgr - TORQUE Database Created
  my $qmgr = `qmgr -c 'p s'` || undef;
  unless (defined $qmgr)
    {
    failure "TORQUE database not created, restart pbs_server using 'pbs_server -t create' - see Section XXX";
    return 0;
    }

  return 1;
  }
#
#------------------------------------------------------------------------------
#
sub verify_compute_nodes
  {
  print "\n## $_[0] - $_[1]\n";

  # pbs_server --about - Directory Information
  my @pbs_server = `pbs_server --about`;
  unless (scalar @pbs_server)
    {
    failure 'Cannot get directory infromation from pbs_server';
    return 0;
    }
  my $torquehome = undef;
  foreach my $line (@pbs_server)
    {
      my ($key, $value) = split /:\s+/, $line;
      if ($key =~ /serverhome/i)
        {
        chomp $value;
        $torquehome = $value;
        last;
        }
    }
  unless (defined $torquehome)
    {
    failure 'Cannot determine server home directory from pbs_server';
    return 0;
    }

  # pbsnodes - Node List
  my $nodesfile = "$torquehome/server_priv/nodes";
     $nodesfile =~ s#//#/#g;
  my $pbsnodes  = `pbsnodes -a` || undef;
  chomp $pbsnodes if defined $pbsnodes;
  unless ((defined $pbsnodes) and ($pbsnodes ne 'no nodes') and (-T $nodesfile))
    {
    failure qq|TORQUE 'nodes' file not created, setup $nodesfile and restart pbs_server - see Section XXX|;
    return 0;
    }

  # Node Count
  my @nodes     = split /\s+/, $pbsnodes;
  my $nodecount = scalar(@nodes);
  unless ($nodecount > 0)
    {
    failure 'TORQUE reporting zero nodes available';
    return 0;
    }

  info "Nodes found: $nodecount";

  return 1;
  }
#
#------------------------------------------------------------------------------
#
sub verify_queues
  {
  print "\n## $_[0] - $_[1]\n";
  return 1;
  }
#
#------------------------------------------------------------------------------
#
sub verify_scheduler
  {
  print "\n## $_[0] - $_[1]\n";
  return 1;
  }
#
#------------------------------------------------------------------------------

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



