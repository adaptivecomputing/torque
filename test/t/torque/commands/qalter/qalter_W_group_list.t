#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


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
setDesc("qalter -W (group_list)");

# Variables
my $qalter_cmd;
my %job_info;
my $depend;

my ($remote_host) = list2array('torque.remote.nodes');

my $group1 = $props->get_property('torque.user.one.group');
my $group2 = $props->get_property('torque.user.two.group');

my $group_list = "$group1,$group2\@$remote_host";

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('torque.home.dir') . '/bin/'
                 };

my $job_id = submitSleepJob($job_params);

# Perform the test
$qalter_cmd = "qalter -W group_list=$group_list $job_id";
run_and_check_cmd($qalter_cmd);

%job_info = job_info($job_id);

ok($job_info{ $job_id }{ 'group_list' } eq $group_list, "Checking for the group list");

# Delete the jobs
delJobs($job_id);
