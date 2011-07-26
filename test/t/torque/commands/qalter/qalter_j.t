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
setDesc("qalter -j");

# Variables
my $j_cmd;
my $fx_cmd;
my %qstat;
my $qstat_fx;
my %qalter;
my @job_ids;
my $join_path;

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('User.1'),
                   'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
                 };

$job_ids[0] = submitSleepJob($job_params);
$job_ids[1] = submitSleepJob($job_params);

# Run a job
runJobs($job_ids[0]);

foreach my $job_id (@job_ids)
{
  SKIP:
  {
    # Check if we can test the command
    $qstat_fx = qstat_fx({job_id => $job_id});
    skip("'$job_id' not in the queued state.  Unable to perform 'qalter -j' tests", 9)
      if $qstat_fx->{ $job_id }{ 'job_state' } ne 'Q';

    my @cases = (
      'oe',
      'eo',
      'n',
    );

    foreach my $join_path (@cases)
    {
      %qalter = runCommand("qalter -j $join_path $job_id", test_success => 1);
      next if $qalter{EXIT_CODE};

      $qstat_fx = qstat_fx({job_id => $job_id});
      
      is($qstat_fx->{ $job_id }{ 'Join_Path' }, $join_path, "Job $job_id Join Path was Successfully Altered");
    }
  };
}

# Delete the jobs
delJobs(@job_ids);
