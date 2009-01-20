#!/usr/bin/perl

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";

setDesc('Qsub -a');

#? apitest
#* This tests qsub -a 10220700

# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -a 201510220700');
ok($jobId{'PTID'} != 0,'Checking if qsub submission worked') or die("qsub failed with rc=$jobId{'PTID'}");

# Run qstat -f on the submitted job and look for job_state and Execution_Time
my $jobState = '';
my $executionTime = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");
ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die("Couldn't run qstat");
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /job_state = (.*)/)
   {
      $jobState = $1;
   }
   elsif ($line =~ /Execution_Time = (.*)/)
   {
      $executionTime = $1;
   }
}

die("Expected job_state [W] but found [$jobState]") unless cmp_ok($jobState,'eq',"W",'Checking that job state is [W]');
die("Expected Execution_Time [Thu Oct 22 07:00:00 2015] but found [$executionTime]") unless cmp_ok($executionTime,'eq',"Thu Oct 22 07:00:00 2015",'Checking that Execution time is [Thu Oct 22 07:00:00 2015]'); 
