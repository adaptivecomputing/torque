#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


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
use Torque::Util        qw(
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

my @attributes = qw(
                     name
                     user
                     time_use
                     state
                     queue
                   );

# Submit a job
$job_params = {
                'args'       => "-q $queue"
              };

# submit a couple of jobs
push(@job_ids, submitSleepJob($job_params));
push(@job_ids, submitSleepJob($job_params));

$cmd   = "qstat $queue";
%qstat = run_and_check_cmd($cmd);

%job_info = parse_qstat( $qstat{ 'STDOUT' } );

foreach my $job_id (@job_ids)
{
  $job_id =~ s/(\d+\.\w+)\.\w+$/$1/;
  ok(exists $job_info{$job_id}, "Found Job $job_id in Output")
    or next;

  foreach my $attribute (@attributes)
  {
    my $reg_exp = &QSTAT_REGEXP->{ $attribute };
    like($job_info{ $job_id }{ $attribute }, $reg_exp, "Checking the '$job_id' $attribute attribute"); 
  }
}

# Delete the job
delJobs(@job_ids);
