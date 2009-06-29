package Torque::Test::Pbsnodes;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::RealBin/../..";

use CRI::Test;
use Carp;

use XML::Simple;
use Torque::Util  qw(
                             list2array
                           );
use Torque::Util::Regexp qw( 
                             HOST_STATES
                             NTYPES
                           );
use Torque::Util::Pbsnodes qw(
                               parse_xml
                               parse_output
                             );

use base 'Exporter';

our @EXPORT_OK = qw(
                   test_output
                   test_q_output
                   test_s_q_output
                   );

###############################################################################
# test_output
###############################################################################
sub test_output #($)
  {

  my ($input) = @_;

  my $stdout         = $input->{ 'output'     } or die "Argument 'output' required";
  my $hosts          = $input->{ 'hosts'      } or Torque::Util::list2array($props->get_property('Test.Host'));    
  my $properties     = $input->{ 'properties' } or Torque::Util::list2array($props->get_property('torque.node.properties'));
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

 use Torque::Test::Pbsnodes::Utils qw( 
                                       test_output
                                       test_q_output
                                       test_s_q_output
                                     );
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
