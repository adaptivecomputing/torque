package CRI::Util::Install;
########################################################################
# Subroutines that assist in downloading and installing different 
# workload mananger applications.
########################################################################

use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

use base 'Exporter';

our @EXPORT_OK = qw(
  installProduct
  get_hg
  get_svn
  get_url
  get_dir
  get_tarball
);

#------------------------------------------------------------------------------
# Takes a hashref and determines which underlying 'get' method to use to
#  obtain product files. Returns boolean for pass/fail.
#------------------------------------------------------------------------------
# PARAMETERS:
# - get_string => a string which represents the key to be used to look up the
#    'get' string for the given product (ex 'msm.get.src') (REQUIRED)
# - dest => a path to where the final group of files should be put (REQUIRED)
# - host => string of hostname upon which to run procedures 
#------------------------------------------------------------------------------
sub installProduct
{
  my ($params) = @_;

  my $get  = $params->{get_string} || die "Must specify 'get' String via 'get_string' key";
  my $dest = $params->{dest}       || die "Must specify Destination Path via 'dest' key";

  my $get_funcs = {
    'hg'      => \&get_hg,
    'svn'     => \&get_svn,
    'url'     => \&get_url,
    'dir'     => \&get_dir,
    'tarball' => \&get_tarball
  };
 
  my $method;
  ($method, $params->{src}) = split(/\|/, $get);
  $method = lc $method;
 
  die "Invalid method: $method" unless defined $get_funcs->{ $method };
  die "Did not get a Source string from '$get'!" unless defined $params->{src};
  
  # Run the approriate method
  my $result = &{ $get_funcs->{ $method } }($params);

  return $result;
}

#------------------------------------------------------------------------------
# get_hg({
#          'src'  => 'http://host/hg/product',
#          'dest' => '/tmp/product',
#       });
# get_hg({
#          'src'     => 'http://host/hg/product',
#          'dest'    => '/tmp/product',
#          'options' => '-v',
#          'host'    => 'node1'
#       });
#------------------------------------------------------------------------------
# Takes a hashref of parameters and clones a Mercurial repostiory.  The 
# required parameters are 'src' and 'dest'.  The 'options' and 'host' 
# parameters are optional.
# PARAMETERS:
# - update => boolean to specify whether to update mercurial repo or not.
#    Only applies when desired repo already exists. (Optional)
#    {DEFAULT: 0}
#------------------------------------------------------------------------------
sub get_hg
{
  my ($params) = @_;
  my %result;

  my $src         = $params->{ 'src' }         || die "Parameter 'src' missing.  Please provide a source";
  my $dest        = $params->{ 'dest' }        || die "Parameter 'dest' missing.  Please provide a destination";
  my $options     = $params->{ 'options' }     || '-y';
  my $do_update   = $params->{update}          || 0;
 
  $options .= ' -y' unless $options =~ /-y/;

  my $runcmd_flags = { };
  $runcmd_flags->{host} = $params->{host} || undef;

  # See if destination already exists and is up-to-date
  %result = runCommand("ls $dest", %$runcmd_flags, logging_off => 1);
  if( $result{EXIT_CODE} == 0  )
  {
    # Just see if a Mercurial command works in destination
    %result = runCommand("hg paths -R $dest", %$runcmd_flags, logging_off => 1);
    if( $result{EXIT_CODE} == 0 )
    {
      if( $result{STDOUT} =~ m{^default\s+=\s+$src$}m )
      {
        logMsg "--- Mercurial Repo Already in Place. ---";
        
        my $pass = 1;

        if( $do_update )
        {
          diag "--- Attempting to Update Repo ---";

          my $ec = runCommand("hg pull -u -R $dest", %$runcmd_flags, test_success => 1);
          $pass = $ec == 0;
        }

        return $pass;
      }
      else
      {
        if( $do_update )
        {
          logMsg "Destination $dest is wrong repo. Removing...";
          runCommand("rm -Rf $dest", %$runcmd_flags);
        }
        else
        {
          die "Destination $dest is wrong repo and updating is disabled. Please handle manually";
        }
      }
    }
    else
    {
      die "Destination $dest is NOT empty and is not a Mercurial repo! Please correct and run again\n$result{STDOUT}\n$result{STDERR}";
    }
  }

  diag "--- Getting Source using Mercurial (HG) ---";
  
  my $hg_cmd = "hg $options clone $src $dest";

  runCommand($hg_cmd, %$runcmd_flags, test_success_die => 1);
}

