#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


# Test Modules
use CRI::Test;
use CRI::Utils                 qw(
                                    resolve_path
                                 );
use Torque::Job::Ctrl          qw( 
                                    submitSleepJob
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
            'user'       => $props->get_property('User.1'),
            'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/',
            'app'        => resolve_path("$FindBin::Bin/../../../test_programs/test.pl")
          };

$job_id = submitSleepJob($params);

# Run the job
runJobs($job_id);

# Test qhold
$cmd     = "qhold $job_id";
%qhold   = run_and_check_cmd($cmd);

verify_qhold($job_id);

# Delete the job
delJobs($job_id);
