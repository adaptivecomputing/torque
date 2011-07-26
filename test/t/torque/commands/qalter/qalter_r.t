#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;

use Torque::Job::Ctrl           qw(
                                    submitSleepJob
                                    runJobs
                                    delJobs
                                  );
use Torque::Util         qw( run_and_check_cmd 
                                    list2array        );
use Torque::Util::Qstat  qw( qstat_fx    );

# Test Description
plan('no_plan');
setDesc("qalter -r");

# Variables
my $h_cmd;
my $fx_cmd;
my $qstat;
my $qstat_fx;
my %qalter;
my $job_id;
my $rerunable;

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('User.1'),
                   'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
                 };

$job_id = submitSleepJob($job_params);

# Run a job
runJobs($job_id);

# Check qalter -r n
$rerunable = 'n';
$h_cmd     = "qalter -r $rerunable $job_id";
%qalter    = run_and_check_cmd($h_cmd);

$fx_cmd   = "qstat -f -x $job_id";

$qstat_fx = qstat_fx({job_id => $job_id});
ok($qstat_fx->{ $job_id }{ 'Rerunable' } =~ /false/i, "Checking if '$h_cmd' was successful");

# Check qalter -r s
$rerunable = 'y';
$h_cmd     = "qalter -r $rerunable $job_id";
%qalter    = run_and_check_cmd($h_cmd);

$fx_cmd   = "qstat -f -x $job_id";

$qstat_fx = qstat_fx({job_id => $job_id});
ok($qstat_fx->{ $job_id }{ 'Rerunable' } =~ /true/i, "Checking if '$h_cmd' was successful");

# Delete the jobs
delJobs($job_id);
