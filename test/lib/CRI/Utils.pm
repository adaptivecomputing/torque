package CRI::Utils;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../lib/";


use CRI::Test;
use Carp;

use base 'Exporter';

our @EXPORT_OK = qw(
                     list2array
                     run_and_check_cmd
                     resolve_path
                     scp
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

###############################################################################
# resolve_path
###############################################################################
sub resolve_path #($)
  {

  my ($path) = @_;

  # Remove ../ from the path the coresponding directories
  my $reg_ex = '[^\/]+\/\.\.(\/)?';
  while ($path =~ /${reg_ex}/)
    {

    $path =~ s/${reg_ex}//;   

    } # END while ($path =~ /\.\./)

  return $path;

  } # END sub resolve_path #($)

###############################################################################
# scp
###############################################################################
sub scp #($)
  {

  my ($params)  = @_;

  my $src       = $params->{ 'src'   } || die "Please provide the 'src' parameter"; 
  my $dest      = $params->{ 'dest'  } || die "Please provide the 'dest' parameter";
  my $retry     = $params->{ 'retry' } || 3;

  my $success   = 0;

  my $scp_cmd   = "scp $src $dest";
  my %scp;
  my $retry_cnt = 0;

  # Keep trying to scp
  while (! $success && $retry_cnt < $retry)
    {

    %scp = runCommand($scp_cmd);

    $success = 1
      if ($scp{ 'EXIT_CODE' } == 0);

    $retry_cnt++;

    } # END while (! $success && $retry_cnt < $retry)

  return %scp;

  } # END scp #($)

1;

=head1 NAME

CRI::Utils - Some useful CRI test utilities

=head1 SYNOPSIS

 use CRI::Utils qw(
                    list2array
                    run_and_check_cmd
                    resolve_path
                    scp
                  );


 # list2array
 my @array2 = list2array('host1,host2');
 my @array3 = list2array('host1 host2');

 # run_and_check_cmd
 my %result = run_and_check_cmd('sleep 30');

 # resolve_path
 my $path = resolve_path('/path/to/dir/../../lib');

 # scp
 my %scp = scp({
                 'src'   => 'test.txt',
                 'dest'  => 'remotenode:test.txt',
                 'retry' => 5
              })

=head1 DESCRIPTION

This module is collection of methods that are useful when writing tests for CRI products

=head1 SUBROUTINES/METHODS

=over 4

=item list2array

Takes a comma or space delimited string and returns an array.

=item run_and_check_cmd

Runs a command using the Moab::Test::runCommand and checks that the command ran successfully

=item resolve_path($path)

Resolves the path by removing '../' from the path and the coresponding directories.  Returns the resolved path.

=item scp($params)

Takes a hashref of parameters including: src, dest, and retry.  Will attempt to scp a file a number of times (default: 3).  Returns a hashref of the last scp attempt.

'src' is the path to the file that needs copied. 

'dest' the path the file on a remote machine that need copied.

'retry' the numbers of times to attempt the scp.

=back

=head1 DEPENDENDCIES

Moab::Test, Carp

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2009 Cluster Resources Inc.

=cut

__END__
