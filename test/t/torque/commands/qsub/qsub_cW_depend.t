#! /usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc('Qsub -W depend');

# Test failure case
my %jobId = runCommandAs($props->get_property('User.1'),'echo /bin/sleep 60 | qsub -W depend=afterok:16397', test_fail => 1);

is($jobId{STDERR}, "qsub: submit error (Unknown Job Id)\n", 'Expected Error Message returned');

#TODO: create a new case for a generic job dependency
=head
# Run qstat -f on the submitted job and look for job_state, Hold_Types and etime
my $jobState = '';
my $holdTypes = '';
my $etime; 

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('User.1'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /job_state = (.*)/)
   {
      $jobState = $1;
   }
   elsif ($line =~ /hold_types = (.*)/)
   {
      $holdTypes = $1;
   }
   elsif ($line =~ /etime = (.*)/)
   {
      $etime = $1;
   }
}

cmp_ok($jobState,'eq',"H",'Checking job state');
cmp_ok($holdTypes,'eq',"u",'Checking hold types');
ok(!defined($etime),'Verifying that etime is not defined');
