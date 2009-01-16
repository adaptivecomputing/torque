#! /usr/bin/perl

use CRI::Test;
plan('no_plan');
use strict;
use warnings;
setDesc('Qsub -S');

#? apitest
#* This tests qsub -S /bin/bash


# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -S /bin/bash');
ok($jobId{'EXIT_CODE'} == 0,'Chekcing if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for Shell_Path_List
my $shellPathList = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Shell_Path_List = (.*)/)
   {
      $shellPathList = $1;
   }
}

die("Expected Shell_Path_List [/bin/bash] but found [$shellPathList]") unless cmp_ok($shellPathList,'eq',"/bin/bash",'Checking shell path list');
