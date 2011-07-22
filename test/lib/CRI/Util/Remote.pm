package CRI::Util::Remote;

########################################################################
# 
# File   :  WM/Util/Remote.pm
# History:  14-sep-2009 (jdayley) Added this header
#
########################################################################
#
# Provides utility subrotines to perform tasks realated to remote nodes.
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
                    remote_file_exists
                   );

#------------------------------------------------------------------------------
# if (remote_file_exists('host1', '/tmp/file.txt') { # Do Something }
#------------------------------------------------------------------------------
#
# Tests whether a file exits on a remote machine.  Returns 1 if the file exists
# and 0 if it does not.
#
#------------------------------------------------------------------------------
sub remote_file_exists # ($$)
  {

  my ($host, $file) = @_;

  croak("Please provide a 'host' and a 'file' parameter")
    if (   !defined $host
        or !defined $file);

  my $rtn    = 0;
  my $ls_cmd = "ls $file";

  my %ls_cmd_result = runCommandSsh($host, $ls_cmd);
  my $exit_code = $ls_cmd_result{ 'EXIT_CODE' };

  if    ($exit_code == 0) { $rtn = 1;                                                }
  elsif ($exit_code == 2) { $rtn = 0;                                                }
  else                    { fail("$host:$ls_cmd failed: $ls_cmd_result{ 'STDERR' }") }

  return $rtn;

  } # END sub remote_file_exists # ($$)

1;

__END__
