package Torque::Test::Pbsnodes::Utils;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;
use Carp;

use XML::Simple;
use Torque::Test::Utils  qw(
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
                   test_output
                   test_q_output
                   test_s_q_output
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

        my @properties = Torque::Test::Utils::list2array($value);
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
        my @properties                      = Torque::Test::Utils::list2array($value);
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


###############################################################################
# test_output
###############################################################################
sub test_output #($)
  {

  my ($input) = @_;

  my $stdout         = $input->{ 'output'     } or die "Argument 'output' required";
  my $hosts          = $input->{ 'hosts'      } or Torque::Test::Utils::list2array($props->get_property('Test.Host'));    
  my $properties     = $input->{ 'properties' } or Torque::Test::Utils::list2array($props->get_property('torque.node.properties'));
  my $xml            = $input->{ 'xml'        } or 0;

  # Parse the output
  my %output;
  if ($xml)
    {
    %output = parse_xml($stdout);
    }
  else
    {
    %output = parse_output($stdout);
    }

  # Test the output
  foreach my $host (@$hosts)
    {

    ok($output{ $host }{ 'state' } =~ /${\HOST_STATES}/, "Checking $host state");
    ok($output{ $host }{ 'np'    } =~ /\d+/,             "Checking $host np"   );
    ok($output{ $host }{ 'ntype' } =~ /${\NTYPES}/,      "Checking $host ntype");

    my $expected_host_properties = $properties;
    my $actual_host_properties   = $output{ $host }{ 'properties' };

    SKIP:
      {

      skip("No properties for '$host'") 
        if ! defined $expected_host_properties;

      ok(defined $actual_host_properties, "Checking that there are properties returned when expected for '$host'");

      my $properties_str = join(",", @$actual_host_properties);
      foreach my $property (@$expected_host_properties)
        {
   
        ok($properties_str =~ /${property}/, "Checking $host properties for '$property'");

        }

      } # END SKIP:
    SKIP:
      {
        skip("No status found for $host", 16)
          unless defined $output{ $host }{ 'status' };

        ok($output{ $host }{ 'status' }{ 'opsys'     } =~ /\w+/,             "Checking $host status for opsys"     );
        ok($output{ $host }{ 'status' }{ 'uname'     } =~ /.+/,              "Checking $host status for uname"     );
        ok($output{ $host }{ 'status' }{ 'sessions'  } =~ /\d+(\s+\d+)*/,    "Checking $host status for sessions"  );
        ok($output{ $host }{ 'status' }{ 'nsessions' } =~ /\d+/,             "Checking $host status for nsessions" );
        ok($output{ $host }{ 'status' }{ 'nusers'    } =~ /\d+/,             "Checking $host status for nusers"    );
        ok($output{ $host }{ 'status' }{ 'idletime'  } =~ /\d+/,             "Checking $host status for idletime"  );
        ok($output{ $host }{ 'status' }{ 'totmem'    } =~ /\d+kb$/,          "Checking $host status for totmem"    );
        ok($output{ $host }{ 'status' }{ 'availmem'  } =~ /\d+kb$/,          "Checking $host status for availmem"  );
        ok($output{ $host }{ 'status' }{ 'physmem'   } =~ /\d+kb$/,          "Checking $host status for physmem"   );
        ok($output{ $host }{ 'status' }{ 'ncpus'     } =~ /\d+/,             "Checking $host status for ncpus"     );
        ok($output{ $host }{ 'status' }{ 'loadave'   } =~ /\d+\.\d{2}/,      "Checking $host status for loadave"   );
        ok($output{ $host }{ 'status' }{ 'netload'   } =~ /\d+/,             "Checking $host status for netload"   );
        ok($output{ $host }{ 'status' }{ 'state'     } =~ /${\HOST_STATES}/, "Checking $host status for state"     );
        ok($output{ $host }{ 'status' }{ 'rectime'   } =~ /\d+/,             "Checking $host status for rectime"   );

        SKIP:
          {

          skip("No jobs found", 1) 
            if $output{ $host }{ 'status' }{ 'jobs' } eq '';

          ok($output{ $host }{ 'status' }{ 'jobs' } =~ /\d+/, "Checking $host status for jobs");

          }; # END SKIP:

        SKIP:
          {

          skip("Varrattr not found", 1)
            if $output{ $host }{ 'status' }{ 'varattr' } eq '';
          
          ok($output{ $host }{ 'status' }{ 'varattr' } =~ /.*/, "Checking $host status for varattr");

          }; # END SKIP

      }; # END SKIP:

    } # END foreach my $host (@hosts)

  } # END sub test_output #($)

###############################################################################
# test_q_output
###############################################################################
sub test_q_output
 {

 my ($q_out) = @_;

 my $host    = $props->get_property('Test.Host');

 my $line_1  = "Cannot connect to default server host '$host' - check pbs_server daemon.";
 my $line_2  = "pbsnodes: cannot connect to server $host, error=111 (Connection refused)";

 ok($q_out =~ /${line_1}/, "Checking that the line \"$line_1\" does exist");
 ok($q_out !~ /${line_2}/, "Checking that the line \"$line_2\" does not exist");

 }

###############################################################################
# test_s_q_output
###############################################################################
sub test_s_q_output
 {

 my ($q_out) = @_;

 my $host    = $props->get_property('Test.Host');

 my $line_1  = "Cannot connect to specified server host '$host'";
 my $line_2  = "pbsnodes: cannot connect to server $host, error=111 (Connection refused)";

 ok($q_out =~ /${line_1}/, "Checking that the line \"$line_1\" does exist");
 ok($q_out !~ /${line_2}/, "Checking that the line \"$line_2\" does not exist");

 }

1;

=head1 NAME

Torque::Test::Pbsnodes::Utils - Some useful Torque test utilities for the pbsnodes command

=head1 SYNOPSIS

 use Torque::Test::Pbsnodes::Utils qw( parse_output 
                                       parse_list_output
                                       parse_list_n_output
                                       parse_xml_output
                                       test_output
                                       test_q_output
                                       test_s_q_output);
 use CRI::Test;

 # Parse output
 my %pbsnodes = runCommand('pbsnodes');
 my %output = parse_output($pbsnodes{ 'STDOUT' });

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

 # Parse output
 my %pbsnodes = runCommand('pbsnodes');
 my %output   = parse_output($pbsnodes{ 'STDOUT' });

 # Parse list output
 my %pbsnodes = runCommand('pbsnodes -l');
 my %output   = parse_list_output($pbsnodes{ 'STDOUT' });

 # Parse list output
 my %pbsnodes = runCommand('pbsnodes -l -n');
 my %output   = parse_list_n_output($pbsnodes{ 'STDOUT' });

 # Parse xml output
 my %pbsnodes = runCommand('pbsnodes -x');
 my %output   = parse_xml_output($pbsnodes{ 'STDOUT' });

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

=item parse_output

Takes the pbsnodes output and returns a hash of the values it returns.

=item parse_list_output

Takes the pbsnodes -l output and returns a hash of the values it returns.

=item parse_list_n_output

Takes the pbsnodes -l -n output and returns a hash of the values it returns.

=item parse_xml_output

Takes the pbsnodes -x output and returns a hash of the values it returns.

=item test_output

Performs a series of tests on the output of pbsnodes.

=item test_q_output

Performs a series of tests on the output of pbsnodes -q.

=item test_q_output

Performs a series of tests on the output of pbsnodes -s server1 -q.

=back

=head1 DEPENDENDCIES

Moab::Test, Torque::Test::Utils, Carp, XML::Simple

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
