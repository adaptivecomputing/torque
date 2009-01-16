#!/usr/bin/perl

use strict;
use warnings;

# Test Modules
use CRI::Test;

use Torque::Job::Ctrl           qw(
                                    submitSleepJob
                                    runJobs
                                    delJobs
                                  );
use Torque::Test::Utils         qw( run_and_check_cmd 
                                    list2array        );
use Torque::Test::Qstat::Utils  qw( parse_qstat_fx    );

# Test Description
plan('no_plan');
setDesc("qalter -A");

# Variables
my $cmd;
my %qstat;
my %qstat_fx;
my %qalter;
my @job_ids;
my $acct_name = "Account_" . time();

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('torque.home.dir') . '/bin/'
                 };

$job_ids[0] = submitSleepJob($job_params);
$job_ids[1] = submitSleepJob($job_params);

# Run a job
runJobs($job_ids[0]);

foreach my $job_id (@job_ids)
  {

  # Get the job information
  $cmd      = "qstat -f -x $job_id";
  %qstat    = run_and_check_cmd($cmd);
  %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });

  SKIP: 
    {

    if ($qstat_fx{ $job_id }{ 'job_state' } ne 'Q')
      {
      
      skip("'$job_id' not in the 'Q' state.  Unable to perform test.", 3);

      } # END  if ($qstat_fx{ 'job_state' } ne 'Q')

    # Alter the job
    $cmd         = "qalter -A $acct_name $job_id";
    %qalter      = run_and_check_cmd($cmd); 

    # Check that the command ran
    $cmd      = "qstat -f -x $job_id";
    %qstat    = run_and_check_cmd($cmd);
    %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });

    ok( (   exists $qstat_fx{ $job_id }{ 'Account_Name' }
         and $qstat_fx{ $job_id }{ 'Account_Name' } eq $acct_name ),
         "Checking for account '$acct_name' for job '$job_id'");

    }; # END SKIP:
  } # END foreach my $job_id (@job_ids)

# Delete the jobs
delJobs(@job_ids);
