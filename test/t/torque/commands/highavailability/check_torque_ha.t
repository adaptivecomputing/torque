#!/usr/bin/perl
###########################################################
# This test hammers pbs_server --ha by spawning off
# servers and killing them, all the while submiting jobs.
# after the test runs, it checks that all submitted jobs
# successfully finished.
#
# author: smcquay
# notes:
#   1) make sure the user var is set in clustertest.props
#   2) this test doesn't work yet ... torque looses jobs
#
# circa feb '08
###########################################################

use CRI::Test;
use Scalar::Util;
plan('no_plan');
setDesc("test for pbs_server --ha");
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


die "************************************************
read the header to this file ...
************************************************\n";

`killall -9 pbs_server`;

# my $total_checks = $props->get_property("torque.ha.runs");
my $total_checks = 5;
my $user = $props->get_property("user");
my %out = ();
my @pbsses = ();
my $a = "";
my $server_to_be_killed = 0;
my $min_sleep_time = 5;
my $range_for_sleep_time = 5;
my $i = 0;

my @jobs = ();
my $slp_cmd = "sleep 2";

%out = runCommand('qstat');
ok($out{"EXIT_CODE"} != 0, "no existing torque");

%out = runCommand('pbs_server --ha');
ok($out{"EXIT_CODE"} == 0, "starting torque $i");
$i++;
%out = runCommand('pbs_server --ha');
ok($out{"EXIT_CODE"} == 0, "starting torque $i");

sleep int(rand($range_for_sleep_time)) + $min_sleep_time;

while ($i < $total_checks)
{
  print "\n";
  %out = runCommandAs($user, "echo \"$slp_cmd\n date '+%H\:%M\:%S'\" | qsub");
  my($tmp) = ($out{"STDOUT"} =~ /(\d+)./);
  push(@jobs, $tmp);

  $i++;

  %out = runCommand("pgrep pbs_server");
  @pbsses = ();
  my @tmp_pbsses = split(/\n/, $out{"STDOUT"});
  foreach(@tmp_pbsses){
  	$_ =~ m/([0-9]*)/;
  	push(@pbsses,$1);	
  }

  ok(@pbsses == 2, "check on number of torques; @pbsses") or die "should always have 2 torques\n";

  $server_to_be_killed = shift(@pbsses);

  %out = runCommand("kill -9 $server_to_be_killed");
  ok($out{"EXIT_CODE"} == 0, "killing a torque: $server_to_be_killed") or die "should kill properly\n";

  sleep int(rand($range_for_sleep_time)) + $min_sleep_time;

  %out = runCommand('pbs_server --ha');
  ok($out{"EXIT_CODE"} == 0, "starting torque $i");

  sleep int(rand($range_for_sleep_time)) + $min_sleep_time;
  %out = runCommand("qstat");
  ok($out{"EXIT_CODE"} == 0, "qstat is responding well");

  sleep int(rand($range_for_sleep_time)) + $min_sleep_time;
}

print "giving jobs time to die\n";
sleep 120;

##############################################
# this next portion of code is meant
# to test that all jobs completed despite
# the hammering we just did to torque
##############################################

my $output = `qstat`;

print "\n\nthis qstat output provided for debugging purposes:\n\n$output\n\n";

my %little_hash = ();
my @lst = split(/\n/, $output);
my $cid = "";
my $ctp = "";

for my $i (@lst)
{
  ($cid, $ctp) = $i =~ /(\d+).*(\w) batch/;

  if($cid)
  {
#   print "\t* $cid => $ctp\n";
    $little_hash{$cid} = $ctp;
  }
}

print "these jobs were submitted, and should be completed:";
for my $i (@jobs)
{
  print " $i";
}
print "\n\n";

# all the jobs in @jobs should be in %little_hash, or else torque did something silly
for my $i (@jobs)
{
  ok($little_hash{$i} eq "C", "test for job completion $i=>$little_hash{$i} (should be C)") or die "we lost a job!!";
}

# clean up and exit
%out = runCommand("killall -9 pbs_server");
ok($out{"EXIT_CODE"} == 0, "killing all torques");
