package CRI::Utils;

use strict;
use warnings;

use CRI::Test;
use Carp;

use base 'Exporter';

our @EXPORT_OK = qw(
                     list2array
                     run_and_check_cmd
                   );

###############################################################################
# list2array
###############################################################################
sub list2array #($)
  {

  my ($list) = @_;

  my @rtn_values = split(/,|\s+/, $list);

  return @rtn_values;

  } # END sub hostlist2array

###############################################################################
# run_and_check_cmd
###############################################################################
sub run_and_check_cmd #($)
  {
  
  my ($cmd) = @_;

  my %result;

  %result = runCommand($cmd);
  cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking that '$cmd' ran")
    or diag("'$cmd' failed: $result{ 'STDERR' }");

  return %result;

  } # END sub run_and_check_cmd #($)

1;

=head1 NAME

CRI::Utils - Some useful CRI test utilities

=head1 SYNOPSIS

 use CRI::Utils qw(
                    list2array
                    run_and_check_cmd
                  );


 # list2array
 my @array2 = list2array('host1,host2');
 my @array3 = list2array('host1 host2');

 # run_and_check_cmd
 my %result = run_and_check_cmd('sleep 30');

=head1 DESCRIPTION

This module is collection of methods that are useful when writing tests for CRI products

=head1 SUBROUTINES/METHODS

=over 4

=item list2array

Takes a comma or space delimited string and returns an array.

=item run_and_check_cmd

Runs a command using the Moab::Test::runCommand and checks that the command ran successfully

=back

=head1 DEPENDENDCIES

Moab::Test, Carp

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2009 Cluster Resources Inc.

=cut

__END__
