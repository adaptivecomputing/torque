#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More tests => 10;

diag('Testing serial jobs on TORQUE - this may take several minutes');

# Check Test User
ok(exists $ENV{'TORQUE_TEST_USER'}, 'Test User Exists') or
  BAIL_OUT('Test user does not exist');
ok(defined $ENV{'TORQUE_TEST_USER'}, 'Valid Test User') or
  BAIL_OUT('Invalid test user');
ok(length $ENV{'TORQUE_TEST_USER'}, 'Valid Test User') or
  BAIL_OUT('Invalid test user');
my $testuser = $ENV{'TORQUE_TEST_USER'};

# Clear Queue
my $joblist = `qstat` || undef;
ok(!defined($joblist), 'Queue Empty') or
  BAIL_OUT("Queue must be empty to run job submission tests - cancel all jobs and rerun test");

# Submit Job
my $joblength = 10; # seconds
my $walltime  = 2 * $joblength;
my $job       = `su $testuser -c 'echo "sleep $joblength" | qsub -l nodes=1,walltime=$walltime'` || undef;
ok(defined $job, 'Job Submission') or
  BAIL_OUT("Unable to submit job to TORQUE as '$testuser' - see Section 2.1");
ok($job =~ /^\d+\S*\s*$/, 'Job Submission') or
  BAIL_OUT("Unable to submit job to TORQUE as '$testuser' - see Section 2.1");

# Job In Queue
$job =~ s/\D//g;
sleep 1;
my $qstat = `qstat | grep $job` || undef;
ok(defined $qstat, 'Job in Queue') or
  BAIL_OUT('Submitted job does not appear in the TORQUE queue');

# Job Running
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
  BAIL_OUT("Submitted job has failed to start within $waittime seconds - check scheduler - see Section 5.1");

# Check Output Files
my $remaining  = $joblength - $timer + 1;
my $outputfile = "STDIN.o$job";
my $errorfile  = "STDIN.e$job";
sleep $remaining if ($remaining > 0);
ok(-T $outputfile, 'Output File') or
  BAIL_OUT('Submitted job output file not copied - check data staging - see Section 6');
ok(-T $errorfile, 'Error File') or
  BAIL_OUT('Submitted job error file not copied - check data staging - see Section 6');
unlink $outputfile if -T $outputfile;
unlink $errorfile  if -T $errorfile;

