#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;
use Torque::Job::Ctrl          qw( 
                                    submitSleepJob
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
setDesc("qhold -h s");

# Variables
my $params;
my $job_id;
my $arg;
my $cmd;
my %qhold;
my %job_info;

###############################################################################
# Test qhold -h s when the job is not checkpointable
###############################################################################

# Submit a job
$params = {
            'user'       => $props->get_property('User.1'),
            'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
          };

$job_id = submitSleepJob($params);

# Run the job
runJobs($job_id);

# Test qhold -h s
$arg   = "s";
$cmd   = "qhold -h $arg $job_id";
%qhold = run_and_check_cmd($cmd);

%job_info = job_info($job_id);
is($job_info{ $job_id }{ 'hold_types' }, $arg, "Checking Hold_Types for '$job_id'");

# Verify that the hold was properly set
verify_qhold($job_id);

# Delete the job
delJobs($job_id);
