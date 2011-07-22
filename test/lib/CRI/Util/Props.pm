package CRI::Util::Props;

#######################################################################
# 
# File   :  CRI/Util/Props.pm
# History:  14-sep-2009 (jdayley) Added this header.
#
########################################################################
#
# Utility subroutines for working with Data::Properties files.
# 
########################################################################

use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use Data::Properties;
use Carp;

use CRI::Test;

use base 'Exporter';

our @EXPORT_OK = qw(
                    props
                    remote_props
                    oslist
                    vmoslist
                   );

#------------------------------------------------------------------------------
# my $props_obj = props();
# my $props_obj = props({
#                        'files' => [
#                                    '/path/to/props1',
#                                    '/path/to/props2',
#                                   ]
#                      });
#------------------------------------------------------------------------------
#
# Returns the Data::Properties object containing properties from the files
# '/etc/clustertest.conf' and '/tmp/clustertest.props'.  Takes an optional
# hashref with the parameter 'files' which is an array reference of
# Data::Propertie files to load.  If the files parameter is specified it will
# override the default files.
#
#------------------------------------------------------------------------------
sub props #($)
  {

  my ($params) = @_;

  croak("Please provide parameters in a hash reference")
    if (ref $params ne 'HASH');

  my $file_aref = $params->{ 'files' } || ['/etc/clustertest.conf', $props->get_property('Test.Props.Loc')];

  my $tmp_props = Data::Properties->new();

  foreach my $file (@$file_aref)
    {

    open(FH, $file)
      or croak("Can't open Data::Properties file $file: $!");

    $tmp_props->load(\*FH);

    close FH;

    } # END foreach my $file (@$file_aref)

  return $tmp_props;

  } # END sub props #($)

#------------------------------------------------------------------------------
# my $rprops_obj = remote_props({
#                                'host' => 'host1'
#                              });
# my $rprops_obj = remote_props({
#                                'host' => 'host1',
#                                'files' => [
#                                             '/path/to/props1',
#                                             '/path/to/props2',
#                                           ]
#                              });
#------------------------------------------------------------------------------
#
# Returns the Data::Properties object containing properties from the files
# '/etc/clustertest.conf' and '/tmp/clustertest.props' on the specified host.
# Takes an hashref with the required parameter 'host' and an optional parameter
# 'files' which is an array reference of Data::Propertie files to load.  If the
# 'files' parameter is specified it will override the default files.
#
#------------------------------------------------------------------------------
sub remote_props    #($)
  {

  my ($params) = @_;

  #  croak ("Please provide parameters in a hash reference")
  #    if (ref $params ne 'HASH');

  my $host             = undef;
  my $remote_file_aref = ['/etc/clustertest.conf', '/tmp/clustertest.props'];
  my $local_file_aref  = [];

  if (ref $params eq '')
    {

    $host = $params || croak "Please provide a 'host'";
    $remote_file_aref = ['/etc/clustertest.conf', '/tmp/clustertest.props'];

    } # END if (ref $params eq '')
  elsif (ref $params eq 'HASH')
    {

    $host = $params->{ 'host' } || croak "Please provide the 'host' parameter";
    $remote_file_aref = $params->{ 'files' } || ['/etc/clustertest.conf', '/tmp/clustertest.props'];
    $local_file_aref = [];

    } # END elsif (ref $params eq 'HASH' )
  else
    {

    croak("Please provide a host or a hashref containing the host");

    } # END else

  foreach my $file (@$remote_file_aref)
    {

    my $tmp_dir   = '/tmp/.props_sandbox';
    my $file_name = $1 if ($file =~ /([^\/]+)$/);
    my $tmp_file  = "$tmp_dir/$file_name";
    ok(mkdir($tmp_dir), "Making the directory $tmp_dir")
      unless (-e $tmp_dir);

    my $scp_cmd    = "scp -o BatchMode=yes $host:$file $tmp_file";
    my %cmd_result = runCommand($scp_cmd);
    croak("Command $scp_cmd failed: $cmd_result{ 'STDERR' }")
      if ($cmd_result{ 'EXIT_CODE' } != 0);
    push(@$local_file_aref, $tmp_file);

    } # END foreach my $file (@$file_aref)

  return props({ 'files' => $local_file_aref });

  } # END sub remote_props #($)


#------------------------------------------------------------------------------
# my @oslist = oslist()
#------------------------------------------------------------------------------
#
# Returns an array of the os's listed in the the moab.oslist property.
#
#------------------------------------------------------------------------------
sub oslist #()
  {

  my $oslist = $props->get_property('moab.oslist') || '';
  my @list     = split(/,/, $oslist);

  return @list;

  } # END sub oslist #()

#------------------------------------------------------------------------------
# my @vmoslist = vmoslist()
#------------------------------------------------------------------------------
#
# Returns an array of the os's listed in the the moab.vmoslist property.
#
#------------------------------------------------------------------------------
sub vmoslist #()
  {

  my $vmoslist = $props->get_property('moab.vmoslist') || '';
  my @list     = split(/,/, $vmoslist);

  return @list;

  } # END sub vmoslist #()

1;

__END__
