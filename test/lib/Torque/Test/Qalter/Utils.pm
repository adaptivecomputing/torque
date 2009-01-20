package Torque::Test::Qalter::Utils;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


use CRI::Test;

use XML::Simple;

use base 'Exporter';

our @EXPORT_OK = qw(
                    gen_a_time
                   );

###############################################################################
# gen_a_time - Takes a the time as given by time() and returns a timestamp 
#            suitable  for qalter -a.
###############################################################################
sub gen_a_time #($)
  {
  
  my ($timestamp) = @_;

  my $time_str;

  # Turn the time into something readable
  my @time = localtime($timestamp);
  $time[5] += 1900;  # The year counts from 1900
  $time[4] += 1;     # The month starts with 0

  # Variables for the $time_str.  The format needs to be [[[[CC]YY]MM]DD]hhmm[.SS]
  my $CC = substr($time[5], 0, 2);
  my $YY = substr($time[5], 2, 2);
  my $MM = $time[4]; 
  my $DD = $time[3];
  my $hh = $time[2];
  my $mm = $time[1];
  my $SS = $time[0];

  # Prefix Variables with zeros if neccessary
  $MM = "0$MM" while (length $MM < 2);
  $DD = "0$DD" while (length $DD < 2);
  $hh = "0$hh" while (length $hh < 2);
  $mm = "0$mm" while (length $mm < 2);
  $SS = "0$SS" while (length $SS < 2);

  $time_str = "$CC$YY$MM$DD$hh$mm.$SS";

  return $time_str;

  } # END sub gen_a_time #($)

1;

=head1 NAME

Torque::Test::Qalter::Utils - Some useful Torque test utilities for the qalter command

=head1 SYNOPSIS

 use Torque::Test::Qalter::Utils qw( );
 use CRI::Test;

=head1 DESCRIPTION

Some useful methods to use when running the qalter command.

=head1 SUBROUTINES/METHODS

=over 4

=back

=head1 DEPENDENDCIES

Moab::Test, Torque::Test::Utils, XML::Simple

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
