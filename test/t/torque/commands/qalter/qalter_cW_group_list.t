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
use Torque::Util         qw( 
                                    run_and_check_cmd 
                                    list2array
                                    job_info
                                  );
use Torque::Util::Qstat  qw( parse_qstat_fx    );

# Test Description
plan('no_plan');
setDesc("qalter -W (group_list)");

# Variables
my $qalter_cmd;
my %job_info;
my $depend;

my ($remote_host) = list2array('Torque.Remote.Nodes');

my $group1 = $props->get_property('Group.1');
my $group2 = $props->get_property('Group.2');

my $group_list = "$group1,$group2\@$remote_host";

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('User.1'),
                   'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
                 };

my $job_id = submitSleepJob($job_params);

# Perform the test
$qalter_cmd = "qalter -W group_list=$group_list $job_id";
run_and_check_cmd($qalter_cmd);

%job_info = job_info($job_id);

ok($job_info{ $job_id }{ 'group_list' } eq $group_list, "Checking for the group list");

# Delete the jobs
delJobs($job_id);
