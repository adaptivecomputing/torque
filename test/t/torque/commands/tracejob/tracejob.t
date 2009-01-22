#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw(
                            run_and_check_cmd
                          );
use Torque::Job::Ctrl   qw(
                            submitSleepJob
                            runJobs
                            delJobs
                          );

plan('no_plan');
setDesc('tracejob');

# Variables
my $job_params;
my $job_id;
my $tracejob_cmd;
my %tracejob;
my @lines;
my $reg_exp;

my $queue = $props->get_property( 'torque.queue.one' );
my $user  = $props->get_property( 'torque.user.one'  );
my $host  = $props->get_property( 'Test.Host'         );

# Submit a job
$job_params = {
                'user'       => $props->get_property( 'torque.user.one' ),
                'torque_bin' => $props->get_property( 'Torque.Home.Dir' ) . '/bin',
                'sleep_time' => 60
              };

$job_id = submitSleepJob($job_params);

# Run Tracejob
$tracejob_cmd = "tracejob $job_id";
%tracejob     = run_and_check_cmd($tracejob_cmd);

# Check the output of tracejob
@lines = split(/\n/, $tracejob{ 'STDOUT' });

$reg_exp = "Job:\\s+$job_id";
ok($lines[1] =~ /${reg_exp}/, "Checking for the job_id '$job_id'")
  or diag("\$lines[1] = '$lines[1]'");

$reg_exp = "enqueuing into $queue";  
ok($lines[3] =~ /${reg_exp}/, "Checking for '$reg_exp'")
  or diag("\$lines[3] = '$lines[3]'");

$reg_exp = "Job Queued at request of $user\@$host";  
ok($lines[4] =~ /${reg_exp}/, "Checking for '$reg_exp'")
  or diag("\$lines[4] = '$lines[4]'");

$reg_exp = "queue=$queue";  
ok($lines[5] =~ /${reg_exp}/, "Checking for '$reg_exp'")
  or diag("\$lines[5] = '$lines[5]'");
