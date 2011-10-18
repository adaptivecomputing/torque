#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc('Remove Torque Directories');

my $torque_home = $props->get_property('Torque.Home.Dir');
my @remote_hosts = split ',', $props->get_property('Torque.Remote.Nodes');

foreach my $host (undef, @remote_hosts)
{
  my $ec;
  my $endtime = time + 10;
  do
  {
    $ec = runCommand("rm -rf $torque_home", host => $host);

  }until( $ec == 0 || time > $endtime );

  fail("Removal of Torque home directory Failed on ".($host || 'localhost')) unless $ec == 0;
}
