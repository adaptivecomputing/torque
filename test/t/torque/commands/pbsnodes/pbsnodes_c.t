#!/usr/bin/perl

use strict;
use warnings;

# Test Modules
use CRI::Test;
use Torque::Test::Utils           qw( 
                                      list2array
                                      run_and_check_cmd 
                                    );
use Torque::Test::Pbsnodes::Utils qw( parse_output );

# Test Description
plan('no_plan');
setDesc('pbsnodes -c <HOST>');

# Variables
my $node       = $props->get_property('MoabHost');
my $cmd        = "pbsnodes";
my $c_cmd      = "pbsnodes -c $node";
my $o_cmd      = "pbsnodes -o $node";
my @properties = list2array($props->get_property('torque.node.args'));
my %pbsnodes;
my %output;

# Run the offline command
run_and_check_cmd($o_cmd);
%pbsnodes = run_and_check_cmd($cmd);
%output   = parse_output($pbsnodes{ 'STDOUT' });
ok($output{ $node }{ 'state' } eq 'offline', "Checking for the offline state");

# Run the clear command
run_and_check_cmd($c_cmd);
%pbsnodes = run_and_check_cmd($cmd);
%output   = parse_output($pbsnodes{ 'STDOUT' });
ok($output{ $node }{ 'state' } eq 'free', "Checking for the free state");

# Check for a given property
foreach my $property (@properties)
  {

  $cmd   = "pbsnodes :$property";
  $o_cmd = "pbsnodes -o :$property";
  $c_cmd = "pbsnodes -c :$property";

  # Run the offline command
  run_and_check_cmd($o_cmd);
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_output($pbsnodes{ 'STDOUT' });
  ok($output{ $node }{ 'state' } eq 'offline', "Checking for the offline state");

  # Run the clear command
  run_and_check_cmd($c_cmd);
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_output($pbsnodes{ 'STDOUT' });
  ok($output{ $node }{ 'state' } eq 'free', "Checking for the free state");

  } # END foreach my $node (@nodes) 


# Check for a given property
