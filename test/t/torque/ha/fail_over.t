#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc('Tests Fail Scenario of Torque HA Setup');

# Kill any running pbs_servers, then start two --ha servers

runCommand("mjobctl -c ALL"); # kill any jobs
runCommand("qdel ALL"); # kill any jobs
sleep 5;
runCommand("killall -9 pbs_server"); # kill any servers
runCommand("pbs_server --ha") && die("Couldn't start pbs_server in HA mode!");
runCommand('pbs_server --ha </dev/null >/dev/null 2>/dev/null &') && die("Couldn't start pbs_server in HA mode!");
runCommand("killall -9 pbs_mom"); # kill any servers
runCommand("pbs_mom") && die("Couldn't start pbs_server in HA mode!");

sleep(5);

my %command = runCommand('showstats');
$command{'STDOUT'}  =~ /Completed Jobs:\s+(\d+)/is;

my $initial_completed_jobs = $1;

my $user = $props->get_property('User.1');
my $num_jobs = 5;
my $num_fails = 3;

%command = runCommand('showconfig | grep RMPOLL');
$command{'STDOUT'} =~ /.*(\d+):(\d+):(\d+)/;
my $delay = $1*3600 + $2*60 + $3 + 2;

for (my $i = 0; $i < $num_jobs; $i++){
    runCommandAs("$user",'echo sleep 45 | qsub');
}
sleep 4; # Allow time for the jobs to enter queue

%command = runCommand('showq');
$command{'STDOUT'}  =~ /(\d+) active jobs?.+(\d+) eligible jobs?.+(\d+) blocked jobs?/is;

logMsg("Found " . ($1 + $2) . " jobs. Looking for $num_jobs");
ok(($1 + $2) == $num_jobs, 'Checking that jobs were added successfully');
ok($3 == 0, 'Checking that jobs were not blocked')
    or die "$3 jobs were blocked";

for (1 .. $num_fails)
{
    # Get head pbs_server pid# and kill process

    runCommand('showq');
    runCommand('checkjob -v ALL');

    %command = runCommand('ps aux | grep pbs_server | grep -v grep');
    $command{'STDOUT'} =~ /^\S+\s+(\d+).+Ss.+pbs_server --ha/s;
    
    ok(!runCommand("kill -9 $1"),'Killing active Torque server')
	or die "Unable to kill pbs_server pid# $1";
    
    sleep $delay; # Allow time for the idle pbs_server to take over
    
    %command = runCommand('showstats');
    $command{'STDOUT'}  =~ /Idle Jobs:\s+\d+\/(\d+).+Active Jobs:\s+(\d+).+Completed Jobs:\s+(\d+)/is;

    sleep 10;
    
    logMsg("Looking for " . ($1 + $2 + $3) . " to equal $num_jobs");
    ok(($1 + $2 + $3) == ($num_jobs + $initial_completed_jobs), 'Checking that jobs were recovered correctly after failure')
	or die "Jobs were not recovered after failure";
   
    runCommand('pbs_server --ha </dev/null >/dev/null 2>/dev/null');
    my %start = runCommand('pgrep -x pbs_server');
    
    ok($start{'STDOUT'} =~ /\d+\s+\d+/s,'Starting a backup Torque server process')
	or die 'Failed to start extra pbs_server';
}

runCommand('canceljob ALL && mschedctl -R');
sleep 5;
