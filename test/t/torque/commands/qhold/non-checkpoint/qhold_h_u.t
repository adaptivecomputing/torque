#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;
use Torque::Job::Ctrl          qw(  qsub
                                    runJobs 
                                    delJobs
                                 );
use Torque::Util        qw( 
                                    run_and_check_cmd
                                    job_info
                                 );
use Torque::Test::Qhold qw(
                                   verify_qhold
                                 );


# Test Description
plan('no_plan');
setDesc("qhold -h u");

# Variables
my $params;
my $job_id;
my $arg;
my $cmd;
my %qhold;
my %job_info;

###############################################################################
# Test qhold -h u when the job is not checkpointable
###############################################################################

# Submit a job
$job_id = qsub({full_jobid => 1});

# Run the job
runJobs($job_id);

# Test qhold -h u
$arg   = "u";
$cmd   = "qhold -h $arg $job_id";
%qhold = run_and_check_cmd($cmd);

%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'Hold_Types' } eq $arg, "Checking for the Hold_Types of '$arg' for '$job_id'");

# Verify that the hold was properly set
verify_qhold($job_id);

# Delete the job
delJobs($job_id);
