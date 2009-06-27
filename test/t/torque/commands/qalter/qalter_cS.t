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
setDesc("qalter -S");

# Variables
my $s_cmd;
my $fx_cmd;
my %qstat;
my %qstat_fx;
my %qalter;
my $job_id;
my $path_list;

my $shell_path = '/bin/bash';
my $host       = $props->get_property('Test.Host');

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
                 };

$job_id = submitSleepJob($job_params);

# Run a job
runJobs($job_id);

# Check qalter -S n
$path_list = "$shell_path,$shell_path\@$host";
$s_cmd     = "qalter -S $path_list $job_id";
%qalter    = run_and_check_cmd($s_cmd);

$fx_cmd   = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($fx_cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Shell_Path_List' } eq $path_list, "Checking if '$s_cmd' was successful");

# Delete the jobs
delJobs($job_id);
