#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More qw(no_plan);
use subs qw(check_jobs cancel_jobs cleanup_jobs);
our @Jobs    = ();
our @Running = ();

diag('SKIPPED: This test is still under development');
ok(1,'');

__END__
diag('Submitting multiple jobs to TORQUE - this may take several minutes');

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
my $joblength = 300; # seconds
my $walltime  = 2 * $joblength;
for my $i (0..($jobcount-1))
  {
  $Jobs[$i] = `su $testuser -c 'echo "sleep $joblength" | qsub -l nodes=1,walltime=$walltime'` || undef;
  ok(defined $Jobs[$i], 'Job Submission') or
    BAIL_OUT("Unable to submit job #$i to TORQUE as '$testuser' - see Section 2.1");
  ok($Jobs[$i] =~ /^\d+\S*\s*$/, 'Job Submission') or
    BAIL_OUT("Unable to submit job #$i to TORQUE as '$testuser' - see Section 2.1");
  }

# Jobs In Queue
for my $i (0..($jobcount-1))
  {
  $Jobs[$i] =~ s/\D//g;
  my $qstat = `qstat | grep $Jobs[$i]` || undef;
  ok(defined $qstat, 'Job in Queue') or
    BAIL_OUT('Submitted job does not appear in the TORQUE queue');
  }

# Job Set 1 (first half)
diag("BP: 1");
check_jobs   0, int($jobcount / 2) - 1;
diag("BP: 2");
cancel_jobs  0, int($jobcount / 2) - 1;
diag("BP: 3");
cleanup_jobs 0, int($jobcount / 2) - 1;
diag("BP: 4");

# Job Set 2 (second half)
diag("BP: 5");
check_jobs   int($jobcount / 2), $jobcount - 1;
diag("BP: 6");
cancel_jobs  int($jobcount / 2), $jobcount - 1;
diag("BP: 7");
cleanup_jobs int($jobcount / 2), $jobcount - 1;
diag("BP: 8");


#------------------------------------------------------------------------------
#
sub check_jobs ($$)
  {
  my ($first, $last, undef) = @_;
  my %checkjob = ();
  my $waittime = int($joblength * .75) + 1;
     $waittime = 45 if $waittime < 45;

  # Wait up to $waittime seconds for job to start
  TIMER: for (0..($waittime - 1))
    {
    sleep 1;
    next unless $_ % 5;

    # Gather Information
    my $qstat = `qstat` || undef;
    ok(defined $qstat, 'qstat Results') or
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
      $Running[$i]++ if exists $checkjob{$i};
      last TIMER;
      }
    }
  for my $i ($first..$last)
    {
    ok($Running[$i], 'Job Running') or
      BAIL_OUT("Submitted job #$i has failed to start within $waittime seconds - check scheduler - see Section 5.1");
    }
  }
#
#------------------------------------------------------------------------------
#
sub cancel_jobs ($$)
  {
  my ($first, $last, undef) = @_;
  for my $i ($first..$last)
   {
     my $qdel = `qdel $Jobs[$i]` || undef;
    ok(!defined $qdel, "Cancel Job #$i") or
      BAIL_OUT("Submitted job #$i could not be cancelled");
    }
  }
#
#------------------------------------------------------------------------------
#
sub cleanup_jobs ($$)
  {
  my ($first, $last, undef) = @_;
  for my $i ($first..$last)
    {
    my $outputfile = "STDIN.o$i";
    my $errorfile  = "STDIN.e$i";
    ok(-T $outputfile, 'Output File') or
      BAIL_OUT("Submitted job #$i output file not copied - check data staging - see Section 6");
    ok(-T $errorfile, 'Error File') or
      BAIL_OUT("Submitted job #$i error file not copied - check data staging - see Section 6");
    unlink $outputfile if -T $outputfile;
    unlink $errorfile  if -T $errorfile;
    }
  }
#
#------------------------------------------------------------------------------

