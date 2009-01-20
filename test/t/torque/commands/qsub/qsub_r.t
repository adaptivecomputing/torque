#! /usr/bin/perl

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

setDesc('Qsub -r');

#? apitest
#* This tests qsub -r n


# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -r n');
ok($jobId{'EXIT_CODE'} == 0,'Chekcing if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for Rerunable
my $rerunable = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Rerunable = (.*)/)
   {
      $rerunable = $1;
   }
}

die("Expected Rerunable [False] but found [$rerunable]") unless cmp_ok($rerunable,'eq',"False",'Checking that it is not re-runable');
