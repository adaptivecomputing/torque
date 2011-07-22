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
setDesc("qalter -W (sync paramaters)");

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
                   'user'       => $props->get_property('User.1'),
                   'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
                 };

my $job1 = submitSleepJob($job_params);
my $job2 = submitSleepJob($job_params);

# Test synccount
diag("Test synccount");
$depend  = "synccount:1";
$w_cmd   = "qalter -W depend=$depend $job1";
%qalter     = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job1";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job1 }{ 'depend' } eq $depend, "Checking if '$w_cmd' was successful");

# Test syncwith
diag("Test syncwith");
$depend  = "syncwith:$job1";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job2 }{ 'depend' } =~ /${depend}/, "Checking if '$w_cmd' was successful");

# Reset synccount
diag("Reset synccount");
$depend  = "synccount";
$w_cmd   = "qalter -W depend=$depend $job1";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job1";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job1 }{ 'depend' } eq 'synccount:0', "Checking if '$w_cmd' was successful");

# Reset syncwith
diag("Reset syncwith");
$depend  = "syncwith";
$w_cmd   = "qalter -W depend=$depend $job2";
%qalter  = run_and_check_cmd($w_cmd);

$fx_cmd     = "qstat -f -x $job2";
%qstat      = run_and_check_cmd($fx_cmd);
%qstat_fx   = parse_qstat_fx($qstat{ 'STDOUT' });
ok(! defined $qstat_fx{ $job2 }{ 'depend' }, "Checking if '$w_cmd' was successful");

# Delete the jobs
delJobs($job1, $job2);
