#!/usr/bin/perl

use strict;
use warnings;

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
setDesc("qalter -h");

# Variables
my $h_cmd;
my $fx_cmd;
my %qstat;
my %qstat_fx;
my %qalter;
my $job_id;
my $hold_type;

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('torque.home.dir') . '/bin/'
                 };

$job_id = submitSleepJob($job_params);

# Run a job
runJobs($job_id);

# Check qalter -h u
$hold_type = 'u';
$h_cmd     = "qalter -h $hold_type $job_id";
%qalter    = run_and_check_cmd($h_cmd);

$fx_cmd   = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($fx_cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Hold_Types' } eq $hold_type, "Checking if '$h_cmd' was successful");

# Check qalter -h s
$hold_type = 's';
$h_cmd     = "qalter -h $hold_type $job_id";
%qalter    = run_and_check_cmd($h_cmd);

$fx_cmd   = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($fx_cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Hold_Types' } eq $hold_type, "Checking if '$h_cmd' was successful");

# Check qalter -h o
$hold_type = 'o';
$h_cmd     = "qalter -h $hold_type $job_id";
%qalter    = run_and_check_cmd($h_cmd);

$fx_cmd   = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($fx_cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Hold_Types' } eq $hold_type, "Checking if '$h_cmd' was successful");

# Check qalter -h sou
$hold_type = 'sou';
$h_cmd     = "qalter -h $hold_type $job_id";
%qalter    = run_and_check_cmd($h_cmd);

$fx_cmd   = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($fx_cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Hold_Types' } eq $hold_type, "Checking if '$h_cmd' was successful");

# Check qalter -h n
$hold_type = 'n';
$h_cmd     = "qalter -h $hold_type $job_id";
%qalter    = run_and_check_cmd($h_cmd);

$fx_cmd   = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($fx_cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Hold_Types' } eq $hold_type, "Checking if '$h_cmd' was successful");

# Delete the jobs
delJobs($job_id);
