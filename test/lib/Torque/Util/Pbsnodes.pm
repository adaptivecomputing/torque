package Torque::Util::Pbsnodes;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../..";


use CRI::Test;
use Carp;

use XML::Simple;
use Torque::Util  qw(
                             list2array
                           );
use Torque::Test::Regexp qw( 
                             HOST_STATES
                             NTYPES
                           );

use base 'Exporter';

our @EXPORT_OK = qw(
                   parse_output
                   parse_xml
                   parse_list_output
                   parse_list_n_output
                   );

###############################################################################
# parse_output - parse the output of pbsnodes or pbsnodes -a
###############################################################################
sub parse_output #($)
  {

  # The given output
  my ($output) = @_;

  # Return value hash
  my %rtn_values;

  # Other Variables
  my @hosts_output;

  # Split up each nodes information
  @hosts_output = split(/\n\n/, $output);

  # Parse out the information
  foreach my $host_output (@hosts_output)
    {

    my @lines = split(/\n/, $host_output);
    my $host  = shift @lines;  # The first line of the output should have the 
                               # host name
    $rtn_values{ $host } = {};

    foreach my $line (@lines)
      {

      # Strip off leading spaces
      $line =~ s/^\s+//;
      
      my ($key, $value) = split(/ = /, $line);

      # Split up the status values
      if ($key eq 'status')
        {

        $rtn_values{ $host }{ $key } = {};
        
        my @status_pairs = split(/,/, $value);

        foreach my $status_pair (@status_pairs)
          {

          my ($status_key, $status_value) = split(/=/, $status_pair);
          $rtn_values{ $host }{ $key }{ $status_key } = $status_value;

          } # END foreach my $status_pair (@status_pairs)

        next;

        } # END if ($key eq 'status')

      # Put the properties in a list context
      if ($key eq 'properties')
        {

        my @properties = Torque::Util::list2array($value);
        $rtn_values{ $host }{ $key } = \@properties;
        next;

        } # END if ($key eq 'properties')

      $rtn_values{ $host }{ $key } = $value;

      } # END foreach my $line (@lines)

    } # END foreach my $host_output (@hosts_output)

  return %rtn_values;

  }

###############################################################################
# parse_xml - parse the output of pbsnodes -x and return a hash
###############################################################################
sub parse_xml #($)
  {

  my ($xml) = @_;

  my $rtn_val;

  $rtn_val = XMLin($xml);
  $rtn_val = $rtn_val->{ 'Node' };

  # This is neccessary when only one node is found in the list.  If this is the
  # case then we don't have the expect { host1 => { } } format
  if (exists $rtn_val->{ 'name' })
   {

   my $tmp_hash;
   my $host = $rtn_val->{ 'name' };
   delete $rtn_val->{ 'name' };
   $tmp_hash->{ $host } = $rtn_val;
   $rtn_val = $tmp_hash;

   } # END if (exists $rtn_value->{ 'name' })

  foreach my $host (keys %$rtn_val)
    {

    # Check if status is defined
    if (defined $rtn_val->{ $host }{ 'status' }) 
      {

        my $value        = $rtn_val->{ $host }{ 'status' };
        my @status_pairs = split(/,/, $value);

        $rtn_val->{ $host }{ 'status' } = {};

        foreach my $status_pair (@status_pairs)
          {

          my ($status_key, $status_value) = split(/=/, $status_pair);
          $rtn_val->{ $host }{ 'status' }{ $status_key } = $status_value;

          } # END foreach my $status_pair (@status_pairs)


      } # END if (defined $rtn_val->{ $host }{ 'status' }) 

    # Check if properties is defined
    if (defined $rtn_val->{ $host }{ 'properties' })
        {
    
        my $value                           = $rtn_val->{ $host }{ 'properties' };
        my @properties                      = Torque::Util::list2array($value);
        $rtn_val->{ $host }{ 'properties' } = \@properties;

        } # END if (defined $rtn_val->{ $host }{ 'properties' })

    } # END foreach my $host (keys %$rtn_val)

  return %$rtn_val;

  } # END sub parse_xml #($)

###############################################################################
# parse_list_output - parse the output of pbsnodes -l
###############################################################################
sub parse_list_output #($)
  {

  my ($output) = @_;
  my %rtn_values;

  my @lines = split(/\n/, $output);

  foreach my $line (@lines)
    {

    my ($node, $state) = split(/\s+/, $line);
    $rtn_values{ $node } = $state;

    } # END foreach my $line (@lines)

    return %rtn_values;

  } # END sub parse_list_output #($)

###############################################################################
# parse_list_n_output - parse the output of pbsnodes -l
###############################################################################
sub parse_list_n_output #($)
  {

  my ($output) = @_;
  my %rtn_values;

  my @lines = split(/\n/, $output);

  foreach my $line (@lines)
    {

    my ($node, $state, $note) = split(/\s+/, $line);
    $rtn_values{ $node }{ 'state' } = $state;
    $rtn_values{ $node }{ 'note'  } = $note;

    } # END foreach my $line (@lines)

    return %rtn_values;

  } # END sub parse_list_output #($)


1;

=head1 NAME

Torque::Util::Pbsnodes - Some useful Torque test utilities for the pbsnodes command

=head1 SYNOPSIS

 use Torque::Util::Pbsnodes qw( parse_output 
                                parse_list_output
                                parse_list_n_output
                                parse_xml_output);
 use CRI::Test;

 # Run test_output
 my $output     = $pbs_nodes{ 'STDOUT' };
 my @nodes      = qw(node1 node2)
 my @properties = qw(property1 property2)

 $test_params = {
                'output'     => $output,
                'hosts'      => \@nodes,
                'properties' => \@properties
                };

 test_output($test_params);

$test_params = {
                'xml'        => 1,
                'output'     => $output,
                'hosts'      => \@nodes,
                'properties' => \@properties
                };

 test_output($test_params);

 # Test q output
 my %pbsnodes = runCommand('pbsnodes -q');
 test_q_output($pbsnodes{ 'STDERR' });

 # Test s q output
 my %pbsnodes = runCommand('pbsnodes -s server1 -q');
 test_s_q_output($pbsnodes{ 'STDERR' });

=head1 DESCRIPTION

Some useful methods to use when running the pbsnodes command.

=head1 SUBROUTINES/METHODS

=over 4

=item test_output

Performs a series of tests on the output of pbsnodes.

=item test_q_output

Performs a series of tests on the output of pbsnodes -q.

=item test_q_output

Performs a series of tests on the output of pbsnodes -s server1 -q.

=back

=head1 DEPENDENDCIES

Moab::Test, Torque::Util, Carp, XML::Simple

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
