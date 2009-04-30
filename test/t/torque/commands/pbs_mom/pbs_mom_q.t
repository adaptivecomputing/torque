#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

use Torque::Ctrl        qw(
                            stopPbsmom
                          );
use Torque::Job::Ctrl   qw(
                            submitSleepJob
                            runJobs
                            delJobs
                          );
use Torque::Test::Utils qw(
                            run_and_check_cmd
                            verify_job_state
                          );

plan('no_plan');
setDesc('pbs_mom -q');

# Variables
my $sleep_time   = 60;
my $job_id;
my $job_params   = {
                     'user'       => $props->get_property('torque.user.one'),
                     'torque_bin' => $props->get_property('Torque.Home.Dir') . "/bin/",
                     'sleep_time' => $sleep_time
                   };
my $process_name = "sleep $sleep_time";

# Commands
my $pgrep_cmd   = 'pgrep -x pbs_mom';
my $pbs_mom_cmd = "pbs_mom -q";
my $ps_cmd      = "ps aux | grep 'sleep $sleep_time' | grep -v 'grep' | grep ' R '";

# Hashes
my %pgrep;
my %pbs_mom;
my %ps;

###############################################################################
# Make sure pbs_mom is stopped
###############################################################################
diag("Make sure pbs_mom is stopped");
stopPbsmom();

###############################################################################
# Start pbs_mom with -q
###############################################################################
diag("Start pbs_mom with -q");
%pbs_mom = runCommand($pbs_mom_cmd);
ok($pbs_mom{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_mom_cmd'")
  or diag("EXIT_CODE: $pbs_mom{ 'EXIT_CODE' }\nSTDERR: $pbs_mom{ 'STDERR' }");

# Make sure that pbs_mom has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_mom is running");

###############################################################################
# Submit and run a sleep job
###############################################################################
diag("Submit and run a sleep job");
$job_id = submitSleepJob($job_params);
runJobs($job_id);

###############################################################################
# Stop pbs and check that the job is still in the running state
###############################################################################
diag("Stop pbs and check that the job is still in the queued state");
stopPbsmom();

# Check that the job is queued
verify_job_state({ 
                   'job_id'        => $job_id,
                   'exp_job_state' => 'Q',
                   'wait_time'     => 2 * $sleep_time
                });

###############################################################################
# Start pbs_mom and verify that the job is still in the queued state
###############################################################################
diag("Start pbs_mom and verify that the job is not in the queued state");
%pbs_mom = runCommand($pbs_mom_cmd);
ok($pbs_mom{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_mom_cmd'")
  or diag("EXIT_CODE: $pbs_mom{ 'EXIT_CODE' }\nSTDERR: $pbs_mom{ 'STDERR' }");

# Make sure that pbs_mom has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_mom is running");

# Check that the job is queued
verify_job_state({ 
                   'job_id'        => $job_id,
                   'exp_job_state' => 'Q',
                   'wait_time'     => 2 * $sleep_time
                });

###############################################################################
# Check for the process
###############################################################################
diag("Check for the process '$process_name'");
%ps = runCommand($ps_cmd);
ok($ps{ 'EXIT_CODE' } == 1, "Checking that the process '$process_name' does not exist")
 or diag("STDOUT:\n\n$ps{ 'STDOUT' }\n");

###############################################################################
# Delete the job
###############################################################################
diag("Delete job '$job_id'");
delJobs($job_id)
