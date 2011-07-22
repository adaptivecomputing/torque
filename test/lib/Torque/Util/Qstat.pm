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
  my $user     = $params->{ 'user'   } || 'root';

  my %qstat_fx = runCommandAs($user, "$qstat -f -x $job_id", ('test_success_die' => 1));

  my $key = 'Job';
  my $xref     = parseXML({
      xml          => $qstat_fx{STDOUT},
      options => {
        KeyAttr => {
          $key => 'Job_Id',
        },
      },
    });

  $xref->{$_} = $xref->{$key}{$_} foreach keys %{$xref->{$key}};
  delete $xref->{$key};

  return $xref;

  } # END sub qstat_fx #($)

1;

__END__
