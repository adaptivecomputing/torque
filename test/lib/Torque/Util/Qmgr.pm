package Torque::Util::Qmgr;

########################################################################
# 
# File   :  Qmgr.pm
# History:  19-aug-2009 (jdayley) Created and added qmgr_c subroutine
#
########################################################################
#
# Provides subroutines for the qmgr command and its flags
# 
########################################################################

use strict;
use warnings;

use CRI::Test;
use Carp;

use base 'Exporter';

our @EXPORT_OK = qw(
                     qmgr_c
                   );

my $qmgr = $props->get_property('Torque.Home.Dir') . "/bin/qmgr";

#------------------------------------------------------------------------
# my %result = qmgr_c('p s');
# my %result = qmgr_c(
#                     'p s',
#                     {
#                      'user'         => 'testuser1',
#                      'runcmd_flags' => { 'test_fail_die' => 1 }
#                     }
#                    )
#------------------------------------------------------------------------
#
# Runs the qmgr -c command with the given command.  Return a hash that is 
# return by the CRI::Test::runCommandAs() method.
#
#------------------------------------------------------------------------
sub qmgr_c #($$)
  {

  my ($qmgr_c_cmd, $params) = @_;

  croak ("Please provide a command for qmgr -c '<cmd>'")
    unless defined $qmgr_c_cmd;

  my $runcmd_flags = $params->{ 'runcmd_flags' } || { 'test_success' => 1 };
  my $user         = $params->{ 'user'         } || 'root';

  my $cmd          = "$qmgr -c '$qmgr_c_cmd'"; 

  my %result = runCommandAs($user, $cmd, %$runcmd_flags);
  return %result;

  } # END qmgr_c #($$)

1;

__END__
