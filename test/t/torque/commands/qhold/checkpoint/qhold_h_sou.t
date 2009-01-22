#!/usr/bin/perl

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
                                    submitCheckpointJob
                                    runJobs 
                                    delJobs
                                 );
use Torque::Test::Utils        qw( 
                                    run_and_check_cmd
                                    job_info
                                 );

use Torque::Test::Qhold::Utils qw(
                                    verify_qhold_chkpt
                                 );

# Test Description
plan('no_plan');
setDesc("qhold -h sou (checkpointable)");

# Variables
my $params;
my $job_id;
my $cmd;
my %qhold;
my $arg;
my %job_info;

###############################################################################
# Test qhold -h sou when the job is checkpointable
###############################################################################

# Submit a job
$params = {
            'user'       => $props->get_property('torque.user.one'),
            'torque_bin' => $props->get_property('Torque.Home.Dir') . 'bin/',
            'app'        => resolve_path("$FindBin::Bin/../../../test_programs/test.pl")
          };

$job_id = submitCheckpointJob($params);

# Run the job
runJobs($job_id);

# Test qhold -h sou
$arg     = "sou";
$cmd     = "qhold -h $arg $job_id";
%qhold   = run_and_check_cmd($cmd);

%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'Hold_Types' } eq $arg, "Checking for Hold_Types of '$arg' for '$job_id'");

# Verify the the job was checkpointed and placed in the hold state
verify_qhold_chkpt($job_id);

# Delete the job
delJobs($job_id);
