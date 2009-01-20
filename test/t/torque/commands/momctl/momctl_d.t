#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Module
use CRI::Test;
use Torque::Test::Momctl::Diag qw(
                                  test_level_3
                                  test_level_2
                                  test_level_1
                                  test_level_0
                                 );

# Test Case Description
plan('no_plan');
setDesc('Momctl -d <LOGLEVEL>');

# Other Variables
my %momctl;

# Test momctl -d 3
%momctl = runCommand("momctl -d 3");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -d 3' ran")
  or die "Couldn't run momctl -d";

test_level_3($momctl{ 'STDOUT' });

# Invoke momctl -d 2
%momctl = runCommand("momctl -d 2");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -d 2' ran")
  or die "Couldn't run momctl -d";

test_level_2($momctl{ 'STDOUT' });

# Invoke momctl -d 1
%momctl = runCommand("momctl -d 1");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -d 1' ran")
  or die "Couldn't run momctl -d";

test_level_1($momctl{ 'STDOUT' });

# Invoke momctl -d 0
%momctl = runCommand("momctl -d 0");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -d 0' ran")
  or die "Couldn't run momctl -d";

test_level_0($momctl{ 'STDOUT' });
