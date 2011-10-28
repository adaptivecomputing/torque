#!/usr//bin/perl
#################################################
# TRQ-490 - make sure that the exit code of the
#  job is not set after qrerun is executed.
#################################################
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Util qw( verify_job_state );
use Torque::Util::Qstat qw( qstat_fx );
use Torque::Job::Ctrl qw( submitSleepJob  runJobs  delJobs );
plan('no_plan');
setDesc("qrerun");

# Variables
my $cmd;
my $job_params;
my $verify_params;
my $job_id;

# Submit a job
$job_id = submitSleepJob({ sleep_time => 5 });

# Run the job
runJobs($job_id);

$verify_params = {
                   'job_id'        => $job_id,
                   'exp_job_state' => 'R'
                 };

verify_job_state($verify_params);

# Rerun the job
$cmd = "qrerun $job_id";
runCommand($cmd, test_success => 1);

$verify_params = {
                   'job_id'        => $job_id,
                   'exp_job_state' => 'Q'
                 };

verify_job_state($verify_params);

my %jhash = qstat_fx({ job_id => $job_id });

cmp_ok($jhash{$job_id}{exit_status}, '==', 0, 'Job exit code is set to passing');

runJobs($job_id);

verify_job_state({
    job_id => $job_id,
    exp_job_state => 'C',
    wait_time => 10,
  });

%jhash = qstat_fx({ job_id => $job_id });

cmp_ok($jhash{$job_id}{exit_status}, '==', 0, 'Job exit code is still set to passing');
