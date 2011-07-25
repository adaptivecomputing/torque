#! /usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc('Qsub -u');

# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('User.1'),'echo /bin/sleep 60 | qsub -u ' . $props->get_property('User.1'));
ok($jobId{'EXIT_CODE'} == 0,'Chekcing if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for User_List
my $userList = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('User.1'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /User_List = (.*)/)
   {
      $userList = $1;
   }
}

die("Expected User_List [".$props->get_property('User.1')."] but found [$userList]") unless cmp_ok($userList,'eq',$props->get_property('User.1'),'Checking User_List');
