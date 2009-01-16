#!/usr/bin/perl

use strict;
use warnings;

# Test Module
use CRI::Test;
use Torque::Test::Momctl::Diag qw(
                                 test_level_0
                                 test_level_1
                                 test_level_2
                                 test_level_3
                                 );

# Test Case Description
plan('no_plan');
setDesc('Momctl -p <PORT> -d <LOGLEVEL>');

# Mom Variables
my $mom_host_port    = $props->get_property('mom.host.port');

# Torque Variables
my $torque_spool_dir = $props->get_property('torque.spool.dir');

# Other Variables
my %momctl;

# Invoke momctl -p $mom_host_port -d 3
%momctl = runCommand("momctl -p $mom_host_port -d 3");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -p $mom_host_port -d 3' ran")
  or die "Couldn't run momctl -p $mom_host_port -d";

test_level_3($momctl{ 'STDOUT' });

# Invoke momctl -p $mom_host_port -d 2
%momctl = runCommand("momctl -p $mom_host_port -d 2");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -p $mom_host_port -d 2' ran")
  or die "Couldn't run momctl -p $mom_host_port -d";

test_level_2($momctl{ 'STDOUT' });

# Invoke momctl -p $mom_host_port -d 1
%momctl = runCommand("momctl -p $mom_host_port -d 1");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -p $mom_host_port -d 1' ran")
  or die "Couldn't run momctl -p $mom_host_port -d";

test_level_1($momctl{ 'STDOUT' });

# Invoke momctl -p $mom_host_port -d 0
%momctl = runCommand("momctl -p $mom_host_port -d 0");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -p $mom_host_port -d 0' ran")
  or die "Couldn't run momctl -p $mom_host_port -d";

test_level_0($momctl{ 'STDOUT' });
