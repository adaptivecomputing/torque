package Torque::Test::Qmgr::Utils;

use strict;
use warnings;

use CRI::Test;

use base 'Exporter';

our @EXPORT_OK = qw(
                   list_server_info
                   );

###############################################################################
# list_server_info
###############################################################################
sub list_server_info #($)
  {

  my ($server) = @_;

  $server = '' 
    unless defined $server;

  # Variables
  my $cmd;
  my %qmgr;
  my $output;
  my @lines;
  my $line;
  my %info   = ();


  # Get the server info
  $cmd    = "qmgr -c 'list server' $server";
  %qmgr   = runCommand($cmd);
  ok($qmgr{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'")
    or return %info;

  # Parse the information
  $output = $qmgr{ 'STDOUT' };
  @lines  = split(/\n/, $output);
  $line   = shift @lines;
 
  # Make sure the output what we expect
  return %info
     if $line !~ /Server\s(${server})?/i;

  foreach $line (@lines)
    {

    # Cleanup leading and trailing spaces
    $line =~ s/^\s+//;
    $line =~ s/\s+$//;

    my ($key,$value) = split(/\s=\s/, $line);
    $info{ $key }    = $value;

    } # END foreach $line (@lines)

  return %info;

  } # END sub list_server_info #($)

1;

=head1 NAME

Torque::Test::Qmgr::Utils - Some useful Torque test utilities for the qmgr command

=head1 SYNOPSIS

 use Torque::Test::Qmgr::Utils qw( 
                                   list_server_info
                                 );
 use CRI::Test;

 # list_server_info
 my $server = 'localhost';
 my %server_info;

 %server_info = list_server_info();
 %server_info = list_server_info($server);

=head1 DESCRIPTION

Some useful methods to use when running the qmgr command.

=head1 SUBROUTINES/METHODS

=over 4

=item list_server_info([$server])

Returns a hash of the list of attributes returned by "qmgr -c 'list server' [SERVER]".  Takes an optional server argument

=back

=head1 DEPENDENDCIES

Moab::Test

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
