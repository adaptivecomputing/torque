#! /usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc('Qsub -l pmem');

# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('User.1'),'echo /bin/sleep 60 | qsub -l pmem=100mb');
ok($jobId{'EXIT_CODE'} == 0,'Chekcing if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for Resource_List.pmem
my $pmem = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('User.1'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = join("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Resource_List.pmem = (.*)/)
   {
      $pmem = $1;
   }
}

die("Expected Resource_List.pmem [100m] but found [$pmem]") unless cmp_ok($pmem,'eq',"100mb",'Checking for Resource_List.pmem [100m]');


