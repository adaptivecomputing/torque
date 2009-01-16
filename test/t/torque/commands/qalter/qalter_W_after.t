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
setDesc("qalter -W (after paramaters)");

# Variables
my $w_cmd;
my $fx_cmd;
my %qstat;
my %qstat_fx;
my %qalter;
my $group_list;
my $depend;

my $host   = $props->get_property('MoabHost');

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('torque.home.dir') . '/bin/'
                 };

my $job1 = submitSleepJob($job_params);
my $job2 = submitSleepJob($job_params);

# Test after
diag("Test after");
$depend  = "after:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset after
diag("Reset after");
$depend  = "after";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(! defined $qstat_fx{ $job2 }{ 'depend' }, "Checking if '$w_cmd' was successful");

# Test afterok
diag("Test afterok");
$depend  = "afterok:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset afterok
diag("Reset afterok");
$depend  = "afterok";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(! defined $qstat_fx{ $job2 }{ 'depend' }, "Checking if '$w_cmd' was successful");

# Test afternotok
diag("Test afternotok");
$depend  = "afternotok:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset afternotok
diag("Reset afternotok");
$depend  = "afternotok";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(! defined $qstat_fx{ $job2 }{ 'depend' }, "Checking if '$w_cmd' was successful");

# Test afteranyok
diag("Test afteranyok");
$depend  = "afterany:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset afteranyok
diag("Reset afteranyok");
$depend  = "afterany";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(! defined $qstat_fx{ $job2 }{ 'depend' }, "Checking if '$w_cmd' was successful");

# Delete the jobs
delJobs($job1, $job2);
