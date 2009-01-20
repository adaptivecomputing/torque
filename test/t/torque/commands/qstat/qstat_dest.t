#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


# Test Modules
use CRI::Test;


use Torque::Job::Ctrl          qw(
                                   submitSleepJob
                                   runJobs
                                   delJobs 
                                 );
use Torque::Test::Regexp       qw(
                                   QSTAT_REGEXP
                                 );
use Torque::Test::Utils        qw(
                                   run_and_check_cmd
                                   job_info
                                 );
use Torque::Test::Qstat::Utils qw(
                                   parse_qstat
                                 );


# Test Description
plan('no_plan');
setDesc("qstat [dest]");

# Variables
my $cmd;
my %qstat;
my @job_ids;
my %job_info;
my $job_params;

my $queue      = $props->get_property( 'torque.queue.one' );
my $user       = $props->get_property( 'torque.user.one' );
my $torque_bin = $props->get_property( 'torque.home.dir' ) . 'bin/';

my @attributes = qw(
                     name
                     user
                     time_use
                     state
                     queue
                   );

# Submit a job
$job_params = {
                'user'       => $user,
                'torque_bin' => $torque_bin,
                'args'       => "-q $queue"
              };

# submit a couple of jobs
push(@job_ids, submitSleepJob($job_params));
push(@job_ids, submitSleepJob($job_params));

foreach my $job_id (@job_ids)
  {

  my $msg = "Checking job '$job_id'";
  diag($msg);
  logMsg($msg);

  # Test qstat
  $cmd   = "qstat $queue";
  %qstat = run_and_check_cmd($cmd);

  %job_info = parse_qstat( $qstat{ 'STDOUT' } );

  foreach my $attribute (@attributes)
    {

    my $reg_exp = &QSTAT_REGEXP->{ $attribute };
    ok($job_info{ $job_id }{ $attribute } =~ /${reg_exp}/, "Checking the '$job_id' $attribute attribute"); 

    } # END foreach my $attribute (@attributes)

  } # END foreach my $job_id (@job_ids)

# Delete the job
delJobs(@job_ids);
