#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;


use Torque::Job::Ctrl   qw(
                            submitSleepJob
                            delJobs
                          );
use Torque::Util qw(
                            is_running
                            verify_job_state
                          );
use Torque::Ctrl        qw(
                            startTorque
                            stopPbssched
                          );


plan('no_plan');
setDesc('pbs_sched');

# Variables
my @job_ids;

# Commands
my $pbs_sched_cmd      = "pbs_sched";

# Params
my $job_params         = {
                           'user'       => $props->get_property('torque.user.one'),
                           'torque_bin' => $props->get_property('Torque.Home.Dir') . "/bin/"
                         };
my $job_state_params;

# Hashes
my %pbs_sched;


###############################################################################
# Setup for the test
###############################################################################
startTorque();
stopPbssched();

delJobs('all'); # make sure there are no jobs to interfere with test

###############################################################################
# Test the pbs_sched command
###############################################################################
%pbs_sched = runCommand($pbs_sched_cmd);
ok($pbs_sched{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_sched_cmd'")
  or diag("EXIT_CODE: $pbs_sched{ 'EXIT_CODE' }\nSTDERR: $pbs_sched{ 'STDERR' }");

# Wait for pbs_sched to stabilize
diag("Waiting for pbs_sched to stabilize...");
sleep 5;

# Make sure that pbs_sched has started
ok(is_running('pbs_sched'), "Verifying that pbs_sched is running");

###############################################################################
# Test pbs_sched's scheduling
###############################################################################
push(@job_ids, submitSleepJob($job_params));
push(@job_ids, submitSleepJob($job_params));
push(@job_ids, submitSleepJob($job_params));

# Check job 0
$job_state_params = {
                      'job_id'        => $job_ids[0],
                      'exp_job_state' => 'R',
                      'wait_time'     => 30
                    };
verify_job_state($job_state_params);

# Check job 1
$job_state_params = {
                      'job_id'        => $job_ids[1],
                      'exp_job_state' => 'R',
                      'wait_time'     => 30
                    };
verify_job_state($job_state_params);

# Check job 2
$job_state_params = {
                      'job_id'        => $job_ids[2],
                      'exp_job_state' => 'Q',
                      'wait_time'     => 30
                    };
verify_job_state($job_state_params);

###############################################################################
# Cleanup after the test
###############################################################################
delJobs(@job_ids);

stopPbssched();
