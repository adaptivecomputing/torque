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
use Torque::Util::Qstat  qw( parse_qstat_fx    );

# Test Description
plan('no_plan');
setDesc("qalter -k");

# Variables
my $k_cmd;
my $fx_cmd;
my %qstat;
my %qstat_fx;
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
    $fx_cmd   = "qstat -f -x $job_id";
    %qstat    = run_and_check_cmd($fx_cmd);
    %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
    skip("'$job_id' not in the queued state.  Unable to perform 'qalter -k' tests", 15)
      if $qstat_fx{ $job_id }{ 'job_state' } ne 'Q';

    # Check qalter -k e
    $join_path = 'e';
    $k_cmd     = "qalter -k $join_path $job_id";
    %qalter    = run_and_check_cmd($k_cmd);

    $fx_cmd      = "qstat -f -x $job_id";
    %qstat    = run_and_check_cmd($fx_cmd);
    %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
    ok($qstat_fx{ $job_id }{ 'Keep_Files' } eq $join_path, "Checking if '$k_cmd' was successful");

    # Check qalter -k o
    $join_path = 'o';
    $k_cmd     = "qalter -k $join_path $job_id";
    %qalter    = run_and_check_cmd($k_cmd);

    $fx_cmd      = "qstat -f -x $job_id";
    %qstat    = run_and_check_cmd($fx_cmd);
    %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
    ok($qstat_fx{ $job_id }{ 'Keep_Files' } eq $join_path, "Checking if '$k_cmd' was successful");

    # Check qalter -k oe
    $join_path = 'oe';
    $k_cmd     = "qalter -k $join_path $job_id";
    %qalter    = run_and_check_cmd($k_cmd);

    $fx_cmd      = "qstat -f -x $job_id";
    %qstat    = run_and_check_cmd($fx_cmd);
    %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
    ok($qstat_fx{ $job_id }{ 'Keep_Files' } eq $join_path, "Checking if '$k_cmd' was successful");

    # Check qalter -k eo
    $join_path = 'eo';
    $k_cmd     = "qalter -k $join_path $job_id";
    %qalter    = run_and_check_cmd($k_cmd);

    $fx_cmd      = "qstat -f -x $job_id";
    %qstat    = run_and_check_cmd($fx_cmd);
    %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
    ok($qstat_fx{ $job_id }{ 'Keep_Files' } eq $join_path, "Checking if '$k_cmd' was successful");

    # Check qalter -k n
    $join_path = 'n';
    $k_cmd     = "qalter -k $join_path $job_id";
    %qalter    = run_and_check_cmd($k_cmd);

    $fx_cmd      = "qstat -f -x $job_id";
    %qstat    = run_and_check_cmd($fx_cmd);
    %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
    ok($qstat_fx{ $job_id }{ 'Keep_Files' } eq $join_path, "Checking if '$k_cmd' was successful");

    }; # END SKIP:

  } # END foreach my $job_id (@job_ids)

# Delete the jobs
delJobs(@job_ids);
