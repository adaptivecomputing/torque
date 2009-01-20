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
                                      parse_list_n_output 
                                    );

# Test Description
plan('no_plan');
setDesc('pbsnodes -o <HOST> -N');

my $cmd;
my @nodes      = list2array($props->get_property('MoabHost'));
my @properties = list2array($props->get_property('torque.node.args'));
my %pbsnodes;
my %output;
my $note;

# Perform the tests for a given node
foreach my $node (@nodes)
  {

  # Mark the node as offline with a note
  $note     = "Maintenance_" . time();
  $cmd      = "pbsnodes -N \"$note\" -o $node";
  %pbsnodes = run_and_check_cmd($cmd);

  # Check that the note exists
  $cmd      = "pbsnodes -l -n $node";
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_list_n_output($pbsnodes{ 'STDOUT' });
  ok($output{ $node }{ 'note' } eq $note, "Checking for note on '$node'");

  # Reset the node
  $note     = "Maintenance_Complete_" . time();
  $cmd      = "pbsnodes -r -N \"$note\" $node";
  %pbsnodes = run_and_check_cmd($cmd);

  # Check the note
  $cmd      = "pbsnodes $node";
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_output($pbsnodes{ 'STDOUT' });
  ok($output{ $node }{ 'note' } eq $note, "Checking for the reset note on '$node'");

  # Erase the note with -N n
  $cmd      = "pbsnodes -N n $node";
  %pbsnodes = run_and_check_cmd($cmd);

  # Check that it was erased
  $cmd      = "pbsnodes $node";
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_output($pbsnodes{ 'STDOUT' });
  ok(! exists $output{ $node }{ 'note' }, "Checking that the note was erase on '$node' with the command 'pbsnodes -N n'");

  # Erase the note with -N ""
  $cmd      = "pbsnodes -N \"\" $node";
  %pbsnodes = run_and_check_cmd($cmd);

  # Check that it was erased
  $cmd      = "pbsnodes $node";
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_output($pbsnodes{ 'STDOUT' });
  ok(! exists $output{ $node }{ 'note' }, "Checking that the note was erase on '$node' with the command 'pbsnodes -N \"\"'");

  # force a cycle
  $cmd      = "momctl -C";
  %pbsnodes = run_and_check_cmd($cmd);

  } # END foreach my $node (@nodes)

# Perform the tests for a given property
foreach my $property (@properties)
  {

  # Mark the node as offline with a note
  $note     = "Maintenance_" . time();
  $cmd      = "pbsnodes -N \"$note\" -o :$property";
  %pbsnodes = run_and_check_cmd($cmd);

  # Check that the note exists
  $cmd      = "pbsnodes -l -n :$property";
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_list_n_output($pbsnodes{ 'STDOUT' });
  foreach my $node (keys %output)
    {
    
    ok($output{ $node }{ 'note' } eq $note, "Checking for note on '$node'");

    } # END foreach my $node (keys %output)

  # Reset the node
  $note     = "Maintenance_Complete_" . time();
  $cmd      = "pbsnodes -r -N \"$note\" :$property";
  %pbsnodes = run_and_check_cmd($cmd);

  # Check the note
  $cmd      = "pbsnodes :$property";
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_output($pbsnodes{ 'STDOUT' });
  foreach my $node (keys %output)
    {
    
    ok($output{ $node }{ 'note' } eq $note, "Checking for note on '$node'");

    } # END foreach my $node (keys %output)

  # Erase the note with -N n
  $cmd      = "pbsnodes -N n :$property";
  %pbsnodes = run_and_check_cmd($cmd);

  # Check that it was erased
  $cmd      = "pbsnodes :$property";
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_output($pbsnodes{ 'STDOUT' });
  foreach my $node (keys %output)
    {
    
    ok(! exists $output{ $node }{ 'note' }, "Checking that the note was erase on '$node' with the command 'pbsnodes -N n'");

    } # END foreach my $node (keys %output)

  # Erase the note with -N ""
  $cmd      = "pbsnodes -N \"\" :$property";
  %pbsnodes = run_and_check_cmd($cmd);

  # Check that it was erased
  $cmd      = "pbsnodes :$property";
  %pbsnodes = run_and_check_cmd($cmd);
  %output   = parse_output($pbsnodes{ 'STDOUT' });
  foreach my $node (keys %output)
    {

  ok(! exists $output{ $node }{ 'note' }, "Checking that the note was erase on '$node' with the command 'pbsnodes -N \"\"'");

    } # END foreach my $node (keys %output)

  # force a cycle
  $cmd      = "momctl -C";
  %pbsnodes = run_and_check_cmd($cmd);

  } # END foreach my $node (@nodes)
