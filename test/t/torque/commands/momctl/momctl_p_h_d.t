#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


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
setDesc('Momctl -p <PORT> -h <HOST> -d <LOGLEVEL>');

# Other Variables
my %momctl;
my $mom_host      = $props->get_property('MoabHost'     );
my $mom_host_port = $props->get_property('mom.host.port');

# Test momctl -p $mom_host_port -h $mom_host -d 3
%momctl = runCommand("momctl -p $mom_host_port -h $mom_host -d 3");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -p $mom_host_port -h $mom_host -d 3' ran")
  or die "Couldn't run momctl -d";

test_level_3($momctl{ 'STDOUT' });

# Test momctl -p $mom_host_port -h $mom_host -d 2
%momctl = runCommand("momctl -p $mom_host_port -h $mom_host -d 2");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -p $mom_host_port -h $mom_host -d 2' ran")
  or die "Couldn't run momctl -d";

test_level_2($momctl{ 'STDOUT' });

# Test momctl -p $mom_host_port -h $mom_host -d 1
%momctl = runCommand("momctl -p $mom_host_port -h $mom_host -d 1");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -p $mom_host_port -h $mom_host -d 1' ran")
  or die "Couldn't run momctl -d";

test_level_1($momctl{ 'STDOUT' });

# Test momctl -p $mom_host_port -h $mom_host -d 0
%momctl = runCommand("momctl -p $mom_host_port -h $mom_host -d 0");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -p $mom_host_port -h $mom_host -d 0' ran")
  or die "Couldn't run momctl -d";

test_level_0($momctl{ 'STDOUT' });

=head1 TODO

Need to find a way of setting the port number in the setup.pl.  

Will also need to test it at each log level.

=cut

