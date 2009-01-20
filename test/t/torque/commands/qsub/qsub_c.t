#!/usr/bin/perl

use CRI::Test;
plan('no_plan');
setDesc('Qsub -c');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


#? apitest
#* This tests qsub -c c=10

# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -c c=10');
ok($jobId{'EXIT_CODE'} == 0,'Checking if qsub submission worked') or die("qsub failed: [rc=$?]");

# Run qstat -f on the submitted job and look for job_state and Execution_Time
my $checkPoint = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die("Couldn't run qstat");
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Checkpoint = (.*)/)
   {
      $checkPoint = $1;
   }
}

die("Expected Checkpoint [c=10] but found [$checkPoint]") unless cmp_ok($checkPoint,'eq',"c=10",'Looking for checkpoint c=10');
