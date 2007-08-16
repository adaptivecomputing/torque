#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More tests => 17;
use subs qw(submit_job job_in_queue job_running cleanup_job);
our $Joblength = 20; # seconds

diag('Testing parallel jobs on TORQUE - this may take several minutes');

# Check Test User
ok(exists $ENV{'TORQUE_TEST_USER'}, 'Test User Exists') or
  BAIL_OUT('Test user does not exist');
ok(defined $ENV{'TORQUE_TEST_USER'}, 'Valid Test User') or
  BAIL_OUT('Invalid test user');
ok(length $ENV{'TORQUE_TEST_USER'}, 'Valid Test User') or
  BAIL_OUT('Invalid test user');
my $testuser = $ENV{'TORQUE_TEST_USER'};

# Determine Number of Procs
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
ok($proccount, 'Processor Count') or
  BAIL_OUT('TORQUE reported 0 processors');

# Submit Jobs
my $jobid;

$jobid = submit_job(2);
         job_in_queue($jobid);
         job_running($jobid);
         cleanup_job($jobid);

$jobid = submit_job($proccount);
         job_in_queue($jobid);
         sleep 10;
         job_running($jobid);
         cleanup_job($jobid);

#------------------------------------------------------------------------------
#
sub submit_job ($)
  {
  my $procs = $_[0] || 2;
  my $walltime  = 2 * $Joblength;
  my $job       = `su $testuser -c 'echo "sleep $Joblength" | qsub -l nodes=$procs,walltime=$walltime'` || undef;
  ok(defined $job, 'Job Submission') or
    BAIL_OUT("Unable to submit job to TORQUE as '$testuser' - see TORQUE docs, Section 2.1");
  ok($job =~ /^\d+\S*\s*$/, 'Job Submission') or
    BAIL_OUT("Unable to submit job to TORQUE as '$testuser' - see TORQUE docs, Section 2.1");
  $job =~ s/\D//g;
  return $job;
  }
#
#------------------------------------------------------------------------------
#
sub job_in_queue ($)
  {
  my $job = $_[0];
  sleep 1;
  my $qstat = `qstat | grep $job` || undef;
  ok(defined $qstat, 'Job in Queue') or
    BAIL_OUT('Submitted job does not appear in the TORQUE queue');
  }
#
#------------------------------------------------------------------------------
#
sub job_running ($)
  {
  my $job = $_[0];
  my $running  = 0;
  my $timer    = 0;
  my $waittime = 45;
  for (1..$waittime) # Wait up to $waittime seconds for job to start
    {
    my $line = `qstat | grep $job` || undef;
    unless (defined $line)
      {
      diag("Submitted job has disappeared from the TORQUE queue");
      last;
      }
    if ($line =~ /\sR\s/)
      {
      $running = 1;
      last;
      }
    sleep 1;
    $timer++;
    }
  ok($running, 'Job Running') or
    BAIL_OUT("Submitted job has failed to start within $waittime seconds - check scheduler - see TORQUE docs, Section 5.1");
  my $remaining  = $Joblength - $timer + 1;
  sleep $remaining if ($remaining > 0);
  }
#
#------------------------------------------------------------------------------
#
sub cleanup_job ($)
  {
  my $job = $_[0];
  my $outputfile = "STDIN.o$job";
  my $errorfile  = "STDIN.e$job";
  ok(-T $outputfile, 'Output File') or
    BAIL_OUT('Submitted job output file not copied - check data staging - see TORQUE docs, Section 6');
  ok(-T $errorfile, 'Error File') or
    BAIL_OUT('Submitted job error file not copied - check data staging - see TORQUE docs, Section 6');
  unlink $outputfile if -T $outputfile;
  unlink $errorfile  if -T $errorfile;
  }
#
#------------------------------------------------------------------------------

