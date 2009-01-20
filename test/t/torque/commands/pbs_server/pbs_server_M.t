#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw(
                            list2array
                            job_info
                            verify_job_state
                          );
use Torque::Ctrl        qw(
                            startPbsmom
                            stopPbsserver
                            startPbsserver
                          );
use Torque::Job::Ctrl   qw(
                            submitSleepJob
                            runJobs
                            delJobs
                          );

plan('no_plan');
setDesc('pbs_server -M');

# Variables
my $host        = $props->get_property('MoabHost');
my $port        = '3334';
my @remote_moms = list2array($props->get_property('torque.remote.nodes'));
my $job_id1;
my $job_id2;

# Params
my $remote_mom_params = {
                          'args'  => "-M $port",
                          'nodes' => \@remote_moms
                        };
my $local_mom_params  = {
                          'args'  => "-M $port"
                        };
my $job_params        = {
                          'user'       => $props->get_property('torque.user.one'),
                          'torque_bin' => $props->get_property('torque.home.dir') . '/bin/'
                        };
my $job_state_params  = {
                          'exp_job_state' => 'R',
                          'wait_time'     => 30
                        };

# Commands
my $pgrep_cmd = 'pgrep -x pbs_server';
my $pbs_server_cmd;  

# Hashes
my %pgrep;
my %pbs_server;
my %job_info;

###############################################################################
# Start the pbs_mom
###############################################################################
diag("Restarting pbs_moms to listen on the port '$port'");
startPbsmom($local_mom_params);
startPbsmom($remote_mom_params);

###############################################################################
# Perform the test for pbs_server -M port
###############################################################################
$pbs_server_cmd = "pbs_server -M $port";
diag("Test '$pbs_server_cmd'");

# Make sure pbs_server is stopped
stopPbsserver();

# Start pbs_server
%pbs_server     = runCommand($pbs_server_cmd);
ok($pbs_server{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_server_cmd'")
  or diag("EXIT_CODE: $pbs_server{ 'EXIT_CODE' }\nSTDERR: $pbs_server{ 'STDERR' }");

diag("Waiting for pbs_server to stabilize...");
sleep 15;

# Make sure that pbs_server has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_server is running");

# Check for any error messages
ok($pbs_server{ 'STDERR' } eq '', "Checking for an error message")
  or diag("STDERR: '$pbs_server{ 'STDERR' }'");

# Submit a Sleep job
$job_id1 = submitSleepJob($job_params);
$job_id2 = submitSleepJob($job_params);

# Run the job
runJobs($job_id1);
runJobs($job_id2);
  
# Check the job for the running state
$job_state_params->{ 'job_id' } = $job_id1;
verify_job_state($job_state_params);

$job_state_params->{ 'job_id' } = $job_id2;
verify_job_state($job_state_params);

# Delete the job
delJobs($job_id1, $job_id2);

###############################################################################
#  Test pbs_mom -M :port
###############################################################################
$pbs_server_cmd = "pbs_server -M :$port";
diag("Test '$pbs_server_cmd'");

# Make sure pbs_server is stopped
stopPbsserver();

# Start pbs_server
%pbs_server     = runCommand($pbs_server_cmd);
ok($pbs_server{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_server_cmd'")
  or diag("EXIT_CODE: $pbs_server{ 'EXIT_CODE' }\nSTDERR: $pbs_server{ 'STDERR' }");

diag("Waiting for pbs_server to stabilize...");
sleep 15;

# Make sure that pbs_server has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_server is running");

# Check for any error messages
ok($pbs_server{ 'STDERR' } eq '', "Checking for an error message")
   or diag("STDERR: '$pbs_server{ 'STDERR' }'");

# Submit a Sleep job
$job_id1 = submitSleepJob($job_params);

# Run the job
runJobs($job_id1);
  
# Check the job for the running state
$job_state_params->{ 'job_id' } = $job_id1;
verify_job_state($job_state_params);

# Delete the job
delJobs($job_id1);

###############################################################################
#  Test pbs_mom -M host:port
###############################################################################
$pbs_server_cmd = "pbs_server -M $host:$port";
diag("Test '$pbs_server_cmd'");

# Make sure pbs_server is stopped
stopPbsserver();

# Start pbs_server
%pbs_server     = runCommand($pbs_server_cmd);
ok($pbs_server{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_server_cmd'")
  or diag("EXIT_CODE: $pbs_server{ 'EXIT_CODE' }\nSTDERR: $pbs_server{ 'STDERR' }");

diag("Waiting for pbs_server to stabilize...");
sleep 15;

# Make sure that pbs_server has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_server is running");

# Check for any error messages
ok($pbs_server{ 'STDERR' } eq '', "Checking for an error message")
   or diag("STDERR: '$pbs_server{ 'STDERR' }'");

# Submit a Sleep job
$job_id1 = submitSleepJob($job_params);

# Run the job
runJobs($job_id1);
  
# Check the job for the running state
$job_state_params->{ 'job_id' } = $job_id1;
verify_job_state($job_state_params);

# Delete the job
delJobs($job_id1);

###############################################################################
# Restart the pbs_mom
###############################################################################
diag("Restarting pbs_moms to listen on the default port");
delete $local_mom_params->{ 'args' };
delete $remote_mom_params->{ 'args' };
startPbsmom($local_mom_params);
startPbsmom($remote_mom_params);

###############################################################################
#  Test pbs_mom -M host
###############################################################################
$pbs_server_cmd = "pbs_server -M $host";
diag("Test '$pbs_server_cmd'");

# Make sure pbs_server is stopped
stopPbsserver();

# Start pbs_server
%pbs_server     = runCommand($pbs_server_cmd);
ok($pbs_server{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_server_cmd'")
  or diag("EXIT_CODE: $pbs_server{ 'EXIT_CODE' }\nSTDERR: $pbs_server{ 'STDERR' }");

diag("Waiting for pbs_server to stabilize...");
sleep 15;

# Make sure that pbs_server has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_server is running");

# Check for any error messages
ok($pbs_server{ 'STDERR' } eq '', "Checking for an error message")
   or diag("STDERR: '$pbs_server{ 'STDERR' }'");

# Submit a Sleep job
$job_id1 = submitSleepJob($job_params);

# Run the job
runJobs($job_id1);
  
# Check the job for the running state
$job_state_params->{ 'job_id' } = $job_id1;
verify_job_state($job_state_params);

# Delete the job
delJobs($job_id1);

###############################################################################
# Restart pbs_server to it's default state
###############################################################################
diag("Restarting pbs_server");  
startPbsserver();
