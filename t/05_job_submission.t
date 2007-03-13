#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More tests => 4;

# Check Test User
ok(exists $ENV{'TORQUE_TEST_USER'}, 'Test User Exists') or
  BAIL_OUT('Test user does not exist');
ok(defined $ENV{'TORQUE_TEST_USER'}, 'Valid Test User') or
  BAIL_OUT('Invalid test user');
ok(length $ENV{'TORQUE_TEST_USER'}, 'Valid Test User') or
  BAIL_OUT('Invalid test user');

# Clear Queue
my $joblist = `qstat` || undef;
ok(!defined($joblist), 'Cleared Queue') or
  BAIL_OUT("Queue must be empty to run advanced tests - cancel all jobs and rerun test");

__END__

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

# verify default queue



