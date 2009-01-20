#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


use CRI::Test;

use Torque::Job::Ctrl   qw( delJobs   );
use Torque::Test::Utils qw( job_info  );

plan('no_plan');
setDesc('qsub -C');

# Variables
my $directive_script = $props->get_property('test.base') . '/torque/test_programs/directive_test.pl';

# Run the command
my $user = $props->get_property('moab.user.one');
my $cmd  = "qsub -C TEST $directive_script";
my %qsub = runCommandAs($user, $cmd);

# Check that the command ran
ok($qsub{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'")
  or die("'$cmd' failed: $qsub{ 'STDERR' }");

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

my $mem = $job_info{ $job_id }{ 'Resource_List' }{ 'mem' } 
  || '';
ok($mem eq '1024mb', "Checking that the directive worked correctly");

# Stop the job
delJobs($job_id);
