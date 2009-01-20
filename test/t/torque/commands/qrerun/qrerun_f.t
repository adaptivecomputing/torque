#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw( 
                             run_and_check_cmd
                             verify_job_state
                          );
use Torque::Job::Ctrl   qw(
                             submitSleepJob
                             runJobs
                             delJobs
                          );

plan('no_plan');
setDesc("qrerun -f");

# Variables
my $cmd;
my $job_params;
my $verify_params;
my $job_id;

# Submit a job
$job_params = {
                'user'       => $props->get_property('torque.user.one'),
                'torque_bin' => $props->get_property('torque.home.dir') . 'bin/',
              };

$job_id = submitSleepJob($job_params);

# Run the job
runJobs($job_id);

$verify_params = {
                   'job_id'        => $job_id,
                   'exp_job_state' => 'R'
                 };

verify_job_state($verify_params);

# Rerun the job
$cmd = "qrerun -f $job_id";
run_and_check_cmd($cmd);

$verify_params = {
                   'job_id'        => $job_id,
                   'exp_job_state' => 'Q'
                 };

verify_job_state($verify_params);
