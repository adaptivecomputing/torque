package Torque::Util::Qstat;

########################################################################
# 
# File   :  Qstat.pm
# History:  25-aug-2009 (jdayley) Created and added the qstat_fx()
#                       subroutine.
#
########################################################################
#
# Provides subroutines for the Torque qstat command.
# 
########################################################################

use strict;
use warnings;

use CRI::Test;
use Carp;

use CRI::Util qw( parseXML );

use base 'Exporter';

our @EXPORT_OK = qw(
                     qstat_fx
                   );

my $qstat = $props->get_property("Torque.Home.Dir") . "/bin/qstat";

#------------------------------------------------------------------------------
# my $qref = qstat_fx()
# my $qref = qstat_fx({ job_id => '6.host' });
#------------------------------------------------------------------------------
#
# DESCRIPTION:
#   Runs the qstat -fx command and returns a hashref of the values returned.
#
# HASHREF PARAMETERS:
#   o job_id - The <job_id> parameter for qstat -f -x <job_id>. 
#   o user   - The user to run qstat -f -x as.
#
#------------------------------------------------------------------------------
sub qstat_fx #($)
{
  my ($params) = @_;

  my $job_id   = exists $params->{job_id} ? $params->{job_id} : '';
  my $runcmd_flags = $params->{user} || { test_success => 1 };

  $runcmd_flags->{user} = $params->{user} || $runcmd_flags->{user};

  my %qstat_fx = runCommand("$qstat -f -x $job_id", %$runcmd_flags);

  my $xref = {};
  unless($qstat_fx{EXIT_CODE})
  {
    my $xref     = parseXML({
        xml          => $qstat_fx{STDOUT},
        options => {
          KeyAttr => {
            Job => 'Job_Id',
          },
        },
      });

    $xref = $xref->{Job};
  }

  return $xref;
}

1;
