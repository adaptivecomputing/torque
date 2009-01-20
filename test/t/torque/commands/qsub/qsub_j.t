#!/usr/bin/perl

use CRI::Test;
plan('no_plan');
setDesc('Qsub -j');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


#? apitest
#* This tests qsub -j oe

# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -j oe');
ok($jobId{'EXIT_CODE'} == 0,'Checking if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for Join_Path
my $joinPath = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die("Couldn't run qstat");
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Join_Path = (.*)/)
   {
      $joinPath = $1;
   }
}

die("Expected Join_Path [oe] but found [$joinPath]") unless cmp_ok($joinPath,'eq',"oe",'Checking for Join_Path [oe]');
