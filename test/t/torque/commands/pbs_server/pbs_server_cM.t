#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
use Torque::Util qw(
                            list2array
                            job_info
                            verify_job_state
                          );
use Torque::Job::Ctrl   qw(
                            submitSleepJob
                            runJobs
                            delJobs
                          );
plan('no_plan');
setDesc('pbs_server -M');

# Variables
my $host        = $props->get_property('Test.Host');
my $port        = '3334';
my @remote_moms = list2array($props->get_property('Torque.Remote.Nodes'));
my $job_id1;

# Params
my $mom_params = {
    'args'       => "-M $port",
    'nodes'      => \@remote_moms,
    'local_node' => 1,
};
my $job_params        = {
                          'user'       => $props->get_property('User.1'),
                          'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
                        };
my $job_state_params  = {
                          'exp_job_state' => 'R',
                          'wait_time'     => 30
                        };

# Hashes
my %pgrep;
my %pbs_server;
my %job_info;

diag("Restarting pbs_moms to listen on the port '$port'");
stopTorque({ 'remote_moms' => \@remote_moms });
startPbsmom($mom_params);

my @tests = (
    $port,
    ":$port",
    "$host:$port",
);

testM( $_ ) foreach @tests;

diag("Restarting pbs_moms to listen on the default port");
stopTorque({ 'remote_moms' => \@remote_moms });

delete $mom_params->{ 'args' };
startPbsmom($mom_params);

testM( $host );

###############################################################################
# Subroutines
###############################################################################
sub testM
{
    diag("############ Test '-M $_[0]' ############");

    stopPbsserver();
    startPbsserver({ 'args' => "-M $_[0]" });
    syncServerMom({ 'mom_hosts' => [ $host, @remote_moms ] });

    # Submit a Sleep job
    $job_id1 = submitSleepJob($job_params);

    # Run the job
    runJobs($job_id1);

    # Check the job for the running state
    $job_state_params->{ 'job_id' } = $job_id1;
    verify_job_state($job_state_params);

    # Delete the job
    delJobs($job_id1);
}
