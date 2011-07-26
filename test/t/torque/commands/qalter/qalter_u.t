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
setDesc("qalter -u");

# Variables
my $m_cmd;
my $fx_cmd;
my %qstat;
my $qstat_fx;
my %qalter;
my @job_ids;
my $user_list;

# Users and host
my $user1 = $props->get_property('User.1');
my $host  = $props->get_property('Test.Host'       );

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
    $qstat_fx = qstat_fx({job_id => $job_id });
    skip("'$job_id' not in the queued state.  Unable to perform 'qalter -u' tests", 3)
      if $qstat_fx->{ $job_id }{ 'job_state' } ne 'Q';


    # Check qalter -u user1@host,user1@host
    $user_list = "$user1,$user1\@$host";
    $m_cmd     = "qalter -u $user_list $job_id";
    %qalter    = runCommand($m_cmd, test_success => 1);
 
    $fx_cmd    = "qstat -f -x $job_id";
    $qstat_fx  = qstat_fx({job_id => $job_id});
    is($qstat_fx->{ $job_id }{ 'User_List' }, $user_list, "Checking if '$m_cmd' was successful");

    }; # END SKIP:

  } # END foreach my $job_id (@job_ids)

# Delete the jobs
delJobs(@job_ids);
