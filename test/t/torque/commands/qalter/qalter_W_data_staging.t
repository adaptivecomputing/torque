#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


# Test Modules
use CRI::Test;

use Torque::Job::Ctrl           qw(
                                    submitSleepJob
                                    runJobs
                                    delJobs
                                  );
use Torque::Test::Utils         qw( 
                                    run_and_check_cmd 
                                    list2array
                                    job_info
                                  );
use Torque::Test::Qstat::Utils  qw( parse_qstat_fx    );

# Test Description
plan('no_plan');
setDesc("qalter -W (data_staging)");

# Variables
my $qalter_cmd;
my %job_info;
my $depend;

my $host     = $props->get_property('MoabHost');
my $in_file  = 'in_test.txt';
my $out_file = 'out_test.txt';
my $stagein  = "$in_file\@$host:$out_file";
my $stageout = "$out_file\@$host:$in_file";

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('torque.home.dir') . '/bin/'
                 };

my $job_id = submitSleepJob($job_params);

# Perform the test
$qalter_cmd = "qalter -W stagein=$stagein -W stageout=$stageout $job_id";
run_and_check_cmd($qalter_cmd);

%job_info = job_info($job_id);

ok($job_info{ $job_id }{ 'stagein' } eq $stagein,   "Checking the 'stagein' attribute of '$job_id'")
  or diag("Got:\t\t'$job_info{ $job_id }{ 'stagein' }'\nExpected:\t'$stagein'");

ok($job_info{ $job_id }{ 'stageout' } eq $stageout, "Checking the 'stageout' attribute of '$job_id'")
  or diag("Got:\t\t'$job_info{ $job_id }{ 'stageout' }'\nExpected:\t'$stageout'");

# Delete the jobs
delJobs($job_id);
