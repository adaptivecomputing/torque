#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


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
                            job_info
                          );

plan('no_plan');
setDesc('pbs_mom (Check default -p behavior for TORQUE 2.4)');

# Variables
my $sleep_time = 299;
my $job_id;
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('torque.home.dir') . "/bin/",
                   'sleep_time' => $sleep_time
                 };

# Commands
my $pgrep_cmd   = 'pgrep -x pbs_mom';
my $pbs_mom_cmd = "pbs_mom";

# Hashes
my %pgrep;
my %pbs_mom;
my %job_info;

###############################################################################
# Make sure pbs_mom is stopped
###############################################################################
diag("Make sure pbs_mom is stopped");
stopPbsmom();

###############################################################################
# Start pbs_mom with
###############################################################################
diag("Start pbs_mom with");
%pbs_mom = runCommand($pbs_mom_cmd);
ok($pbs_mom{ 'EXIT_CODE' } == 0, "Checking exit code of pbs_mom")
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
diag("Stop pbs and check that the job is still in the running state");
stopPbsmom();

# Check that the job is still running
%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'job_state' } eq 'R', "Checking that job '$job_id' is in the running (R) state")
  or diag("\$job_info{ $job_id }{ 'job_state' } => $job_info{ $job_id }{ 'job_state' }");

###############################################################################
# Start pbs_mom and verify that the job is still in the running state
###############################################################################
diag("Start pbs_mom and verify that the job is still in the running state");
%pbs_mom = runCommand($pbs_mom_cmd);
ok($pbs_mom{ 'EXIT_CODE' } == 0, "Checking exit code of pbs_mom")
  or diag("EXIT_CODE: $pbs_mom{ 'EXIT_CODE' }\nSTDERR: $pbs_mom{ 'STDERR' }");

# Make sure that pbs_mom has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_mom is running");

# Check that the job is still running
%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'job_state' } eq 'R', "Checking that job '$job_id' is in the running (R) state")
  or diag("\$job_info{ $job_id }{ 'job_state' } => $job_info{ $job_id }{ 'job_state' }");

###############################################################################
# Delete the job
###############################################################################
diag("Delete job '$job_id'");
delJobs($job_id)
