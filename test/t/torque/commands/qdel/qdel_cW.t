#!/usr/bin/perl
###############################################################
#  NEEDS REFACTORING
#  The way this test is attempting to test this functionality is
#  incorrect/incomplete. -W is the delay between a SIGTERM and
#  a SIGKILL. Besides, to adequately test it, the shell in which
#  the job script is run needs to be able to handle signals
#  correctly and redirect them to the job. The default shell (bash)
#  does not and so the solution to testing this parameter is more
#  complicated than this test currently handles.
###############################################################
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;

use Torque::Job::Ctrl           qw( qsub
                                    delJobs
                                  );
use Torque::Util::Qstat  qw(
                                    qstat_fx
                                  );
use Torque::Util         qw( 
                                    run_and_check_cmd 
                                  );

# Test Description
plan('no_plan');
setDesc("qdel -W");

# Variables
my $cmd;
my %qdel;
my $job_id;
my $params;
my $delay;

# Submit a job
$job_id = qsub({full_jobid => 1});

# Delete the job
$delay = 8;
$cmd   = "qdel -W $delay $job_id";
%qdel  = run_and_check_cmd($cmd);

my $job_info = qstat_fx();

is($job_info->{ $job_id }{ 'job_state' }, 'Q', "Checking that job:$job_id has not been canceled yet");

sleep_diag 8 * $delay;

# Check that job is deleted
$job_info = qstat_fx();

cmp_ok($job_info->{ $job_id }{ 'job_state' }, 'eq', 'C', "Checking job state of job:$job_id");
delJobs($job_id); # Make sure the job gets deleted
