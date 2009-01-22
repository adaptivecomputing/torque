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

# Other modules
use Sys::Hostname;

# Test Description
plan('no_plan');
setDesc("qalter -e");

# Variables
my $cmd;
my %qstat;
my %qstat_fx;
my %qalter;
my $job_id;
my $err_path;
my $err_host_path;
my $cwd;
my $host           = hostname();
my $torque_host    = $props->get_property('Test.Host');     

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
                 };

$job_id = submitSleepJob($job_params);

# Run the job
runJobs($job_id);

# Set variables to relative paths
$err_path      = "$job_id.err";
$err_host_path = "$host:$err_path";

# Alter the job error path with a relative path and without a host
$cmd         = "qalter -e $err_path $job_id";
%qalter      = run_and_check_cmd($cmd); 

$cwd      = $qalter{ 'CWD' };
$cmd      = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Error_Path' } eq "$host:$cwd/$err_path", 
   "Checking for the new error file when specified without a host and with an relative path")
   or diag("Expected: '$host:$cwd/$err_path\nGot: $qstat_fx{ $job_id }{ 'Error_Path' }'");

# Alter the job error path with a relative path and a host
$cmd         = "qalter -e $err_host_path $job_id";
%qalter      = run_and_check_cmd($cmd); 

$cmd      = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Error_Path' } eq "$err_host_path", 
   "Checking for the new error file when specified with a host and with a relative path");

# Set variables to a absolute path
$err_path      = "/tmp/$job_id.err";
$err_host_path = "$host:$err_path";

# Alter the job error path with a relative path and without a host
$cmd         = "qalter -e $err_path $job_id";
%qalter      = run_and_check_cmd($cmd); 

$cmd      = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Error_Path' } eq "$host:$err_path", 
   "Checking for the new error file when specified without a host and with an abosolute path");

# Alter the job error path with a relative path and a host
$cmd         = "qalter -e $err_host_path $job_id";
%qalter      = run_and_check_cmd($cmd); 

$cmd      = "qstat -f -x $job_id";
  %qstat    = run_and_check_cmd($cmd);
  %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
  ok($qstat_fx{ $job_id }{ 'Error_Path' } eq "$err_host_path", 
     "Checking for the new error file when specified with a host and with an abosolute path");

# Delete the job
delJobs($job_id);
