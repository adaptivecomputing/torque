#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
use Torque::Job::Ctrl qw( qsub runJobs );
use Torque::Util::Qstat qw( qstat_fx );
plan('no_plan');
setDesc('Tests Fail Scenario of Torque HA Setup');


stopTorque();
startTorqueClean({ pbs_server => { args => '--ha' }});
runCommand('pbs_server --ha </dev/null >/dev/null 2>/dev/null &', test_success_die => 1);

sleep(5);

my $user = $props->get_property('User.1');
my $num_jobs = 5;
my $num_fails = 3;

my $delay = 5;

my @jobs;
for (my $i = 0; $i < $num_jobs; $i++){
    push @jobs, qsub();
}
runJobs(@jobs);

for (1 .. $num_fails)
{
    # Get head pbs_server pid# and kill process

    %command = runCommand('ps aux | grep pbs_server | grep -v grep');
    $command{'STDOUT'} =~ /^\S+\s+(\d+).+Ss.+pbs_server --ha/s;
    
    ok(!runCommand("kill -9 $1"),'Killing active Torque server')
	or die "Unable to kill pbs_server pid# $1";
    
    sleep $delay; # Allow time for the idle pbs_server to take over
   
    my $jref = qstat_fx();
    cmp_ok(scalar keys %$jref, '==', scalar @jobs, 'Checking that jobs were recovered correctly after failure')
	or die "Jobs were not recovered after failure";
   
    runCommand('pbs_server --ha </dev/null >/dev/null 2>/dev/null');
    my %start = runCommand('pgrep -x pbs_server');
    
    ok($start{'STDOUT'} =~ /\d+\s+\d+/s,'Starting a backup Torque server process')
	or die 'Failed to start extra pbs_server';
}
