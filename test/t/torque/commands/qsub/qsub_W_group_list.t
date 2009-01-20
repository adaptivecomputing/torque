#! /usr/bin/perl

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

setDesc('Qsub -W group_list');

#? apitest
#* This tests qsub -W group_list=apitest


# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -W group_list=' . $props->get_property('moab.user.one.group'));
ok($jobId{'EXIT_CODE'} == 0,'Checking if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for group_list
my $groupList = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = split("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /group_list = (.*)/)
   {
      $groupList = $1;
   }
}

die("Expected group_list [".$props->get_property('moab.user.one.group')."] but found [$groupList]") unless cmp_ok($groupList,'eq',$props->get_property('moab.user.one.group'),'Checking group list');
