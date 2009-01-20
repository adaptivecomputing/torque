package CRI::Install::Utils;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;
use Carp;

use base 'Exporter';

our @EXPORT_OK = qw(
                     get_hg
                     get_svn
                     get_url
                     get_dir
                     get_tarball
                   );

###############################################################################
# get_hg
###############################################################################
sub get_hg #($)
  {

  my ($params) = @_;

  diag("Getting source using Mercurial (HG)");

  my $src         = $params->{ 'src'         } || confess("Parameter 'src' missing.  Please provide a source");
  my $dest        = $params->{ 'dest'        } || confess("Parameter 'dest' missing.  Please provide a destination");
  my $options     = $params->{ 'options'     } || '';
  my $remote_node = $params->{ 'remote_node' } || undef;
  my $hg_cmd      = "hg $options clone $src $dest";
  my %result;

  if (defined $remote_node)
    {

    %result = runCommandSsh($remote_node, $hg_cmd);
    cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$hg_cmd'")
      or confess("STDERR: $result{ 'STDERR' }");

    my $ls_cmd = "ls $dest";
    my %ls     = runCommandSsh($remote_node, $ls_cmd);
    cmp_ok($ls{ 'EXIT_CODE' }, '==', 0, "Checking for cloned directory '$dest'")
      or confess("Failed cloning '$src' to '$remote_node:$dest'");

    } # END if (defined $remote_node)
  else
    {

    %result = runCommand($hg_cmd);
    cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$hg_cmd'")
      or confess("STDERR: $result{ 'STDERR' }");

    ok(-d $dest, "Checking for cloned directory '$dest'")
      or confess("Failed cloning '$src' to '$dest': $!");

    } # END else

  } # sub get_hg #($)

###############################################################################
# get_svn
###############################################################################
sub get_svn #($)
  {

  my ($params) = @_;

  diag("Getting source using Subversion (SVN)");

  my $src         = $params->{ 'src'         } || confess("Parameter 'src' missing.  Please provide a source");
  my $dest        = $params->{ 'dest'        } || confess("Parameter 'dest' missing.  Please provide a destination");
  my $options     = $params->{ 'options'     } || '';
  my $remote_node = $params->{ 'remote_node' } || undef;
  my $svn_cmd     = "svn co $options $src $dest";
  my %result;

  if (defined $remote_node)
    {

    %result = runCommandSsh($remote_node, $svn_cmd);
    cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$svn_cmd'")
      or confess("STDERR: $result{ 'STDERR' }");

    my $ls_cmd = "ls $dest";
    my %ls     = runCommandSsh($remote_node, $ls_cmd);
    cmp_ok($ls{ 'EXIT_CODE' }, '==', 0, "Checking for directory '$dest'")
      or confess("Failed to checkout '$src' to '$remote_node:$dest'");

    } # END if (defined $remote_node)
  else
    {

    %result = runCommand($svn_cmd);
    cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$svn_cmd'")
      or confess("STDERR: $result{ 'STDERR' }");


    ok(-d $dest, "Checking for the directory: '$dest'")
      or confess("Failed checking out '$src' from svn to '$dest': $!");

    } # END else

  } # END sub get_svn #($)

###############################################################################
# get_url
###############################################################################
sub get_url #($)
  {

  my ($params) = @_;

  diag("Getting source using a URL");

  my $src         = $params->{ 'src'         } || confess("Parameter 'src' missing.  Please provide a source");
  my $dest        = $params->{ 'dest'        } || confess("Parameter 'dest' missing.  Please provide a destination");
  my $remote_node = $params->{ 'remote_node' } || undef;
  my $wget_cmd    = "wget -c -nv -P /tmp $src";
  my %result;

  if (defined $remote_node)
    {

    %result = runCommandSsh($remote_node, $wget_cmd);
    cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$wget_cmd'")
      or confess("STDERR: $result{ 'STDERR' }");

    } # END if (defined $remote_node)
  else
    {

    %result = runCommand($wget_cmd);
    cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$wget_cmd'")
      or confess("STDERR: $result{ 'STDERR' }");

    } # END else

  get_tarball($params); 

  } # END sub get_url #($)

###############################################################################
# get_dir
###############################################################################
sub get_dir #($)
  {

  my ($params) = @_;

  diag("Extracting the source from a directory");

  my $src         = $params->{ 'src'         } || confess("Parameter 'src' missing.  Please provide a source");
  my $dest        = $params->{ 'dest'        } || confess("Parameter 'dest' missing.  Please provide a destination");
  my $remote_node = $params->{ 'remote_node' } || undef;
  my $ln_cmd      = "ln -sv $src $dest";
  my %result;

  unless( get_tarball($params) )
    {

    if (defined $remote_node)
      {

      %result = runCommandSsh($remote_node, $ln_cmd);
      cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$ln_cmd'")
        or confess("STDERR: $result{ 'STDERR' }");

      my $ls_cmd = "ls $dest";
      my %ls     = runCommandSsh($remote_node, $ls_cmd);
      cmp_ok($ls{ 'EXIT_CODE' }, '==', 0, "Checking for directory '$dest'")
        or confess("Failed linking '$remote_node:$src' to '$remote_node:$dest'");

      } # END if (defined $remote_node)
    else
      {

      %result = runCommand($ln_cmd);
      cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$ln_cmd'")
        or confess("STDERR: $result{ 'STDERR' }");

      ok(-d $dest,"Checking for directory '$dest'")
        or confess "No directory $dest exists!!";

      } # END else

    } # END unless( get_tarball({ 'src' => $src, 'dest' => $dest }) )

  } # END sub get_dir #($)

