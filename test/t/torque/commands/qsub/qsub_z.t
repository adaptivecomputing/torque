#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;


use Torque::Test::Utils qw( job_info );
use Torque::Job::Ctrl   qw( delJobs  );

plan('no_plan');
setDesc('qsub -z');

# Run the qsub -z command
my $user = $props->get_property('torque.user.one');
my $cmd  = "echo sleep 30 | qsub -z";
my %qsub = runCommandAs($user, $cmd);

# Check the exit code
ok($qsub{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'")
 or die ("'$cmd' failed: $qsub{ 'STDERR' }");

# Get the job_id
my $job_id = $qsub{ 'STDOUT' };

ok($job_id eq '', "Checking that the job_id was not outputed to STDOUT")
  or diag("\$job_id = $job_id");

# Delete all jobs, we don't have a way of getting the job_id
delJobs('all');
