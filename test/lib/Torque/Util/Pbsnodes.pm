package Torque::Util::Pbsnodes;

use strict;
use warnings;

use CRI::Test;
use Carp;

use XML::Simple;
use CRI::Util  qw(
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
                   pbsnodes
                   );

my $pbs_nodes = $props->get_property('Torque.Home.Dir') . "/bin/pbsnodes";

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

        my @properties = CRI::Util::list2array($value);
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
        my @properties                      = CRI::Util::list2array($value);
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

#------------------------------------------------------------------------------
#
#------------------------------------------------------------------------------
#
#
#
#------------------------------------------------------------------------------
sub pbsnodes #($)
  {

  my ($params)    = @_;

  my $user        = $params->{ 'user'   } || 'root';

  my $cmd         = "$pbs_nodes -x";
  my %pbsnodes   = runCommandAs($user, $cmd);

  cmp_ok($pbsnodes{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$cmd' for user $user")
    or return undef;

  return parse_xml($pbsnodes{ 'STDOUT' });

  } # END sub pbsnodes #($)

1;
__END__
