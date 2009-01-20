#! /usr/bin/perl

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";

setDesc('Qsub -o');

#? apitest
#* This tests qsub -o /home/apitest/myOutputFile


# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -o /home/' . $props->get_property('moab.user.one') . '/myOutputFile');
ok($jobId{'EXIT_CODE'} == 0,'Chekcing if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for job_state and Execution_Time
my $outputPath = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Output_Path = (.*)/)
   {
      $outputPath = $1;
   }
}

die("Expected Output_Path [" . $props->get_property('MoabHost') . ":/home/".$props->get_property('moab.user.one')."/myOutputFile] but found [$outputPath]") unless cmp_ok($outputPath,'eq',$props->get_property('MoabHost') . ":/home/".$props->get_property('moab.user.one')."/myOutputFile",'Checking for expected output file');

