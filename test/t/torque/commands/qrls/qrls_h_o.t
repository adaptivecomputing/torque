#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;
use CRI::Util                 qw(
                                    resolve_path
                                 );
use Torque::Job::Ctrl          qw( 
                                    submitSleepJob
                                    delJobs
                                 );
use Torque::Util        qw( 
                                    run_and_check_cmd
                                    verify_job_state
                                    job_info
                                 );


# Test Description
plan('no_plan');
setDesc("qrls -h o");

# Variables
my $job_params;
my $verify_params;
my $job_id;
my $cmd;
my %qhold;
my %qrls;
my %job_info;

my $root  = 'root';
my $user1 = $props->get_property('User.1');

my $hld_type = 'o';
my $rls_type = 'n';

###############################################################################
# Submit a job
###############################################################################
$job_params = {
               'user'       => $user1,
               'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/',
               'app'        => resolve_path("$FindBin::Bin/../../test_programs/test.pl")
              };

$job_id = submitSleepJob($job_params);

###############################################################################
# Test qrls as the original user
###############################################################################

# Test qhold
$cmd   = "qhold -h $hld_type $job_id";
%qhold = runCommandAs($root, $cmd);
ok($qhold{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'");

# Check the job_state and Hold_Types
$verify_params = {
                  'job_id' => $job_id,
                  'exp_job_state' => 'H'
                 };

verify_job_state($verify_params);

%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'Hold_Types' } eq $hld_type, 
   "Checking Hold_Types attribute of '$job_id' for '$hld_type'");

# Release the job
$cmd  = "qrls -h $hld_type $job_id";
%qrls = runCommandAs($root, $cmd);
ok($qrls{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'");

# Check the job_state and Hold_Types (Should change)
$verify_params = {
                  'job_id' => $job_id,
                  'exp_job_state' => 'Q'
                 };

verify_job_state($verify_params);

%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'Hold_Types' } eq $rls_type, 
   "Checking Hold_Types attribute of '$job_id' for '$rls_type'");

##############################################################################
# Test qrls as a different user
###############################################################################

# Test qhold
$cmd   = "qhold -h $hld_type $job_id";
%qhold = runCommandAs($root, $cmd);
ok($qhold{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'");

# Check the job_state and Hold_Types
$verify_params = {
                  'job_id' => $job_id,
                  'exp_job_state' => 'H'
                 };

verify_job_state($verify_params);

%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'Hold_Types' } eq $hld_type, 
   "Checking Hold_Types attribute of '$job_id' for '$hld_type'");


# Check the job_state and Hold_Types (Shouldn't change)
logMsg("The user '$user1' shouldn't be able to release the hold");

$cmd  = "qrls -h $hld_type $job_id";
%qrls = runCommandAs($user1, $cmd);
ok($qrls{ 'EXIT_CODE' } != 0, "Checking exit code of '$cmd'");

$verify_params = {
                  'job_id' => $job_id,
                  'exp_job_state' => 'H'
                 };

verify_job_state($verify_params);

%job_info = job_info($job_id);
ok($job_info{ $job_id }{ 'Hold_Types' } eq $hld_type, 
   "Checking Hold_Types attribute of '$job_id' for '$hld_type'");

###############################################################################
# Delete the job
###############################################################################
delJobs($job_id);
