#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;

# Test Description
plan('no_plan');
setDesc('Momctl -f <HOSTLIST_FILE> -C');

# Variables
my %momctl;
my $hostlistfile = $props->get_property('mom.host.list.file');
my @hosts        = split(/,|\s/, `cat $hostlistfile`);

%momctl = runCommand("momctl -f $hostlistfile -C", test_success => 1);

# Check the stdout
my $stdout = $momctl{ 'STDOUT' };
foreach my $host (@hosts)
  {

  ok($stdout =~ /^mom $host successfully cycled cycle forced$/m, "Checking output of 'momctl -f $hostlistfile -C' for $host");

  }

