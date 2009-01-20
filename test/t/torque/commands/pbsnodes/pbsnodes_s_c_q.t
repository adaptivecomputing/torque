#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


# Test Modules
use CRI::Test;
use Torque::Ctrl                  qw( startTorque
                                      stopTorque
                                    );
use Torque::Test::Utils           qw( 
                                      run_and_check_cmd 
                                      list2array
                                    );

use Torque::Test::Pbsnodes::Utils qw( parse_output 
                                      test_s_q_output
                                    );

# Test Description
plan('no_plan');
setDesc('pbsnodes -server <SERVER> -c <HOST> -q');

# Variables
my $node     = $props->get_property('MoabHost');
my $server   = $props->get_property('MoabHost');
my $cmd      = "pbsnodes -s $server";
my $c_cmd    = "pbsnodes -s $server -c $node";
my $o_cmd    = "pbsnodes -s $server -o $node";
my @properties = list2array($props->get_property('torque.node.args'));
my %pbsnodes;
my %output;

# Run the offline command
run_and_check_cmd($o_cmd);
%pbsnodes = run_and_check_cmd($cmd);
%output   = parse_output($pbsnodes{ 'STDOUT' });
ok($output{ $node }{ 'state' } eq 'offline', "Checking for the offline state");

# Stop Torque
stopTorque();

# Run the clear command
%pbsnodes = runCommand($cmd);
test_s_q_output($pbsnodes{ 'STDERR' });

# Start Torque
startTorque();

# Run the clear command
run_and_check_cmd($c_cmd);
%pbsnodes = run_and_check_cmd($cmd);
%output   = parse_output($pbsnodes{ 'STDOUT' });
ok($output{ $node }{ 'state' } eq 'free', "Checking for the free state");

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
  ok($output{ $node }{ 'state' } eq 'offline', "Checking for the offline state");

  # Stop Torque
  stopTorque();

  # Run the clear command
  %pbsnodes = runCommand($cmd);
  test_s_q_output($pbsnodes{ 'STDERR' });

  # Start Torque
  startTorque();

  # Run the clear command
  run_and_check_cmd($c_cmd);
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_output($pbsnodes{ 'STDOUT' });
  ok($output{ $node }{ 'state' } eq 'free', "Checking for the free state");

  } # END foreach my $property (@properties)
