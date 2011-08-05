#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc('Qsub -a');

# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('User.1'),'echo /bin/sleep 60 | qsub -a 201510220700');
ok($jobId{'PTID'} != 0,'Checking if qsub submission worked') or die("qsub failed with rc=$jobId{'PTID'}");

# Run qstat -f on the submitted job and look for job_state and Execution_Time
my $jobState = '';
my $executionTime = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('User.1'),"qstat -f $jobId");
ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die("Couldn't run qstat");
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /job_state = (.*)/)
   {
      $jobState = $1;
   }
   elsif ($line =~ /execution_time = (.*)/)
   {
      $executionTime = $1;
   }
}

cmp_ok($jobState,'eq',"W",'Checking that job state is [W]');
cmp_ok($executionTime,'eq',"Thu Oct 22 07:00:00 2015",'Checking that Execution time is [Thu Oct 22 07:00:00 2015]'); 
