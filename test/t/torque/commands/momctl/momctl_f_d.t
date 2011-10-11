#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Module
use CRI::Test;
use Torque::Test::Momctl::Diag qw(test_mult_diag);

# Test Case Description
plan('no_plan');
setDesc('Momctl -f <HOSTLISTFILE> -d <LOGLEVEL>');

# Other Variables
my %momctl;
my $mom_host_list_file = $props->get_property('mom.host.list.file');
my @mom_hosts          = split(/,|\s/, `cat $mom_host_list_file`);

for(0 .. 3)
{
  %momctl = runCommand("momctl -f $mom_host_list_file -d $_", test_success => 1);

  test_mult_diag($_, $momctl{ 'STDOUT' }, \@mom_hosts);
}
