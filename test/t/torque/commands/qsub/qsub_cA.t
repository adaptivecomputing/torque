#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc('Qsub -A');

# Submit a job with qsub and get its job id
my %jobId = runCommandAs($props->get_property('User.1'),'echo /bin/sleep 60 | qsub -A myAccount');

die("qsub failed: [rc=$?]") 
  unless ok(0==$jobId{'EXIT_CODE'},"Checking that qsub -A submitted correctly");

# Run qstat -f on the submitted job and look for Account_Name
my $accountName = '';

# Untaint qsub output
my $jobId = $jobId{'STDOUT'};
$jobId = $1 if ($jobId =~ /(.*)/);
chomp($jobId);

my %qstat = runCommandAs($props->get_property('User.1'),"qstat -f $jobId");
ok($qstat{'EXIT_CODE'} != 999,'Checking that qstat ran') 
  or die("Couldn't run qstat");
my @lines = split("\n",$qstat{'STDOUT'});
foreach my $line (@lines)
{
   if ($line =~ /account_name = (.*)/)
   {
      $accountName = $1;
   }
}

cmp_ok($accountName,'eq',"myAccount",'Checking that account name is correct');
