#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More qw(no_plan);
use subs qw(check_jobs cancel_jobs cleanup_jobs);
our @Jobs    = ();
our @Running = ();
our $MAX_CANCEL_TRIES = 5;

diag('Testing multiple jobs on TORQUE - this may take several minutes');

# Check Test User
ok(exists $ENV{'TORQUE_TEST_USER'}, 'Test User Exists') or
  BAIL_OUT('Test user does not exist');
ok(defined $ENV{'TORQUE_TEST_USER'}, 'Valid Test User') or
  BAIL_OUT('Invalid test user');
ok(length $ENV{'TORQUE_TEST_USER'}, 'Valid Test User') or
  BAIL_OUT('Invalid test user');
my $testuser = $ENV{'TORQUE_TEST_USER'};

# Determine Number of Jobs
my %nodes     = ();
my $node      = undef;
my $proccount = 0;
my $jobcount  = 0;
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
$jobcount = 2 * $proccount;
ok($proccount, 'Processor Count') or
  BAIL_OUT('TORQUE reported 0 processors');

# Submit Jobs
our $joblength = 300 + ($proccount * 30); # seconds
my  $walltime  = 1.1 * $joblength;
for my $i (0..($jobcount-1))
  {
  $Jobs[$i] = `su $testuser -c 'echo "sleep $joblength" | qsub -l nodes=1,walltime=$walltime'` || undef;
  ok(defined $Jobs[$i], "Job Submission ($i.1)") or
    BAIL_OUT("Unable to submit job #$i to TORQUE as '$testuser' - see Section 2.1");
  ok($Jobs[$i] =~ /^\d+\S*\s*$/, "Job Submission ($i.2)") or
    BAIL_OUT("Unable to submit job #$i to TORQUE as '$testuser' - see Section 2.1");
  }

# Jobs In Queue
sleep 10;
for my $i (0..($jobcount-1))
  {
  $Jobs[$i] =~ s/\D//g;
  my $qstat = `qstat | grep $Jobs[$i]` || undef;
  ok(defined $qstat, "Job in Queue ($i)") or
    BAIL_OUT('Submitted job does not appear in the TORQUE queue');
  }

# Job Set 1 (first half)
check_jobs   0, int($jobcount / 2) - 1;
cancel_jobs  0, int($jobcount / 2) - 1;
cleanup_jobs 0, int($jobcount / 2) - 1;

# Job Set 2 (second half)
check_jobs   int($jobcount / 2), $jobcount - 1;
cancel_jobs  int($jobcount / 2), $jobcount - 1;
cleanup_jobs int($jobcount / 2), $jobcount - 1;


#------------------------------------------------------------------------------
#
sub check_jobs ($$)
  {
  my ($first, $last, undef) = @_;
  my %checkjob = ();
  my $waittime = int($joblength * .25) + 1;
     $waittime = 181 if $waittime < 180;

  # Wait up to $waittime seconds for job to start
  ROUND: for my $second (0..($waittime - 1))
    {

    # Gather Information
    my $qstat = `qstat` || undef;
    ok(defined $qstat, "qstat Results ($second/".($waittime-1).')') or
      BAIL_OUT("Cannot gather information from qstat");
    foreach my $line (split /[\r\n]+/, $qstat)
      {
      next unless $line =~ /^(\d+)/;
      my $job = $1;
      $checkjob{$job}++ if $line =~ /\s[RC]\s/;
      }

    # Check Information
    for my $i ($first..$last)
      {
      next if $Running[$i];
      $Running[$i]++ if exists $checkjob{$Jobs[$i]};
      }
    my $complete = 1;
    for my $i ($first..$last)
      {
      unless ($Running[$i])
        {
      $complete = 0;
        next ROUND;
        }
      }
    last ROUND if $complete;

    sleep 1;

    }

  for my $i ($first..$last)
    {
    ok($Running[$i], 'Job Running') or
      BAIL_OUT("Submitted job #$i ($Jobs[$i]) has failed to start within $waittime seconds - check scheduler - see Section 5.1");
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
       $qdel = undef if $qdel =~ /MSG=invalid state for job/i;
    if (!defined($qdel) and ($tries < $MAX_CANCEL_TRIES))
      {
      $tries++;
      sleep 1;
      diag("Failed to cancel job $i ($Jobs[$i]) - Retry $tries/$MAX_CANCEL_TRIES");
      redo CANCEL;
      }
    ok(!defined $qdel, "Cancel Job ($i.$Jobs[$i])") or
      BAIL_OUT("Submitted job #$i ($Jobs[$i]) could not be cancelled");
    $tries = 0;
    }
  }
#
#------------------------------------------------------------------------------
#
sub cleanup_jobs ($$)
  {
  my ($first, $last, undef) = @_;
  sleep 10;
  for my $i ($first..$last)
    {
    my $outputfile = "STDIN.o$Jobs[$i]";
    my $errorfile  = "STDIN.e$Jobs[$i]";
    ok(-T $outputfile, "Output File ($i.$Jobs[$i])") or
      BAIL_OUT("Submitted job #$i output file not copied - check data staging - see Section 6");
    ok(-T $errorfile, "Error File ($i.$Jobs[$i])") or
      BAIL_OUT("Submitted job #$i error file not copied - check data staging - see Section 6");
    unlink $outputfile if -T $outputfile;
    unlink $errorfile  if -T $errorfile;
    }
  }
#
#------------------------------------------------------------------------------