###############################################################################
# get_tarball
###############################################################################
sub get_tarball #($)
  {

  my ($params) = @_;

  diag("Extracting the source from a tarball");

  my $src          = $params->{ 'src'         } || confess("Parameter 'src' missing.  Please provide a source");
  my $dest         = $params->{ 'dest'        } || confess("Parameter 'dest' missing.  Please provide a destination");
  my $remote_node  = $params->{ 'remote_node' } || undef;
  my %extract_args = (
                       '.tar'     => '-xvf',
                       '.tar.gz'  => '-xvzf',
                       '.tar.bz2' => '-xvjf',
                       '.tar.Z'   => '-xvZf',
                     );

  if( $src =~ /.+\/(\S+?(\.tar(?:\.gz|\.bz2|\.Z)?))$/ )
    {

    my $tarball = "/tmp/$1";
    my $tar_ext = $2;
    my $tar_cmd = "tar -C /tmp $extract_args{$tar_ext} $tarball";
    my %result;

    if (defined $remote_node)
      {

      %result = runCommandSsh($remote_node, $tar_cmd);
      cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$tar_cmd'")
        or confess("STDERR: $result{ 'STDERR' }");

      } # END if (defined $remote_node)
    else
      {

      %result = runCommand($tar_cmd);
      cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$tar_cmd'")
        or confess("STDERR: $result{ 'STDERR' }");

      } # END else

    my @lines = split("\n", $result{ 'STDOUT' });

    if( $lines[0] =~ /(.+)\/$/)
      {

      my $mv_cmd = "mkdir -p $dest; mv -f /tmp/$1/* $dest";

      if (defined $remote_node)
        {

        my %cmd = runCommandSsh($remote_node, $mv_cmd);

        cmp_ok($cmd{ 'EXIT_CODE' }, '==', 0, "Checking exit code of $mv_cmd")     
  	      or confess "Unable to move extracted files: $cmd{ 'STDERR' }";

        } # END if (defined $remote_node)
      else
        {

        my %cmd = runCommand($mv_cmd);

        cmp_ok($cmd{ 'EXIT_CODE' }, '==', 0, "Checking exit code of $mv_cmd")     
  	      or confess "Unable to move extracted files: $cmd{ 'STDERR' }";

        } # END else

      } # END if( $lines[0] =~ /(.+\/)$/)

    return 1;

    } # END if( $src =~ /\/(\S+(\.tar(?:\.gz|\.bz2|\.Z)?))$/ )
  else
    {

    return 0;

    } # END else

  } # END sub get_tarball #($)

1;

=head1 NAME

CRI::Install::Utils - Utilities to help with the installing of CRI products

=head1 SYNOPSIS

 use CRI::Install::Utils qw(
                            get_hg
                            get_svn
                            get_url
                            get_dir
                            get_tarball
                           );

 # get_hg
 get_hg({ 'src' => $src, 'dest' => $dest, 'options' => '-al' });

 # get_svn
 get_svn({ 'src' => $src, 'dest' => $dest, 'options' => '-al' });

 # get_url
 get_url({ 'src' => $src, 'dest' => $dest });

 # get_dir
 get_dir({ 'src' => $src, 'dest' => $dest });

 # get_tarball
 get_tarball({ 'src' => $src, 'dest' => $dest });

=head1 DESCRIPTION

A set of utility methods to help with the installation of Cluster Resources Inc. products.  

=head1 SUBROUTINES/METHODS

=over 4

=item get_hg($hashref)

Takes a hashref of parameters and clones a Mercurial repostiory.  The required parameters are 'src' and 'dest'.  The 'options' and 'remote_node' parameters are optional.

=item get_svn($hashref)

Takes a hashref of parameters and checkouts a SVN repostiory.  The required parameters are 'src' and 'dest'.  The 'options' and 'remote_node' parameters are optional.

=item get_url($hashref)

Takes a hashref of parameters and downloads source code from a URL.  The required parameters are 'src' and 'dest'. The 'remote_node' parameter is optional.

=item get_dir($hashref)

Takes a hashref of parameters and retrieves source code from a directory.  The required parameters are 'src' and 'dest'.  The 'remote_node' parameter is optional.

=item get_tarball($hashref)

Takes a hashref of parameters and retrieves source code from a archive.  The required parameters are 'src' and 'dest'. The 'remote_node' parameter is optional.

=back

=head1 DEPENDENDCIES

Moab::Test, Carp, Exporter

=head1 AUTHOR(S)

 Isaac Tobler 
 Jeff Dayley  <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2009 Cluster Resources Inc.

=cut

__END__
