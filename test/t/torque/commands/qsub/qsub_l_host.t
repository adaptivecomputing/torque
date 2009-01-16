#!/usr/bin/perl

use CRI::Test;
plan('no_plan');
setDesc('Qsub -l host');
use strict;
use warnings;

#? apitest
#* This tests qsub -l host=myHost


# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -l host=myHost');
ok($jobId{'EXIT_CODE'} == 0,'Checking if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for Resource_List.host
my $host = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die("Couldn't run qstat");
my @stdout = join("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Resource_List.host = (.*)/)
   {
      $host = $1;
   }
}

die("Expected Resource_List.host [myHost] but found [$host]") unless cmp_ok($host,'eq',"myHost",'Checking for Resource_List.host [myHost]');
