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
use Torque::Util::Qalter qw( gen_a_time        );

# Test Description
plan('no_plan');
setDesc("qalter -a");

# Variables
my $cmd;
my $qstat;
my $qstat_fx;
my %qalter;
my @job_ids;

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

  # Alter the job
  my $date_str = gen_a_time(time() + 3600);
  $cmd         = "qalter -a $date_str $job_id";
  %qalter      = run_and_check_cmd($cmd); 

  # Check that the command ran
  $cmd      = "qstat -f -x";

  $qstat_fx = qstat_fx({job_id => $job_id});

  } # END foreach my $job_id (@job_ids)

# Delete the jobs
delJobs(@job_ids);
