#! /usr/bin/perl

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";

setDesc('Qsub -M');

#? apitest
#* This tests qsub -M apitest@clusterresources.com


# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('moab.user.one'),'echo /bin/sleep 60 | qsub -M apitest\@clusterresources.com');
ok($jobId{'EXIT_CODE'} == 0,'Chekcing if qsub submission worked') or die("qsub failed with rc=$jobId{'EXIT_CODE'}");

# Run qstat -f on the submitted job and look for Mail_Users
my $mailUsers = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('moab.user.one'),"qstat -f $jobId");

ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') or die('Couldn\'t run qstat');
my @stdout = join("\n",$qstat{'STDOUT'});
foreach my $line (@stdout)
{
   if ($line =~ /Mail_Users = (.*)/)
   {
      $mailUsers = $1;
   }
}

die("Expected Mail_Users [apitest\@clusterresources.com] but found [$mailUsers]") unless cmp_ok($mailUsers,'eq',"apitest\@clusterresources.com",'Checking for expected Mail_Users');


