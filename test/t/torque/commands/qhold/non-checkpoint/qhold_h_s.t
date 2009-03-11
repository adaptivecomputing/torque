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
use Torque::Test::Utils        qw( 
                                    run_and_check_cmd
                                    job_info
                                 );

# Test Description
plan('no_plan');
setDesc("qhold -h -s");

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
            'user'       => $props->get_property('torque.user.one'),
            'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
          };

$job_id = submitSleepJob($params);

# Run the job
runJobs($job_id);

# Test qhold -h s
$arg   = "s";
$cmd   = "qhold -h $arg $job_id";
%qhold   = runCommand($cmd);

like($qhold{ 'STDERR' }, 
     qr/qhold: Invalid request MSG=job not held since checkpointing is expected but not enabled for job $job_id/, 
     "Checking for qhold error message");

# Delete the job
delJobs($job_id);