#------------------------------------------------------------------------------
# get_svn({
#          'src'  => 'svn://host/svn/product',
#          'dest' => '/tmp/product',
#        });
# get_svn({
#          'src'         => 'svn://host/svn/product',
#          'dest'        => '/tmp/product',
#          'options'     => '-v',
#          'remote_node' => 'node1'
#        });
#------------------------------------------------------------------------------
#
# Takes a hashref of parameters and checkouts a SVN repostiory.  The required 
# parameters are 'src' and 'dest'.  The 'options' and 'remote_node' parameters 
# are optional.
#
#------------------------------------------------------------------------------
sub get_svn #($)
  {

  my ($params) = @_;

  diag("Getting source using Subversion (SVN)");

  my $src         = $params->{ 'src' }         || die("Parameter 'src' missing.  Please provide a source");
  my $dest        = $params->{ 'dest' }        || die("Parameter 'dest' missing.  Please provide a destination");
  my $options     = $params->{ 'options' }     || '';
  my $remote_node = $params->{ 'remote_node' } || undef;
  my $svn_cmd = "svn co --non-interactive $options $src $dest";
  my %result;

  my $runcmd_flags = { host => $remote_node };

  %result = runCommand($svn_cmd, %$runcmd_flags, test_success_die => 1, msg => "Checking out SVN Repo $src" );

  %result = runCommand("ls $dest", %$runcmd_flags, logging_off => 1);

  die "Failed checking out '$src' from svn to '$dest': $result{STDERR}\n$result{STDOUT}" if $result{EXIT_CODE};

  } # END sub get_svn #($)

#------------------------------------------------------------------------------
# get_url({
#          'src'  => 'http://host/product.tar.gz',
#          'dest' => '/tmp/product',
#        });
# get_url({
#          'src'         => 'http://host/product.tar.gz',
#          'dest'        => '/tmp/product',
#          'remote_node' => 'node1'
#        });
#------------------------------------------------------------------------------
#
# Takes a hashref of parameters and downloads source code from a URL.  The 
# required parameters are 'src' and 'dest'. The 'remote_node' parameter is 
# optional.
#
#------------------------------------------------------------------------------
sub get_url #($)
  {

  my ($params) = @_;

  diag("Getting source using a URL");

  my $src         = $params->{ 'src' }         || die("Parameter 'src' missing.  Please provide a source");
  my $dest        = $params->{ 'dest' }        || die("Parameter 'dest' missing.  Please provide a destination");
  my $remote_node = $params->{ 'remote_node' } || undef;
  my $wget_cmd = "wget --no-check-certificate -c -nv -P /tmp $src";
  my %result;

  if (defined $remote_node)
    {

    %result = runCommandSsh($remote_node, $wget_cmd);
    cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$wget_cmd'")
      or die("STDERR: $result{ 'STDERR' }");

    } # END if (defined $remote_node)
  else
    {

    %result = runCommand($wget_cmd);
    cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$wget_cmd'")
      or die("STDERR: $result{ 'STDERR' }");

    } # END else

  get_tarball($params);

  } # END sub get_url #($)

