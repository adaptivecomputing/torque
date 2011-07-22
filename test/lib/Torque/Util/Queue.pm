package Torque::Util::Queue;

########################################################################
# 
# File   :  Queue.pm
# History:  19-aug-2009 (jdayley) Created and added the create_queues()
#                       subroutine.
#
########################################################################
#
# Provides subroutines for managing PBS queues
# 
########################################################################

use strict;
use warnings;

use CRI::Test;
use Carp;

use Torque::Util::Qmgr qw( qmgr_c );

use base 'Exporter';

our @EXPORT_OK = qw(
                     create_queues
                   );

#------------------------------------------------------------------------------
# create_queues(['simple1', 'simple2', 'simple3']);
# create_queues({
#                'q_aref' => [
#                              {
#                                'qnames' => ['batch1', 'batch2']
#                              }
#                            ]
#              });
# create_queues({
#                'q_aref'        => [
#                                    {
#                                      'qnames'   => ['batch1', 'batch2']
#                                    },
#                                    {
#                                      'qnames'   => ['batch3', 'batch4'],
#                                      'set_href' => {
#                                                     'queue_type'                 => 'Execution',
#                                                     'resources_default.nodes'    => 2,
#                                                     'resources_default.walltime' => '02:00:00',
#                                                     'enabled'                    => 'True',
#                                                     'started'                    => 'False'
#                                                    }
#                                    }
#                                   ],
#                 'user'         => 'user1',
#                 'runcmd_flags' => { 
#                                     'test_fail'                  => 1 
#                                   },
#                 'def_set_href' => {
#                                     'queue_type'                 => 'Execution',
#                                     'resources_default.nodes'    => 1,
#                                     'resources_default.walltime' => '01:00:00',
#                                     'enabled'                    => 'False',
#                                     'started'                    => 'False'
#                                   }
#              });
#------------------------------------------------------------------------------
#
# DESCRIPTION:
# Takes a hashref or a arrayref to create torque queues.  
#
# If an arrayref is given it is assumed that it contains the new queue names. 
# The queues are setup using the default options. 
#
# If a hashref is given then it is assumed that more of the queue details are
# to be given. (See HASHREF PARAMETERS).
#
# Returns 1 upon completion.
#
# HASHREF PARAMETERS:
#   o q_aref      - An arrayref of hashrefs containing the 'qnames' and 
#                   optional 'set_href'.
#         o qnames   - An arrayref of queuenames.
#         o set_href - A hashref of queue attributes that can be set 
#                      using the qmgr command 'qmgr -c 'set queue <qname> 
#                      <attr> = <value>'.
#   o user         - The unix user to run the commands as
#   o runcmd_flags - Any runcmd_flags that can be passed to the 
#                    CRI::Test::runCommandAs() method.
#   o def_set_href - The default 'set_href' hash reference that is used when
#                    none is specified.
#
#------------------------------------------------------------------------------
sub create_queues #($)
  {

  my ($params) = @_;

  diag "Creating new queues";

  # Check for an arrayref,  this will allow for similiar calls
  if (ref $params eq "ARRAY")
    {

    my $tmp_aref = $params;

    $params               = {};
    $params->{ 'q_aref' } = [{ 'qnames' => $tmp_aref }];

    } # END if (ref $params 'eq' "ARRAY")

  my $q_aref       = $params->{ 'q_aref'       } || croak "Please provide the q_aref parameter";

  my $user         = $params->{ 'user'         } || 'root';
  my $runcmd_flags = $params->{ 'runcmd_flags' } || { 'test_success' => 1 };
  my $def_set_href = $params->{ 'def_set_href' } || {
                                                      'queue_type'                 => 'Execution',
                                                      'resources_default.nodes'    => 1,
                                                      'resources_default.walltime' => '01:00:00',
                                                      'enabled'                    => 'True',
                                                      'started'                    => 'True'
                                                    };

  croak "Parameter q_aref needs to be an arrayref"
    unless (ref $q_aref eq 'ARRAY');
 
  my $index = -1;
  foreach my $q_href (@$q_aref)
    {
 
    $index++;

    # Make sure this is a hashref 
    if (ref $q_href ne 'HASH')
      {

      diag "The q_aref element at index:$index is not a hash as expected.  Skipping.";
      next;

      } # END if (ref $q_href ne 'HASH')

   
    # Gather the required parameters
    my $qnames = $q_href->{ 'qnames' };
    my $set_href    = $q_href->{ 'set_href'   } || $def_set_href;

    # Check the hash parameters
    if (! defined $qnames)
      {

      diag "The q_aref element at index:$index does not provide a 'qnames' parameter. Skipping";
      next;

      } # END if (! defined $qnames_qref)
    if (ref $qnames ne 'ARRAY')
      {

      diag "The q_aref element at index:$index does not provide a 'qnames' parameter that is an arrayref. Skipping";
      next;

      } # END if (ref $qnames ne 'ARRAY')
    if (ref $set_href ne 'HASH')
      {

      diag "The q_aref element at index:$index does not provide a 'set_href' parameter that is a hashref. Skipping";
      next;

      } # END if (ref $set_href ne 'HASH')
    
    # Perform the setup for the given queues
    foreach my $qname (@$qnames)
      {

      qmgr_c("create queue $qname", {
                                     'user'         => $user,
                                     'runcmd_flags' => $runcmd_flags
                                    });

      while (my ($qattr, $value) = each (%$set_href))
        {

        qmgr_c("set queue $qname $qattr = $value", {
                                                    'user'         => $user,
                                                    'runcmd_flags' => $runcmd_flags
                                                   });

        } # END  while (my ($qattr, $value) = each (%$set_href))

      } # END foreach my $qname (@$qnames)

    } # END foreach my $q_href (@$q_aref)

  return 1;

  } # END sub create_queues #($)

1;

__END__
