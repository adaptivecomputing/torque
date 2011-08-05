#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;

use Torque::Job::Ctrl           qw( qsub
                                    delJobs
                                  );
use Torque::Util::Qstat  qw(
                                    qstat_fx
                                  );
use Torque::Util         qw( 
                                    run_and_check_cmd 
                                  );

# Test Description
plan('no_plan');
setDesc("qdel -m");

# Variables
my $cmd;
my %qdel;
my $job_id;
my $params;
my $msg;

# Submit a job
$job_id = qsub({full_jobid => 1});

# Delete the job
$msg  = "Test Message - " . localtime(); 
$cmd  = "qdel -m '$msg' $job_id";
%qdel = run_and_check_cmd($cmd);

# Check that job is deleted
my $job_info = qstat_fx();

cmp_ok($job_info->{ $job_id }{ 'job_state' }, 'eq', 'C', "Checking job state of job:$job_id");
delJobs($job_id); # Make sure the job gets deleted
