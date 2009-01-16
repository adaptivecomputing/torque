#! /usr/bin/perl

use CRI::Test;
plan('no_plan');
use strict;
use warnings;
setDesc('Qsub -l arch');

#? apitest
#* This tests qsub -l arch=linux


# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -l arch=linux');
ok($jobId{'EXIT_CODE'} == 0,'Chekcing if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for Resource_List.arch
my $arch = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");
ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = join("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Resource_List.arch = (.*)/)
   {
      $arch = $1;
   }
}

die("Expected Resource_List.arch [linux] but found [$arch]") unless cmp_ok($arch,'eq',"linux",'Found Linux architecture');


