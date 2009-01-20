#! /usr/bin/perl

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";

setDesc('Qsub -W depend');

#? apitest
#* This tests qsub -W depend=afterok:16397


# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -W depend=afterok:16397');
ok($jobId{'EXIT_CODE'} == 0,'Chekcing if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for job_state, Hold_Types and etime
my $jobState = '';
my $holdTypes = '';
my $etime; 

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /job_state = (.*)/)
   {
      $jobState = $1;
   }
   elsif ($line =~ /Hold_Types = (.*)/)
   {
      $holdTypes = $1;
   }
   elsif ($line =~ /etime = (.*)/)
   {
      $etime = $1;
   }
}

die("Expected job_state [H] but found [$jobState]") unless cmp_ok($jobState,'eq',"H",'Checking job state');
die("Expected Hold_Types [u] but found [$holdTypes]") unless cmp_ok($holdTypes,'eq',"u",'Checking hold types');
die("Expected etime to be undefined but found [$etime]") unless ok(!defined($etime),'Verifying that etime is not defined');
