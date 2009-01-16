#!/usr/bin/perl

use strict;
use warnings;

use CRI::Test;

use Torque::Ctrl        qw( 
                            startPbsserver 
                          );
use Torque::Job::Ctrl   qw(
                            submitCheckpointJob
                            runJobs
                            delJobs
                          );
use Torque::Test::Utils qw(
                             run_and_check_cmd
                             job_info
                          );

plan('no_plan');
setDesc('qterm -t delay');

# Make sure pbs_server is running
diag("Restarting the pbs_server");
startPbsserver();

# Submit a job
my $job_params = {
                   'user'       => $props->get_property( 'torque.user.one' ),
                   'torque_bin' => $props->get_property( 'torque.home.dir' ) . '/bin',
                   'app'        => $props->get_property( 'test.base' ) . 'torque/test_programs/test.pl',
                   'c_value'    => 'shutdown'
                 };

my $job_id1 = submitCheckpointJob($job_params);
my $job_id2 = submitCheckpointJob($job_params);

die("Unable to submit jobs")
  if ($job_id1 eq '-1' or $job_id2 eq '-1');

# Run the jobs
runJobs($job_id2);

# Test qterm -t delay
my $qterm_cmd = "qterm -t delay";
my %qterm     = run_and_check_cmd($qterm_cmd);

# sleep for a few seconds
sleep 5;

# Restart pbs_server
diag("Restarting the pbs_server");
startPbsserver();

my %job_info = job_info();

# Check that job1 exists
ok(exists $job_info{ $job_id1 }, "Checking for job '$job_id1'");

# Check that job2 exists and has been checkpointed
if (ok(exists $job_info{ $job_id2 }, "Checking for job '$job_id2'"))
  {

  ok(exists $job_info{ $job_id2 }{ 'checkpoint_name' }, "Checking that job '$job_id2' is checkpointed")
    or diag( "\$job_info{ '$job_id2' }{ 'checkpoint_name' } = $job_info{ $job_id2 }{ 'checkpoint_name' }" );

  } # END if (ok(exists $job_info{ $job_id2 }, "Checking for job '$job_id2'"))

# Delete Jobs
delJobs($job_id1, $job_id2);
