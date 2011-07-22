#! /usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc('Qsub stdin Test');

# Submit a job with qsub and get its job id
my $user  = $props->get_property('User.1');
my %jobId = runCommandAs($user,'echo /bin/hostname | qsub');
ok($jobId{'EXIT_CODE'} == 0,'Checking if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for Job_Name
my $jobName = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($user,"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Job_Name = (.*)/)
   {
      $jobName = $1;
   }
}

die("Expected Job_Name [STDIN] but found [$jobName]") unless cmp_ok($jobName,'eq',"STDIN",'Checking job name');
