#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

use Torque::Util qw( job_info );
use Torque::Job::Ctrl   qw( delJobs  );

plan('no_plan');
setDesc('qsub -D');

# Run the qsub -D command
my $dir  = '/tmp';
my $user = $props->get_property('User.1');
my $cmd  = "echo sleep 30 | qsub -D $dir";
my %qsub = runCommandAs($user, $cmd);

# Check the exit code
ok($qsub{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'")
 or die ("'$cmd' failed: $qsub{ 'STDERR' }");

# Get the job_id
my $job_id = $qsub{ 'STDOUT' };

# Detaint the job_id
if ($job_id =~ /^(\d+\.[\w\-\.]+)/)
 {
 
 $job_id = $1;

 }
else
 {
 
 die("Bad job_id: '$job_id'");

 }

my %job_info = job_info($job_id);

is($job_info{ $job_id }{ 'variable_list' }{ 'pbs_o_rootdir' }, $dir, "Checking the environment variable 'pbs_o_rootdir'");

# Delete the job
delJobs($job_id);
