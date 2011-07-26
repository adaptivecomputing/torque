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
setDesc("qalter -A");

# Variables
my $cmd;
my %qstat;
my $qstat_fx;
my %qalter;
my @job_ids;
my $acct_name = "Account_" . time();

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

  # Get the job information
  $qstat_fx = qstat_fx({job_id => $job_id});

  SKIP: 
    {

    if ($qstat_fx->{ $job_id }{ 'job_state' } ne 'Q')
      {
      
      skip("'$job_id' not in the 'Q' state.  Unable to perform test.", 3);

      } # END  if ($qstat_fx{ 'job_state' } ne 'Q')

    # Alter the job
    $cmd         = "qalter -A $acct_name $job_id";
    %qalter      = runCommand($cmd, test_success => 1); 

    # Check that the command ran
    $qstat_fx = qstat_fx({job_id => $job_id});

    is( $qstat_fx->{ $job_id }{ 'Account_Name' }, $acct_name ,
         "Checking for account '$acct_name' for job '$job_id'");

    }; # END SKIP:
  } # END foreach my $job_id (@job_ids)

# Delete the jobs
delJobs(@job_ids);
