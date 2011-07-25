#! /usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc('Qsub -o');

# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('User.1'),'echo /bin/sleep 60 | qsub -o /home/' . $props->get_property('User.1') . '/myOutputFile');
ok($jobId{'EXIT_CODE'} == 0,'Chekcing if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for job_state and Execution_Time
my $outputPath = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('User.1'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Output_Path = (.*)/)
   {
      $outputPath = $1;
   }
}

die("Expected Output_Path [" . $props->get_property('Test.Host') . ":/home/".$props->get_property('User.1')."/myOutputFile] but found [$outputPath]") unless cmp_ok($outputPath,'eq',$props->get_property('Test.Host') . ":/home/".$props->get_property('User.1')."/myOutputFile",'Checking for expected output file');

