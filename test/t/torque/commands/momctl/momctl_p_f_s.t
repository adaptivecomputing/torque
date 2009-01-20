#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;
use Torque::Ctrl;
use Torque::Test::Utils qw(hostlist2array);

# Test Description
plan('no_plan');
setDesc('Momctl -p <PORT> -f <HOSTLISTFILE> -s');

# Variables
my %momctl;

my $host_list_file = $props->get_property('mom.single.host.list.file');
my $port           = $props->get_property('mom.host.port');
my @hosts          = hostlist2array($host_list_file);
my $cmd            = "momctl -p $port -f $host_list_file -s";

%momctl = runCommand($cmd);
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that '$cmd' ran")
  or die "Couldn't run '$cmd'";

# Perform the tests
my $stdout = $momctl{ 'STDOUT' };

foreach my $host (@hosts)
  {

  ok($stdout =~ /shutdown request successful on ${host}/i, "Checking output of '$cmd'"                    );

  sleep 15;
  my $pgrep = `pgrep -x pbs_mom`;
  ok($pgrep !~ /\d+/,                                      "Checking that the pbs_mom pid does not exists");

  } # END foreach my $host (\@hosts)

# Restart pbs
startPbsmom();
