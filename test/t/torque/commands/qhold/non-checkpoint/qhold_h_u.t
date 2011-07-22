#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


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
$params = {
            'user'       => $props->get_property('User.1'),
            'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
          };

$job_id = submitSleepJob($params);

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
