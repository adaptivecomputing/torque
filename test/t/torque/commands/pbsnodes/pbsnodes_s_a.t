#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


# Test Modules
use CRI::Test;
use Torque::Test::Utils           qw( 
                                      list2array
                                      run_and_check_cmd 
                                    );
use Torque::Test::Pbsnodes::Utils qw( 
                                      parse_output
                                      test_output
                                    );

# Test Description
plan('no_plan');
setDesc("pbsnodes -s <SERVER> -a");
# Variables
my $cmd;
my $nodes_str      = $props->get_property('MoabHost');
my @nodes          = list2array($nodes_str);
my $properties_str = $props->get_property('torque.node.args');
my @properties     = list2array($properties_str);
my $server         = $props->get_property('MoabHost');
my $output;
my %pbsnodes;
my $test_params;

# Test the output of pbsnodes -s $server -a
$cmd         = "pbsnodes -s $server -a";
%pbsnodes    = run_and_check_cmd($cmd);
$output      = $pbsnodes{ 'STDOUT' };
$test_params = {
                'output'     => $output,
                'hosts'      => \@nodes,
                'properties' => \@properties
               };
test_output( $test_params );

# Test the output of pbsnodes -s $server -a <node>
foreach my $node (@nodes)
  {
 
  my @single_node = qq($node);

  $cmd         = "pbsnodes -s $server -a $node";
  %pbsnodes    = run_and_check_cmd($cmd);
  $output      = $pbsnodes{ 'STDOUT' };
  $test_params = {
                  'output'     => $output,
                  'hosts'      => \@single_node,
                  'properties' => \@properties
                 };
  test_output( $test_params );

  } # END foreach my $node (@nodes)

# Test the output of pbsnodes -s $server -a <:property>
foreach my $property (@properties)
  {

  my @single_property = qq($property);

  $cmd         = "pbsnodes -s $server -a :$property";
  %pbsnodes    = run_and_check_cmd($cmd);
  $output      = $pbsnodes{ 'STDOUT' };
  $test_params = {
                  'output'     => $output,
                  'hosts'      => \@nodes,
                  'properties' => \@single_property
                 };
  test_output( $test_params );

  } # END foreach my $node (@nodes)
