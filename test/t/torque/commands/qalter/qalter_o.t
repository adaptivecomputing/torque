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
setDesc("qalter -o");

# Variables
my $cmd;
my %qstat;
my %qstat_fx;
my %qalter;
my @job_ids;
my $job_id;
my $out_path;
my $out_host_path;
my $cwd;
my $host           = hostname();
my $torque_host    = $props->get_property('Test.Host');     

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
                 };

$job_id = submitSleepJob($job_params);

# Run a job
runJobs($job_id);

# Set variables to non-absolute paths
$out_path      = "$job_id.out";
$out_host_path = "$host:$out_path";

# Alter the job output path with a non-absolute path and without a host
$cmd         = "qalter -o $out_path $job_id";
%qalter      = run_and_check_cmd($cmd); 

$cwd      = $qalter{ 'CWD' };
$cmd      = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Output_Path' } eq "$host:$cwd/$out_path", 
   "Checking for the new output file when specified without a host and with an non-absolute path");

# Alter the job output path with a non-absolute path and a host
$cmd         = "qalter -o $out_host_path $job_id";
%qalter      = run_and_check_cmd($cmd); 

$cmd      = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Output_Path' } eq "$out_host_path", 
   "Checking for the new output file when specified with a host and with a non-absolute path");

# Set variables to a absolute path
$out_path      = "/tmp/$job_id.out";
$out_host_path = "$host:$out_path";

# Alter the job output path with a non-absolute path and without a host
$cmd         = "qalter -o $out_path $job_id";
%qalter      = run_and_check_cmd($cmd); 

$cmd      = "qstat -f -x $job_id";
%qstat    = run_and_check_cmd($cmd);
%qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Output_Path' } eq "$host:$out_path", 
   "Checking for the new output file when specified without a host and with an abosolute path");

# Alter the job output path with a non-absolute path and a host
$cmd         = "qalter -o $out_host_path $job_id";
%qalter      = run_and_check_cmd($cmd); 

$cmd      = "qstat -f -x $job_id";
  %qstat    = run_and_check_cmd($cmd);
  %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });
  ok($qstat_fx{ $job_id }{ 'Output_Path' } eq "$out_host_path", 
     "Checking for the new output file when specified with a host and with an abosolute path");

# Delete the jobs
delJobs($job_id);