#------------------------------------------------------------------------------
# get_url({
#          'src'  => '/tmp/product.tar.gz',
#          'dest' => '/tmp/product',
#        });
# get_url({
#          'src'         => '/tmp/product.tar.gz',
#          'dest'        => '/tmp/product',
#          'remote_node' => 'node1'
#        });
#------------------------------------------------------------------------------
#
# Takes a hashref of parameters and retrieves source code from a directory.  
# The required parameters are 'src' and 'dest'.  The 'remote_node' parameter is 
# optional.
#
#------------------------------------------------------------------------------
sub get_dir #($)
  {

  my ($params) = @_;

  diag("Extracting the source from a directory");

  my $src         = $params->{ 'src' }         || die("Parameter 'src' missing.  Please provide a source");
  my $dest        = $params->{ 'dest' }        || die("Parameter 'dest' missing.  Please provide a destination");
  my $remote_node = $params->{ 'remote_node' } || undef;
  my $ln_cmd      = "ln -sv $src $dest";
  my %result;

  unless (get_tarball($params))
    {

    if (defined $remote_node)
      {

      %result = runCommandSsh($remote_node, $ln_cmd);
      cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$ln_cmd'")
        or die("STDERR: $result{ 'STDERR' }");

      my $ls_cmd = "ls $dest";
      my %ls = runCommandSsh($remote_node, $ls_cmd);
      cmp_ok($ls{ 'EXIT_CODE' }, '==', 0, "Checking for directory '$dest'")
        or die("Failed linking '$remote_node:$src' to '$remote_node:$dest'");

      } # END if (defined $remote_node)
    else
      {

      %result = runCommand($ln_cmd);
      cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$ln_cmd'")
        or die("STDERR: $result{ 'STDERR' }");

      ok(-d $dest, "Checking for directory '$dest'")
        or die "No directory $dest exists!!";

      } # END else

    } # END unless( get_tarball({ 'src' => $src, 'dest' => $dest }) )

  } # END sub get_dir #($)

#------------------------------------------------------------------------------
# get_tarball({
#              'src'  => '/tmp/product.tar.gz',
#              'dest' => '/tmp/product',
#            });
# get_tarball({
#              'src'         => '/tmp/product.tar.gz',
#              'dest'        => '/tmp/product',
#              'remote_node' => 'node1'
#            });
#------------------------------------------------------------------------------
#
# Takes a hashref of parameters and retrieves source code from a archive.  The 
# required parameters are 'src' and 'dest'. The 'remote_node' parameter is 
# optional.
#
#------------------------------------------------------------------------------
sub get_tarball #($)
  {

  my ($params) = @_;

  diag("Extracting the source from a tarball");

  my $src         = $params->{ 'src' }         || die("Parameter 'src' missing.  Please provide a source");
  my $dest        = $params->{ 'dest' }        || die("Parameter 'dest' missing.  Please provide a destination");
  my $remote_node = $params->{ 'remote_node' } || undef;
  my %extract_args = (
                      '.tar'     => '-xvf',
                      '.tar.gz'  => '-xvzf',
                      '.tar.bz2' => '-xvjf',
                      '.tar.Z'   => '-xvZf',
                     );

  if ($src =~ /.+\/(\S+?(\.tar(?:\.gz|\.bz2|\.Z)?))$/)
    {

    my $tarball = "/tmp/$1";
    my $tar_ext = $2;
    my $tar_cmd = "tar -C /tmp $extract_args{$tar_ext} $tarball";
    my %result;

    if (defined $remote_node)
      {

      %result = runCommandSsh($remote_node, $tar_cmd);
      cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$tar_cmd'")
        or die("STDERR: $result{ 'STDERR' }");

      } # END if (defined $remote_node)
    else
      {

      %result = runCommand($tar_cmd);
      cmp_ok($result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$tar_cmd'")
        or die("STDERR: $result{ 'STDERR' }");

      } # END else

    my @lines = split("\n", $result{ 'STDOUT' });

    if ($lines[0] =~ /(.+)\/$/)
      {

      my $mv_cmd = "mkdir -p $dest; mv -f /tmp/$1/* $dest";

      if (defined $remote_node)
        {

        my %cmd = runCommandSsh($remote_node, $mv_cmd);

        cmp_ok($cmd{ 'EXIT_CODE' }, '==', 0, "Checking exit code of $mv_cmd")
          or die "Unable to move extracted files: $cmd{ 'STDERR' }";

        } # END if (defined $remote_node)
      else
        {

        my %cmd = runCommand($mv_cmd);

        cmp_ok($cmd{ 'EXIT_CODE' }, '==', 0, "Checking exit code of $mv_cmd")
          or die "Unable to move extracted files: $cmd{ 'STDERR' }";

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

__END__
