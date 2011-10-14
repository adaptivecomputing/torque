#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;
use Torque::Util    qw( 
                               list2array
                               run_and_check_cmd 
                             );
use Torque::Util::Pbsnodes qw( parse_output );

# Test Description
plan('no_plan');
setDesc('pbsnodes -s <SERVER> -c <HOST>');

# Variables
my $node       = $props->get_property('Test.Host');
my $server     = $props->get_property('Test.Host');
my $cmd        = "pbsnodes -s $server";
my $c_cmd      = "pbsnodes -s $server -c $node";
my $o_cmd      = "pbsnodes -s $server -o $node";
my @properties = list2array($props->get_property('node.properties'));
my %pbsnodes;
my %output;

# Run the offline command
run_and_check_cmd($o_cmd);
%pbsnodes = run_and_check_cmd($cmd);
%output   = parse_output($pbsnodes{ 'STDOUT' });
like($output{ $node }{ 'state' }, qr/\boffline\b/, "Checking for the offline state");

# Run the clear command
run_and_check_cmd($c_cmd);
%pbsnodes = run_and_check_cmd($cmd);
%output   = parse_output($pbsnodes{ 'STDOUT' });
unlike($output{ $node }{ 'state' }, qr/\boffline\b/, "Checking that nodes are on longer offline");

# Check for a given property
foreach my $property (@properties)
  {

  $cmd   = "pbsnodes -s $server :$property";
  $o_cmd = "pbsnodes -s $server -o :$property";
  $c_cmd = "pbsnodes -s $server -c :$property";

  # Run the offline command
  run_and_check_cmd($o_cmd);
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_output($pbsnodes{ 'STDOUT' });
  like($output{ $node }{ 'state' }, qr/\boffline\b/, "Checking for the offline state");

  # Run the clear command
  run_and_check_cmd($c_cmd);
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_output($pbsnodes{ 'STDOUT' });
  unlike($output{ $node }{ 'state' }, qr/\boffline\b/, "Checking that nodes are on longer offline");

  } # END foreach my $node (@properties) 
