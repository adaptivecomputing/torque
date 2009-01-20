#! /usr/bin/perl

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

setDesc('Qsub -l walltime');

#? apitest
#* This tests qsub -l walltime=4:00:00


# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -l walltime=4:00:00');
ok($jobId{'EXIT_CODE'} == 0,'Chekcing if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for Resource_List.walltime
my $walltime = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = join("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Resource_List.walltime = (.*)/)
   {
      $walltime = $1;
   }
}

die("Expected Resource_List.walltime [04:00:00] but found [$walltime]") unless cmp_ok($walltime,'eq',"04:00:00",'Checking for Resource_List.walltime [04:00:00]');


