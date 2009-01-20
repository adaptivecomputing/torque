#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;


use Torque::Job::Ctrl         qw(
                                 submitJob
                                 runJobs
                                 delJobs 
                                );
use Torque::Test::Utils       qw(
                                 run_and_check_cmd
                                 job_info
                                );

use Torque::Test::Qsig::Utils qw(
                                 get_recieved_signals
                                );

# Test Description
plan('no_plan');
setDesc("qsig -s");

# Variables
my $cmd;
my %qsub;
my %qrun;
my %qsig;
my $job_params;
my $job_id;
my %job_info;
my @signals;
my %rcvd_signals;

my $user       = $props->get_property('torque.user.one');
my $app        = $props->get_property('test.base') . "torque/test_programs/sig_trap.pl";
my $torque_bin = $props->get_property('torque.home.dir') . "bin/";
my $spool_dir  = $props->get_property('torque.home.dir') . "spool/";

# Test qsig -s
@signals = qw(HUP
              INT
              QUIT
              ILL
              TRAP
              ABRT
              BUS
              FPE
              USR1
              SEGV
              USR2
              PIPE
              ALRM
              TERM
             );

# Send each signal to the job
foreach my $signal (@signals)
  {

  diag("Testing for signal '$signal'");

  $job_id = submit_and_run_job();

  $cmd  = "qsig -s $signal $job_id";
  %qsig = run_and_check_cmd($cmd);

  sleep 2;

  %rcvd_signals = get_recieved_signals($job_id);
  ok(defined $rcvd_signals{ $signal }, "Checking for the '$signal' signal");

  delJobs($job_id);

  } # END foreach my $signal (@signals)


###############################################################################
# submit_and_run_job - subroutine to submit and run a job
###############################################################################
sub submit_and_run_job
  {

   my $job_params = {
                  'user'       => $user,
                  'torque_bin' => $torque_bin,
                  'app'        => $app
                 };

   my $job_id = submitJob($job_params);
   my %job_info;

   runJobs($job_id);

   # Check that the job is running
   %job_info = job_info($job_id);
   ok($job_info{ $job_id }{ 'job_state' } eq 'R', 
      "Checking the job state for 'R' for job '$job_id'");

   return $job_id

  } # END sub submit_and_run_job
