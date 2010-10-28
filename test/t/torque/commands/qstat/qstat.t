#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;


use Torque::Job::Ctrl          qw(
                                   submitSleepJob
                                   runJobs
                                   delJobs 
                                 );
use Torque::Util::Regexp       qw(
                                   QSTAT_REGEXP
                                 );
use Torque::Util        qw(
                                   run_and_check_cmd
                                   job_info
                                 );
use Torque::Util::Qstat qw(
                                   parse_qstat
                                 );


# Test Description
plan('no_plan');
setDesc("qstat");

# Variables
my $cmd;
my %qstat;
my @job_ids;
my %job_info;
my $job_params;

my $user       = $props->get_property( 'torque.user.one' );
my $torque_bin = $props->get_property( 'Torque.Home.Dir' ) . '/bin/';

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
                'torque_bin' => $torque_bin
              };

# submit a couple of jobs
push(@job_ids, submitSleepJob($job_params));
push(@job_ids, submitSleepJob($job_params));

# Test qstat
$cmd   = "qstat";
%qstat = run_and_check_cmd($cmd);

%job_info = parse_qstat( $qstat{ 'STDOUT' } );

foreach my $job_id (@job_ids)
  {

  my $msg = "Checking job '$job_id'";
  diag($msg);
  logMsg($msg);

  foreach my $attribute (@attributes)
    {

    my $reg_exp = &QSTAT_REGEXP->{ $attribute };
    like($job_info{ $job_id }{ $attribute }, qr/${reg_exp}/, "Checking the '$job_id' $attribute attribute"); 

    } # END foreach my $attribute (@attributes)

  } # END foreach my $job_id (@job_ids)

# Delete the job
delJobs(@job_ids);
