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
use Torque::Util::Qstat  qw( qstat_fx    );

# Test Description
plan('no_plan');
setDesc("qalter -W (after paramaters)");

# Variables
my $w_cmd;
my $fx_cmd;
my %qstat;
my $qstat_fx;
my %qalter;
my $group_list;

my $host   = $props->get_property('Test.Host');

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('User.1'),
                   'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
                 };

my $job1 = submitSleepJob($job_params);
my $job2 = submitSleepJob($job_params);

my @dep_cases = (
  'after',
  'afterok',
  'afternotok',
  'afteranyok',
);

foreach my $depend (@dep_cases)
{
  diag("Test $depend");
  my $full_depend  = "$depend:$job1";
  runCommand("qalter -W depend=$full_depend $job2", test_success => 1);

  $qstat_fx   = qstat_fx({job_id => $job2});
  like($qstat_fx->{ $job2 }{ 'depend' }, qr/$full_depend/, "Job $job2 Dependency Successfully Altered");

  diag("Reset $depend");
  $full_depend  = $depend;
  runCommand("qalter -W depend=$full_depend $job2", test_success => 1);

  $qstat_fx   = qstat_fx({job_id => $job2});
  ok(! defined $qstat_fx->{ $job2 }{ 'depend' }, "Job $job2 Dependency Successfully Reset");
}

# Delete the jobs
delJobs($job1, $job2);
