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
use Torque::Job::Utils          qw(                                  
                                    checkForJob
                                  );
use Torque::Util         qw( 
                                    run_and_check_cmd 
                                  );

# Test Description
plan('no_plan');
setDesc("qdel -p");

# Variables
my $cmd;
my %qdel;
my $job_id;
my $params;

# Submit a job
$job_id = qsub({full_jobid => 1});

# Delete the job
$cmd  = "qdel -p $job_id";
%qdel = run_and_check_cmd($cmd);

# Check that job is deleted
ok(! checkForJob($job_id), "Checking that the job was deleted");
