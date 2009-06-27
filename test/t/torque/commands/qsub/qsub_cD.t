#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

use Torque::Util qw( job_info );
use Torque::Job::Ctrl   qw( delJobs  );

plan('no_plan');
setDesc('qsub -D');

# Run the qsub -D command
my $dir  = '/tmp';
my $user = $props->get_property('torque.user.one');
my $cmd  = "echo sleep 30 | qsub -D $dir";
my %qsub = runCommandAs($user, $cmd);

# Check the exit code
ok($qsub{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'")
 or die ("'$cmd' failed: $qsub{ 'STDERR' }");

# Get the job_id
my $job_id = $qsub{ 'STDOUT' };

# Detaint the job_id
if ($job_id =~ /^(\d+\.[\w-]+)/)
 {
 
 $job_id = $1;

 }
else
 {
 
 die("Bad job_id: '$job_id'");

 }

my %job_info = job_info($job_id);

ok($job_info{ $job_id }{ 'Variable_List' }{ 'PBS_O_ROOTDIR' } eq $dir, "Checking the environment variable 'PBS_O_ROOTDIR' for the directory: '$dir'")
  or diag("\$job_info{ '$job_id' }{ 'Variable_List' }{ 'PBS_O_ROOTDIR' } = $job_info{ $job_id }{ 'Variable_List' }{ 'PBS_O_ROOTDIR' }");

# Delete the job
delJobs($job_id);
