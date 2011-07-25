#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

# Test Modules
use CRI::Test;
use CRI::Util qw(
                   resolve_path
                 );

use Torque::Job::Ctrl          qw( 
                                    submitCheckpointJob
                                    runJobs 
                                    delJobs
                                 );
use Torque::Util        qw( 
                                    run_and_check_cmd
                                    job_info
                                 );

# Test Description
plan('no_plan');
setDesc("qalter -c");

# Variables
my $cmd;
my $arg;
my %qalter;
my %job_info;

# Submit a job
my $params = {
              'user'       => $props->get_property('User.1'),
              'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/',
              'app'        => resolve_path("$FindBin::Bin/../../test_programs/test.pl")
             };

my $job_id = submitCheckpointJob($params);

# Run the job
#runJobs($job_id);

# Test qalter -c n
$arg     = "n"; 
$cmd     = "qalter -c $arg $job_id";
%qalter  = run_and_check_cmd($cmd);

%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'Checkpoint' } eq $arg, "Checking for Checkpoint='$arg'");

# Test qalter -c s
$arg     = "s"; 
$cmd     = "qalter -c $arg $job_id";
%qalter  = run_and_check_cmd($cmd);

%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'Checkpoint' } eq $arg, "Checking for Checkpoint='$arg'");

# Test qalter -c c
$arg     = "c"; 
$cmd     = "qalter -c $arg $job_id";
%qalter  = run_and_check_cmd($cmd);

%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'Checkpoint' } eq $arg, "Checking for Checkpoint='$arg'");

# Test qalter -c c=60
$arg     = "c=60"; 
$cmd     = "qalter -c $arg $job_id";
%qalter  = run_and_check_cmd($cmd);

%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'Checkpoint' } eq $arg, "Checking for Checkpoint='$arg'");

# Delete the job
delJobs($job_id);
