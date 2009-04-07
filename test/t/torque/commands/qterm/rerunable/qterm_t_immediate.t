#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


use CRI::Test;

use Torque::Ctrl        qw( 
                            startPbsserver 
                          );
use Torque::Job::Ctrl   qw(
                            submitSleepJob
                            runJobs
                            delJobs
                          );
use Torque::Test::Utils qw(
                             run_and_check_cmd
                             job_info
                          );

plan('no_plan');
setDesc('qterm -t immediate');

# Make sure pbs_server is running
diag("Restarting the pbs_server");
startPbsserver();

# Submit a job
my $job_params = {
                   'user'       => $props->get_property( 'torque.user.one' ),
                   'torque_bin' => $props->get_property( 'Torque.Home.Dir' ) . '/bin',
                   'sleep_time' => 15
                 };

# Submit rerunnable jobs
$job_params->{ 'add_args' } = '-r y';
my $job_id1 = submitSleepJob($job_params);
my $job_id2 = submitSleepJob($job_params);

# Submit jobs that are not rerunnable
$job_params->{ 'add_args' } = '-r n';
my $job_id3 = submitSleepJob($job_params);

die("Unable to submit jobs")
  if (   $job_id1 eq '-1' 
      or $job_id2 eq '-1' 
      or $job_id3 eq '-1');

# Run the jobs
runJobs($job_id2, $job_id3);

# sleep for a few seconds
sleep 2;

# Test qterm -t immediate
my $qterm_cmd = "qterm -t immediate";
my %qterm     = run_and_check_cmd($qterm_cmd);

# sleep for a few seconds
sleep 2;

# Restart pbs_server
diag("Restarting the pbs_server");
startPbsserver();

# Test the jobs and their states
my %job_info = job_info();
ok(exists $job_info{ $job_id1 }, "Checking for job '$job_id1'");
cmp_ok($job_info{ $job_id1 }{ 'job_state' }, 'eq', 'Q', "Checking state of job '$job_id1'");
ok(exists $job_info{ $job_id2 }, "Checking for job '$job_id2'");
cmp_ok($job_info{ $job_id2 }{ 'job_state' }, 'eq', 'Q', "Checking state of job '$job_id2'");
ok(exists $job_info{ $job_id3 }, "Checking for job '$job_id3'");
cmp_ok($job_info{ $job_id3 }{ 'job_state' }, 'eq', 'C', "Checking state of job '$job_id3'");

# Delete Jobs
delJobs($job_id1, $job_id2, $job_id3);
