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
setDesc("qalter -l");

# Variables
my $l_cmd;
my $fx_cmd;
my $qstat;
my $qstat_fx;
my %qalter;
my @job_ids;
my $resouce_list;
my $walltime;
my $nodes;

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

    $qstat_fx = qstat_fx({job_id => $job_id});
    skip("'$job_id' not in the queued state.  Unable to perform 'qalter -l' tests", 8)
      if $qstat_fx->{ $job_id }{ 'job_state' } ne 'Q';

    # Check qalter -l with the tmp variables
    $walltime     = $props->get_property( 'tmp.job.walltime' );
    $nodes        = $props->{ '_props' }{ 'tmp.job.nodes' };   # We do this because get_property won't return a 0
    $resouce_list = "walltime=$walltime,nodes=$nodes";
    $l_cmd        = "qalter -l $resouce_list $job_id";
    %qalter       = run_and_check_cmd($l_cmd);

    $fx_cmd       = "qstat -f -x $job_id";

    $qstat_fx     = qstat_fx({job_id => $job_id});
    ok($qstat_fx->{ $job_id }{ 'Resource_List' }{ 'walltime' } eq $walltime, "Checking if '$l_cmd' was successful in setting the walltime");
    ok($qstat_fx->{ $job_id }{ 'Resource_List' }{ 'nodes'    } eq $nodes,    "Checking if '$l_cmd' was successful in setting the nodes");

    # Check qalter -l with the original variables (Reset the variables)
    $walltime     = $props->get_property( 'job.walltime' );
    $nodes        = $props->get_property( 'job.nodes' );
    $resouce_list = "walltime=$walltime,nodes=$nodes";
    $l_cmd        = "qalter -l $resouce_list $job_id";
    %qalter       = run_and_check_cmd($l_cmd);

    $fx_cmd       = "qstat -f -x $job_id";

    $qstat_fx     = qstat_fx({job_id => $job_id});
    ok($qstat_fx->{ $job_id }{ 'Resource_List' }{ 'walltime' } eq $walltime, "Checking if '$l_cmd' was successful in setting the walltime");
    ok($qstat_fx->{ $job_id }{ 'Resource_List' }{ 'nodes'    } eq $nodes,    "Checking if '$l_cmd' was successful in setting the nodes");

    }; # END SKIP:

  } # END foreach my $job_id (@job_ids)

# Delete the jobs
delJobs(@job_ids);
