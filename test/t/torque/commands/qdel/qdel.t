#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

# Test Modules
use CRI::Test;
use Torque::Job::Ctrl    qw( qsub  delJobs );
use Torque::Util::Qstat  qw( qstat_fx );
plan('no_plan');
setDesc("qdel");

# Variables
my $cmd;
my %qdel;
my $job_id;
my $params;

# Submit a job
$job_id = qsub({full_jobid => 1});

# Delete the job
$cmd  = "qdel $job_id";
%qdel = runCommand($cmd, test_success => 1);

# Check that job is deleted
my $job_info = qstat_fx();

is($job_info->{ $job_id }{ 'job_state' }, 'C', "Checking job state of job:$job_id");
delJobs($job_id); # Make sure the job gets deleted
