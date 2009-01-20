#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;

use Torque::Job::Ctrl           qw(
                                    submitSleepJob
                                    runJobs
                                    delJobs
                                  );
use Torque::Test::Utils         qw( run_and_check_cmd 
                                    list2array        );
use Torque::Test::Qstat::Utils  qw( parse_qstat_fx    );

# Test Description
plan('no_plan');
setDesc("qalter -p");

# Variables
my $p_cmd;
my $fx_cmd;
my %qstat;
my %qstat_fx;
my %qalter;
my $job_id;
my $priority;

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('torque.home.dir') . '/bin/'
                 };

$job_id = submitSleepJob($job_params);

# Run a job
runJobs($job_id);

# Check qalter -p positive boundry 
$priority  = 1023; # This should pass
$p_cmd     = "qalter -p $priority $job_id";
%qalter    = run_and_check_cmd($p_cmd);

$fx_cmd   = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($fx_cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Priority' } eq $priority, "Checking if '$p_cmd' was successful");

$priority  = 1024; # This should fail
$p_cmd     = "qalter -p $priority $job_id";
%qalter    = runCommand($p_cmd);
ok((    defined $qalter{ 'STDERR' } 
    and $qalter{ 'STDERR' } =~ /qalter: illegal -p value/), 
   "Checking for failure of '$p_cmd'");

# Check qalter -p negative boundry 
$priority  = -1024; # This should pass
$p_cmd     = "qalter -p $priority $job_id";
%qalter    = run_and_check_cmd($p_cmd);

$fx_cmd   = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($fx_cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Priority' } eq $priority, "Checking if '$p_cmd' was successful");

$priority  = -1025; # This should fail
$p_cmd     = "qalter -p $priority $job_id";
%qalter    = runCommand($p_cmd);
ok((    defined $qalter{ 'STDERR' } 
    and $qalter{ 'STDERR' } =~ /qalter: illegal -p value/), 
   "Checking for failure of '$p_cmd'");

# Delete the jobs
delJobs($job_id);
