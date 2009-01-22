#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


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
setDesc("qalter -j");

# Variables
my $j_cmd;
my $fx_cmd;
my %qstat;
my %qstat_fx;
my %qalter;
my @job_ids;
my $join_path;

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
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
    skip("'$job_id' not in the queued state.  Unable to perform 'qalter -j' tests", 9)
      if $qstat_fx{ $job_id }{ 'job_state' } ne 'Q';

    # Check qalter -j oe
    $join_path = 'oe';
    $j_cmd     = "qalter -j $join_path $job_id";
    %qalter    = run_and_check_cmd($j_cmd);

    $fx_cmd      = "qstat -f -x $job_id";
    %qstat    = run_and_check_cmd($fx_cmd);
    %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
    ok($qstat_fx{ $job_id }{ 'Join_Path' } eq $join_path, "Checking if '$j_cmd' was successful");

    # Check qalter -j eo
    $join_path = 'eo';
    $j_cmd     = "qalter -j $join_path $job_id";
    %qalter    = run_and_check_cmd($j_cmd);

    $fx_cmd      = "qstat -f -x $job_id";
    %qstat    = run_and_check_cmd($fx_cmd);
    %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
    ok($qstat_fx{ $job_id }{ 'Join_Path' } eq $join_path, "Checking if '$j_cmd' was successful");

    # Check qalter -j n
    $join_path = 'n';
    $j_cmd     = "qalter -j $join_path $job_id";
    %qalter    = run_and_check_cmd($j_cmd);

    $fx_cmd      = "qstat -f -x $job_id";
    %qstat    = run_and_check_cmd($fx_cmd);
    %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
    ok($qstat_fx{ $job_id }{ 'Join_Path' } eq $join_path, "Checking if '$j_cmd' was successful");

    }; # END SKIP:

  } # END foreach my $job_id (@job_ids)

# Delete the jobs
delJobs(@job_ids);
