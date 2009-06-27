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
use Torque::Util         qw( run_and_check_cmd 
                                  list2array        );
use Torque::Util::Qstat  qw( parse_qstat_fx    );

# Test Description
plan('no_plan');
setDesc("qalter -W (before paramaters)");

# Variables
my $w_cmd;
my $fx_cmd;
my %qstat;
my %qstat_fx;
my %qalter;
my $group_list;
my $depend;

my $host   = $props->get_property('Test.Host');

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
                 };
 
my $job1 = submitSleepJob($job_params);
my $job2 = submitSleepJob($job_params);

# Test before
$depend  = "before:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset before
$depend  = "before";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(! defined $qstat_fx{ $job2 }{ 'depend' }, "Checking if '$w_cmd' was successful");

# Test beforeok
$depend  = "beforeok:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset beforeok
$depend  = "beforeok";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(! defined $qstat_fx{ $job2 }{ 'depend' }, "Checking if '$w_cmd' was successful");

# Test beforenotok
$depend  = "beforenotok:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset beforenotok
$depend  = "beforenotok";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(! defined $qstat_fx{ $job2 }{ 'depend' }, "Checking if '$w_cmd' was successful");

# Test beforeany
$depend  = "beforeany:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset beforeany
$depend  = "beforeany";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(! defined $qstat_fx{ $job2 }{ 'depend' }, "Checking if '$w_cmd' was successful");

# Test on before
$depend  = "on:1,before:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset on before
$depend  = "on,before";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(defined $qstat_fx{ $job2 }{ 'depend' } eq 'on:0', "Checking if '$w_cmd' was successful");

# Test on beforeok
$depend  = "on:1,beforeok:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset on beforeok
$depend  = "on,beforeok";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(defined $qstat_fx{ $job2 }{ 'depend' } eq 'on:0', "Checking if '$w_cmd' was successful");

# Test on beforenotok
$depend  = "on:1,beforenotok:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset on beforenotok
$depend  = "on,beforenotok";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(defined $qstat_fx{ $job2 }{ 'depend' } eq 'on:0', "Checking if '$w_cmd' was successful");

# Test on beforeany
$depend  = "on:1,beforeany:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset on beforeany
$depend  = "on,beforeany";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(defined $qstat_fx{ $job2 }{ 'depend' } eq 'on:0', "Checking if '$w_cmd' was successful");

# Delete the jobs
delJobs($job1, $job2);
