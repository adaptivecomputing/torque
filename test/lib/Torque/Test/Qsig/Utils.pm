package Torque::Test::Qsig::Utils;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

use base 'Exporter';

our @EXPORT_OK = qw(
                   get_recieved_signals
                   );

###############################################################################
# get_recieved_signals
###############################################################################
sub get_recieved_signals #($)
  {

  # Input
  my ($job_id) = @_;

  # Return value
  my %signals = ();

  my $job_output_file = $props->get_property('Torque.Home.Dir') 
                        . "/spool/$job_id.OU";

  open(JOB_OU, "<$job_output_file")
    or (    fail("Unable to open '$job_output_file'")
        and return %signals);

  while (my $line = <JOB_OU>)
    {

     chomp $line;

     next unless $line =~ /^SIGNAL:\s/;

     my (undef, $signal) = split(/:\s/, $line);

     $signals{ $signal } = 0
       unless defined $signals{ $signal };

     # Count the number of times the signal is encountered
     $signals{ $signal }++;

    } # END while (my $line = <JOB_OU>)

  return %signals;

  } # END sub get_recieved_signals #($)

1;

=head1 NAME

Torque::Test::Qsig::Utils - Some useful Torque test utilities for the qsig command

=head1 SYNOPSIS

 use Torque::Test::Qsig::Utils qw( 
                                   get_recieved_signals
                                 );

 use CRI::Test;

 # get_recieved_signals
 my %signals = get_recieved_signals($job_id)

=head1 DESCRIPTION

Some useful methods to use when running and testing the qsig command.

=head1 SUBROUTINES/METHODS

=over 4

=item get_recieved_signals($job_id)

Parses the STDOUT of a job running a program that captures signals (ex: t/torque/test_programs/sig_trap.pl).  The program needs to print to STDOUT the following when a signal is capture "SIGNAL: SIGNAME".  get_recieved_signals will return a hash of the signals recieved with a count of the number of times the signal was recieved (ex {'TERM' => 1}).

=back

=head1 DEPENDENDCIES

Moab::Test

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
