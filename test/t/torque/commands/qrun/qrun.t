#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;


use Torque::Job::Ctrl   qw(
                           submitSleepJob
                           delJobs 
                          );
use Torque::Test::Utils qw(
                           run_and_check_cmd
                           job_info
                          );

# Test Description
plan('no_plan');
setDesc("qrun");

# Variables
my $cmd;
my %qrun;
my $job_params;
my $job_id;
my %job_info;

# Submit a job
$job_params = {
                'user'       => $props->get_property('torque.user.one'),
                'torque_bin' => $props->get_property('Torque.Home.Dir') . "/bin/",
              };

$job_id = submitSleepJob($job_params);

# Run the job
$cmd  = "qrun $job_id";
%qrun = run_and_check_cmd($cmd);

# Check the job
%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'job_state' } eq 'R', "Checking the job state for 'R' for job '$job_id'");

delJobs($job_id);
