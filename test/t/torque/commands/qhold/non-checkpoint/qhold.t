#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;
use Torque::Job::Ctrl          qw( qsub
                                    runJobs 
                                    delJobs
                                 );
use Torque::Util        qw( 
                                    run_and_check_cmd
                                 );
use Torque::Test::Qhold qw(
                                   verify_qhold
                                 );


# Test Description
plan('no_plan');
setDesc("qhold");

# Variables
my $params;
my $job_id;
my $cmd;
my %qhold;

###############################################################################
# Test qhold when the job is not checkpointable
###############################################################################

# Submit a job
$params = {
  script => test_lib_loc()."/../t/torque/test_programs/test.pl",
  full_jobid => 1,
};

$job_id = qsub($params);

# Run the job
runJobs($job_id);

# Test qhold
$cmd     = "qhold $job_id";
%qhold   = run_and_check_cmd($cmd);

verify_qhold($job_id);

# Delete the job
delJobs($job_id);
