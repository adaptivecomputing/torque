#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;

# Test Description
plan('no_plan');
setDesc('Momctl -p $port -f <HOSTLIST_FILE> -C');

# Variables
my %momctl;
my $hostlistfile = $props->get_property('mom.host.list.file');
my $port         = $props->get_property('mom.host.port');
my @hosts        = split(/,|\s/, `cat $hostlistfile`);

%momctl = runCommand("momctl -p $port -f $hostlistfile -C");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -p $port -f $hostlistfile -C' ran")
  or die "Couldn't run momctl -p $port -f $hostlistfile -C";

# Check the stdout
my $stdout = $momctl{ 'STDOUT' };
foreach my $host (@hosts)
  {

  ok($stdout =~ /mom ${host} successfully cycled cycle forced/i, "Checking output of 'momctl -p $port -f $hostlistfile -C' for $host");

  }

