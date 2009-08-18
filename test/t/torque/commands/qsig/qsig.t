#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;
use CRI::Utils                qw(
                                  resolve_path
                                );


use Torque::Job::Ctrl         qw(
                                 submitJob
                                 runJobs
                                 delJobs 
                                );
use Torque::Util       qw(
                                 run_and_check_cmd
                                 job_info
                                );

use Torque::Util::Qsig qw(
                                 get_recieved_signals
                                );

# Test Description
plan('no_plan');
setDesc("qsig");

# Variables
my $cmd;
my %qsub;
my %qrun;
my %qsig;
my $job_params;
my $job_id;
my %job_info;
my %signals;

my $user       = $props->get_property('torque.user.one');
my $app        = resolve_path("$FindBin::Bin/../../test_programs/sig_trap.pl");
my $torque_bin = $props->get_property('Torque.Home.Dir') . "/bin/";

###############################################################################
# Submit a job
###############################################################################
$job_params = {
                'user'       => $user,
                'torque_bin' => $torque_bin,
                'app'        => $app
              };

$job_id = submitJob($job_params);

###############################################################################
# Run the job
###############################################################################
runJobs($job_id);

# Check that the job is running
%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'job_state' } eq 'R', 
   "Checking the job state for 'R' for job '$job_id'");

###############################################################################
# Test qsig
###############################################################################
sleep_diag 5;
$cmd  = "qsig $job_id";
%qsig = run_and_check_cmd($cmd);

%signals = get_recieved_signals($job_id);
ok(defined $signals{ 'TERM' }, "Checking for the 'TERM' signal");

###############################################################################
# Delete the job
###############################################################################
delJobs($job_id);
