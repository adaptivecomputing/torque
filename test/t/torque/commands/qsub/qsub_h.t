#!/usr/bin/perl

use CRI::Test;
plan('no_plan');
setDesc('Qsub -h');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


#? apitest
#* This tests qsub -h


# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -h');
ok($jobId{'EXIT_CODE'} == 0,'Checking if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for job_state and Execution_Time
my $holdTypes = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die("Couldn't run qstat");
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Hold_Types = (.*)/)
   {
      $holdTypes = $1;
   }
}

die("Expected Hold_Types [u] but found [$holdTypes]") unless cmp_ok($holdTypes,'eq',"u",'Checking that Hold Types are as expected');
