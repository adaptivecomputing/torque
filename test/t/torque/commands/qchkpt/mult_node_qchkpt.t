#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;
use Torque::Job::Ctrl          qw( 
                                    submitCheckpointJob
                                    runJobs 
                                    delJobs
                                 );
use Torque::Test::Utils        qw( 
                                    run_and_check_cmd
                                    list2array
                                 );
use Torque::Test::Qstat::Utils qw(
                                    parse_qstat_fx
                                 );
use Torque::Test::Regexp       qw(
                                    CHECKPOINT_FILE_NAME
                                 );


# Test Description
plan('no_plan');
setDesc("qchkpt (Multiple Nodes)");

# Variables
my $qchkpt_cmd;
my %qchkpt;
my $qrun_cmd;
my %qrun;
my $ls_cmd;
my %ls;
my %job_info;
my $checkpoint_name;
my $checkpoint_path;
my $local_job_id;
my $job_id;
my @job_ids;

my $local_node   = $props->get_property('MoabHost');
my @remote_nodes = list2array($props->get_property('torque.remote.nodes'));

# Submit checkpoint job params
my $params = {
              'user'       => $props->get_property('torque.user.one'),
              'torque_bin' => $props->get_property('torque.home.dir') . 'bin/',
              'app'        => $props->get_property('test.base') . 'torque/test_programs/test.pl'
             };

###############################################################################
# Submit a job on the local node
###############################################################################
$job_id = submitCheckpointJob($params);
push(@job_ids, $job_id);

# Run the job
$qrun_cmd = "qrun -H $local_node $job_id";
%qrun     = runCommand($qrun_cmd);
ok($qrun{ 'EXIT_CODE' } == 0, "Checking exit code of '$qrun_cmd'");

# Wait a few seconds
sleep 2;

# Test qchkpt
$qchkpt_cmd = "qchkpt $job_id";
%qchkpt     = run_and_check_cmd($qchkpt_cmd);

# Check every second for two minutes for the checkpoint to be created
my $count = 0;
while ($count < 120 and ! defined $checkpoint_name)
  {

  sleep 1;

  my %qstat = runCommand("qstat -fx $job_id");
  %job_info = parse_qstat_fx($qstat{ 'STDOUT' });

  if (defined $job_info{ $job_id }{ 'checkpoint_name' })
    {
    $checkpoint_name = $job_info{ $job_id }{ 'checkpoint_name' }; 
    }

  $count++;

  } # END while ($count < 120 and $checkpoint_name)

# Make sure that we just didn't run out of time
if (! defined $checkpoint_name)
  {

  fail("'checkpoint_name' attribute not found for job '$job_id'");

  } # END if (! defined $checkpoint_name)

# Perform the basic tests
ok($checkpoint_name  =~ /${\CHECKPOINT_FILE_NAME}/,  "Checking the 'checkpoint_name' attribute of the job");
ok(exists $job_info{ $job_id }{ 'checkpoint_time' }, "Checking for the existence of the job attribute 'checkpoint_time'");

# Check for the actual file 
$checkpoint_path = $props->get_property('torque.home.dir') . "checkpoint/${job_id}.CK/$checkpoint_name";
ok(-e $checkpoint_path, "Checking that '$checkpoint_path' exists");

###############################################################################
# Check qchkpt on each remote node
###############################################################################
# Test qchkpt for each remote node
foreach my $node (@remote_nodes)
  {

  # Reset the $checkpoint_name
  $checkpoint_name = undef;

  # Submit a the job
  $job_id = submitCheckpointJob($params);
  push(@job_ids, $job_id);

  # Run the job
  $qrun_cmd = "qrun -H $node $job_id";
  %qrun     = runCommand($qrun_cmd);
  ok($qrun{ 'EXIT_CODE' } == 0, "Checking exit code of '$qrun_cmd'")
    or next;

  # Wait a few seconds
  sleep 2;

  # Test qchkpt
  $qchkpt_cmd = "qchkpt $job_id";
  %qchkpt     = run_and_check_cmd($qchkpt_cmd);

  # Check every second for two minutes for the checkpoint to be created
  my $count = 0;
  while ($count < 120 and ! defined $checkpoint_name)
    {

    sleep 1;

    my %qstat = runCommand("qstat -fx $job_id");
    %job_info = parse_qstat_fx($qstat{ 'STDOUT' });

    if (defined $job_info{ $job_id }{ 'checkpoint_name' })
      {
      $checkpoint_name = $job_info{ $job_id }{ 'checkpoint_name' }; 
      }

    $count++;

    } # END while ($count < 120 and $checkpoint_name)

  # Make sure that we just didn't run out of time
  if (! defined $checkpoint_name)
    {

    fail("'checkpoint_name' attribute not found for job '$job_id'");
    next;

    } # END if (! defined $checkpoint_name)

  # Perform the basic tests
  ok($checkpoint_name  =~ /${\CHECKPOINT_FILE_NAME}/,  "Checking the 'checkpoint_name' attribute of the job");
  ok(exists $job_info{ $job_id }{ 'checkpoint_time' }, "Checking for the existence of the job attribute 'checkpoint_time'");

  # Check for the actual checkpoint file on the node
  $checkpoint_path = $props->get_property('torque.home.dir') . "checkpoint/${job_id}.CK/$checkpoint_name";
  $ls_cmd = "ls $checkpoint_path";
  %ls     = runCommandSsh($node, $ls_cmd);
  ok($ls{ 'EXIT_CODE' } == 0, "Checking exit code of '$ls_cmd'")
    or next;
  ok($ls{ 'STDOUT' } =~ /${checkpoint_path}/, "Checking that '$node:$checkpoint_path' exists");

  } # END foreach my $job_id (@job_ids)

###############################################################################
# Delete the jobs
###############################################################################
delJobs(@job_ids);
