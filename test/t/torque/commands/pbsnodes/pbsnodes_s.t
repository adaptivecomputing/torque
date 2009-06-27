#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;
use Torque::Util    qw( 
                               list2array
                               run_and_check_cmd 
                             );
use Torque::Util::Pbsnodes qw( 
                               parse_output
                             );
use Torque::Test::Pbsnodes qw( 
                               test_output
                             );

# Test Description
plan('no_plan');
setDesc("pbsnodes -s <SERVER>");
# Variables
my $cmd;
my $nodes_str      = $props->get_property('Test.Host');
my @nodes          = list2array($nodes_str);
my $properties_str = $props->get_property('torque.node.args');
my @properties     = list2array($properties_str);
my $server         = $props->get_property('Test.Host');
my $output;
my %pbsnodes;
my $test_params;

# Test the output of pbsnodes -s $server
$cmd         = "pbsnodes -s $server";
%pbsnodes    = run_and_check_cmd($cmd);
$output      = $pbsnodes{ 'STDOUT' };
$test_params = {
                'output'     => $output,
                'hosts'      => \@nodes,
                'properties' => \@properties
               };
test_output( $test_params );

# Test the output of pbsnodes -s $server <node>
foreach my $node (@nodes)
  {
 
  my @single_node = qq($node);

  $cmd         = "pbsnodes -s $server $node";
  %pbsnodes    = run_and_check_cmd($cmd);
  $output      = $pbsnodes{ 'STDOUT' };
  $test_params = {
                  'output'     => $output,
                  'hosts'      => \@single_node,
                  'properties' => \@properties
                 };
  test_output( $test_params );

  } # END foreach my $node (@nodes)

# Test the output of pbsnodes -s $server <:property>
foreach my $property (@properties)
  {

  my @single_property = qq($property);

  $cmd         = "pbsnodes -s $server :$property";
  %pbsnodes    = run_and_check_cmd($cmd);
  $output      = $pbsnodes{ 'STDOUT' };
  $test_params = {
                  'output'     => $output,
                  'hosts'      => \@nodes,
                  'properties' => \@single_property
                 };
  test_output( $test_params );

  } # END foreach my $node (@nodes)
