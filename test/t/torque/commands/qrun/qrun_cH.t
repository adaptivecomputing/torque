#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;


use Torque::Job::Ctrl   qw(
                           submitSleepJob
                           delJobs 
                          );
use Torque::Util qw(
                           list2array
                           run_and_check_cmd
                           job_info
                          );

# Test Description
plan('no_plan');
setDesc("qrun -H");

# Variables
my $cmd;
my %qrun;
my @nodes = list2array($props->get_property('Torque.Remote.Nodes'));
my $node;
my $job_params;
my $job_id;
my %job_info;

# Make sure we have a node to submit to
die "No remote nodes to submit to.  Please add the 'Torque.Remote.Nodes' list to clustertest.conf"
  if (! scalar @nodes);

# Submit a job
$job_params = {
                'user'       => $props->get_property('User.1'),
                'torque_bin' => $props->get_property('Torque.Home.Dir') . "/bin/",
              };

$job_id = submitSleepJob($job_params);

# Run the job
$node = shift @nodes;
$cmd  = "qrun -H $node $job_id";
%qrun = run_and_check_cmd($cmd);

# Check the job
%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'job_state' } eq 'R', "Checking the job state for 'R' for job '$job_id'");
ok($job_info{ $job_id }{ 'exec_host' } =~ /${node}\/\d+/, "Checking that '$job_id' is executing on '$node'");

delJobs($job_id);
