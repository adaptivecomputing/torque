#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc('Qsub -e');

# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -e /home/' . $props->get_property('moab.user.one') . '/myErrorFile');
ok($jobId{'EXIT_CODE'} == 0,'Checking if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for job_state and Execution_Time
my $errorPath = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die("Couldn't run qstat");
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Error_Path = (.*)/)
   {
      $errorPath = $1;
   }
}

die("Expected Error_Path [mauna:/home/" . $props->get_property('moab.user.one') . "/myErrorFile] but found [$errorPath]") unless cmp_ok($errorPath,'eq', $props->get_property('MoabHost') . ":/home/" . $props->get_property('moab.user.one') . "/myErrorFile",'Looking for expected error file');
