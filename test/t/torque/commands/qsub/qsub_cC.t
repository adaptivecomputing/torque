#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

use Torque::Job::Ctrl   qw( delJobs   );
use Torque::Util qw( job_info  );
plan('no_plan');
setDesc('qsub -C');

# Variables
my $directive_script = test_lib_loc().'/../t/torque/test_programs/directive_test.pl';

# Run the command
my $user = $props->get_property('User.1');
my $cmd  = "qsub -C '#TEST' $directive_script";
my %qsub = runCommandAs($user, $cmd);

# Check that the command ran
ok($qsub{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'")
  or die("'$cmd' failed: $qsub{ 'STDERR' }");

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

my $wcl = $job_info{$job_id}{resource_list}{walltime};
is($wcl, '00:00:30', "Checking that the directive worked correctly (Walltime set as specified)");

# Stop the job
delJobs($job_id);
