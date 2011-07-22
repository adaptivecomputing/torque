#!/usr/bin/perl

########################################################################
#
# Name: Memory.pm
#
# Description: Memory related subroutines used in CRI tests
#
# Copyright (C) 2010 by Adaptive Computing Enterprises, Inc.
# All Rights Reserved.
#
########################################################################

package CRI::Util::Memory;

use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

use Carp;

use base 'Exporter';

our @EXPORT_OK = qw(
  pid_memory
);

#------------------------------------------------------------------------------
# my $memory_in_kb = pid_memory('1111');
#------------------------------------------------------------------------------
sub pid_memory    #($)
{

  my ($pid) = @_;

  croak "Please provide a pid"
    unless defined $pid;

  # Variables
  my $pmap = `which pmap`;

  # Detaint
  $pmap = $1 if $pmap =~ /(\S+\/pmap)\n/;
  $pid  = $1 if $pid  =~ /^(\d+)$/;

  my $cmd = "$pmap $pid";
  my %res = ();
  my $mem = -1;

  # Get the pid memory
  %res = runCommand($cmd, ("test_success_die" => 1));
  $mem = $1 if $res{"STDOUT"} =~ /\stotal\s+(\d+)\S+/;

  return $mem;

}    # END sub pid_memory #($)

1;

__END__

=head1 NAME

CRI::Util::Memory - Memory module for CRI::Test's

=head1 SYNOPSIS

 use CRI::Util::Memory qw(
                          memory_pid
                         );

 $memory_in_kb = memory_pid('111');                       

=head1 DESCRIPTION

A module for subroutines related to memory functions.  To be used with CRI::Test. The following methods are available:

=over 4

=item $memory_in_kb = memory_pid($pid);

B<memory_pid()> Returns the total memory usage (kb) of the given $pid.

=back

=head1 EXAMPLES

 $moab_pid = moab_pid();

 $mem_in_kb = memory_pid($moab_pid);
 cmp_ok($mem_in_kb, '<=', 1024000, "Verify moab is using less than a gig of memory");

=head1 REQUIRES

TestLibFinder, CRI::Test, Carp, Exporter

=head1 AUTHOR

Adaptive Computing Enterprises, Inc. E<lt>http://www.adaptivecomputing.com/E<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2010 by Adaptive Computing Enterprises, Inc. All Rights Reserved.

=cut
