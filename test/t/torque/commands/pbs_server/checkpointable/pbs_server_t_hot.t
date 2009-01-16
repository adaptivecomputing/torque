#!/usr/bin/perl

use strict;
use warnings;

use CRI::Test;

use Torque::Test::Utils qw(
                             job_info
                             list2array
                          );
use Torque::Ctrl        qw(
                             stopPbsserver
                             startPbsserver
                             startPbsmom
                          );
use Torque::Job::Ctrl   qw(
                             submitCheckpointJob
                             submitSleepJob
                             runJobs
                             delJobs
                          );

plan('no_plan');
setDesc("pbs_server -t hot (Rerunnable jobs)");

# Variables
my @job_ids;
my @remote_moms = list2array($props->get_property('torque.remote.nodes'));

# Commands
my $pbs_server_cmd   = "pbs_server -t hot";

# Hashes
my %pbs_server;
my %job_info;

# Params
my $chkpt_params     = {
                         'torque_bin' => $props->get_property('torque.home.dir') . "/bin/",
                         'user'       => $props->get_property('torque.user.one'),
                         'c_value'    => 'enabled,shutdown',
                         'app'        => $props->get_property('test.base') . "/torque/test_programs/test.pl"
                       };
my $non_chkpt_params = {
                         'torque_bin'  => $props->get_property('torque.home.dir') . "/bin/",
                         'user'        => $props->get_property('torque.user.one'),
                       };
my $mom_params       = {
                         'nodes'      => \@remote_moms,
                         'local_node' => 1
                       };

###############################################################################
# Restart pbs_mom
###############################################################################
startPbsmom($mom_params);

###############################################################################
# Submit a job that can be chkpt and one that cannot be chkpt
###############################################################################
# Submit two jobs
push(@job_ids, submitCheckpointJob($chkpt_params));
push(@job_ids, submitSleepJob($non_chkpt_params));

# Run the jobs
runJobs(@job_ids);

###############################################################################
# Restart the pbs_server hot
###############################################################################
stopPbsserver();

%pbs_server = runCommand($pbs_server_cmd);
ok($pbs_server{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_server_cmd'")
  or diag("STDERR: '$pbs_server{ 'STDERR' }'");

diag("Waiting for the pbs_server to stabilize...");

sleep 15;

###############################################################################
# Perform the test
###############################################################################
%job_info = job_info();

# Job1 should be running
ok($job_info{ $job_ids[0] }{ 'job_state' } eq 'R', "Checking job '$job_ids[0]' for job state running")
  or diag("Expected state: 'R'\nActual state: '$job_info{ $job_ids[0] }{ 'job_state' }'");

# Job2 should be queued
ok($job_info{ $job_ids[1] }{ 'job_state' } eq 'Q', "Checking job '$job_ids[1]' for job state running")
  or diag("Expected state: 'Q'\nActual state: '$job_info{ $job_ids[1] }{ 'job_state' }'");

###############################################################################
# Delete the jobs
###############################################################################
delJobs(@job_ids);

###############################################################################
# Restart the pbs_server
###############################################################################
startPbsserver();
