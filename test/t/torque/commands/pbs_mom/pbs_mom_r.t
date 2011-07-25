#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

use Torque::Ctrl        qw(
                            stopPbsmom
                            startPbsmom
                          );
use Torque::Job::Ctrl   qw(
                            submitSleepJob
                            runJobs
                            delJobs
                          );
use Torque::Util qw(
                            run_and_check_cmd
                            verify_job_state
                          );

plan('no_plan');
setDesc('pbs_mom -r');

# Variables
my $sleep_time   = 60;
my $job_id;
my $job_params   = {
                     'user'       => $props->get_property('User.1'),
                     'torque_bin' => $props->get_property('Torque.Home.Dir') . "/bin/",
                     'sleep_time' => $sleep_time
                   };
my $process_name = "sleep $sleep_time";

# Commands
my $pgrep_cmd   = 'pgrep -x pbs_mom';
my $ps_cmd      = "ps aux | grep 'sleep $sleep_time' | grep -v 'grep' | grep ' R '";
my @pbs_moms    = split(/,/, $props->get_property('Torque.Remote.Nodes'));
my $pbsmom_href = {
                   'nodes'      => \@pbs_moms,
                   'local_node' => 1,
                   'args'       => '-r'
                  };

# Hashes
my %pgrep;
my %pbs_mom;
my %ps;

###############################################################################
# Make sure pbs_mom is stopped
###############################################################################
diag("Make sure pbs_mom is stopped");
stopPbsmom($pbsmom_href);

###############################################################################
# Start pbs_mom with -r
###############################################################################
diag("Start pbs_mom with -r");
startPbsmom($pbsmom_href);

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
stopPbsmom($pbsmom_href);

# Check that the job is queued
verify_job_state({ 
                   'job_id'        => $job_id,
                   'exp_job_state' => 'R',
                   'wait_time'     => 2 * $sleep_time
                });

###############################################################################
# Start pbs_mom and verify that the job is still in the queued state
###############################################################################
diag("Start pbs_mom and verify that the job is not in the queued state");
startPbsmom($pbsmom_href);

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
