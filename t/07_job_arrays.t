#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More qw(no_plan);
use subs qw(check_jobs cancel_jobs qstat_info);
our @Jobs    = ();
our $MAX_CANCEL_TRIES = 5;
our $Jobcount  = 0;
our $Joblength = 1200; # seconds (Default)
our $Testuser  = undef;

diag('Testing job arrays on TORQUE - this may take several minutes');

# Check Test User
  {
  ok(exists $ENV{'TORQUE_TEST_USER'}, 'Test User Exists') or
    BAIL_OUT('Test user does not exist');
  ok(defined $ENV{'TORQUE_TEST_USER'}, 'Valid Test User') or
    BAIL_OUT('Invalid test user');
  ok(length $ENV{'TORQUE_TEST_USER'}, 'Valid Test User') or
    BAIL_OUT('Invalid test user');
  $Testuser = $ENV{'TORQUE_TEST_USER'};
  }

# Determine TORQUE version
my $version = 0;
  {
  my $qstat = `qstat --about 2>&1`;
  ok(defined $qstat, 'TORQUE Information from qstat') or
    BAIL_OUT('Unable to gather TORQUE information from qstat');

  $version = $1 if $qstat =~ /Version:\s+(\d+\.\d+\.\d+)/;
  }

SKIP:
  {

  my $versionMM = ($version =~ /^(\d+\.\d+)\.\d+$/) ? $1 : 0;
  skip "TORQUE not version 2.2.0 or higher ($version)", 1 if $versionMM < 2.2;

  # Determine Number of Jobs
    {
    my %nodes     = ();
    my $node      = undef;
    my $proccount = 0;
    my $pbsnodes  = `pbsnodes` || undef;
    ok(defined $pbsnodes, 'Node Information from pbsnodes') or
      BAIL_OUT('Unable to gather node information from pbsnodes');

    foreach my $line (split /[\r\n]+/, $pbsnodes)
      {
      if ($line =~ /^(\S+)/)
        {
        $node = $1;
        $nodes{$node} = 1;
        next;
        }
      $nodes{$node} += $1 - 1
        if ($line =~ /^\s+np = (\d+)/) and defined $node;
      }

    map { $proccount += $_ } (values %nodes);
    $Jobcount = 2 * $proccount;
    ok($proccount, 'Processor Count') or
      BAIL_OUT('TORQUE reported 0 processors');

    $Joblength = 300 + ($proccount * 30); # seconds
    }

  # Submit Jobs
    {
    my $walltime = 1.1 * $Joblength;
    my $baseid   = `su $Testuser -c 'echo "sleep $Joblength" | qsub -k oe -l nodes=1,walltime=$walltime -t $Jobcount'` || undef;
       $baseid   =~ s/\D//g if defined $baseid;
    ok(defined $baseid, "Job Submission") or
      BAIL_OUT("Unable to submit job to TORQUE as '$Testuser' - see Section 2.1");
    ok($baseid =~ /^\d+\S*\s*$/, "Job Submission") or
      BAIL_OUT("Unable to submit job to TORQUE as '$Testuser' - see Section 2.1");
    @Jobs = map { "$baseid-$_" } (0..($Jobcount-1));
    }

  # Jobs In Queue
    {
    sleep 5;
    my %data = qstat_info;
    for my $i (0..($Jobcount-1))
      {
      ok(exists $data{$Jobs[$i]}, "Job in Queue ($Jobs[$i])") or
        BAIL_OUT("Submitted job ($Jobs[$i]) does not appear in the TORQUE queue");
      }
    }

  # Job Set 1 (first half)
    {
    check_jobs   0, int($Jobcount / 2) - 1;
    cancel_jobs  0, int($Jobcount / 2) - 1;
    }

  # Job Set 2 (second half)
    {
    check_jobs   int($Jobcount / 2), $Jobcount - 1;
    cancel_jobs  int($Jobcount / 2), $Jobcount - 1;
    }

  }


#------------------------------------------------------------------------------
#
sub check_jobs ($$)
  {
  my ($first, $last, undef) = @_;
  my $waittime = int($Joblength * .25) + 1;
     $waittime = 180 if $waittime < 180;
  my %complete = ();

  # Wait up to $waittime seconds for job to start
  ROUND: for my $second (0..($waittime - 1))
    {
    sleep 1;
    my %data = qstat_info;
    # Check Information
    my $allcomplete = 1;
    for my $i ($first..$last)
      {
      my $id = $Jobs[$i];
      $complete{$id} = ((exists $data{$id}) and ($data{$id} =~ /^[RC]$/)) ? 1 : 0;
      unless ($complete{$id})
        {
        $allcomplete = 0;
        next ROUND;
        }
      }
    last ROUND if $allcomplete;

    }

  for my $i ($first..$last)
    {
    ok($complete{$Jobs[$i]}, "Job Running ($Jobs[$i])") or
      BAIL_OUT("Submitted job ($Jobs[$i]) has failed to start within $waittime seconds - check scheduler - see Section 5.1");
    }

  }
#
#------------------------------------------------------------------------------
#
sub cancel_jobs ($$)
  {
  my ($first, $last, undef) = @_;
  my $tries = 0;
  CANCEL: for my $i ($first..$last)
    {
    my $qdel = `qdel $Jobs[$i] 2>&1` || undef;
       $qdel =~ s/[\r\n]//g if defined $qdel;
    if ((defined $qdel) && ($qdel =~ /MSG=invalid state for job/i))
      {
      $qdel = undef if $qdel =~ /MSG=invalid state for job/i;
      }
    if ((defined $qdel) and ($tries < $MAX_CANCEL_TRIES))
      {
      $tries++;
      sleep 1;
      diag("Failed to cancel job ($Jobs[$i]) - Retry $tries/$MAX_CANCEL_TRIES - [$qdel]");
      redo CANCEL;
      }
    ok(!defined $qdel, "Cancel Job ($Jobs[$i])") or
      BAIL_OUT("Submitted job ($Jobs[$i]) could not be cancelled");
    $tries = 0;
    }
  }
#
#------------------------------------------------------------------------------
#
sub qstat_info
  {
  my %data;
  # Gather Information
  my $qstat = `qstat` || undef;
  ok(defined $qstat, 'qstat Results') or
    BAIL_OUT('Cannot gather information from qstat');
  foreach my $line (split /[\r\n]+/, $qstat)
    {
    next unless $line =~
      /^
        (\d+-\d+)\S*\s+ # Job ID
        \S+\s+          # Name
        \S+\s+          # User
        \S+\s+          # Time Use
        (\w)\s+         # State
        \S+\s*          # Queue
      $/x;
    $data{$1} = $2 if defined($1) and defined($2);
    }
  return %data;
  }
#
#------------------------------------------------------------------------------

