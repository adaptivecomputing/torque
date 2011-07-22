package Torque::Job::Ctrl;

########################################################################
# 
# File   :  Torque/Job/Ctrl.pm
# History:  26-aug-2009 (jdayley) Added this header, updated comments,
#                       and added qsub subroutine.
#
########################################################################
#
# Contains subroutine related to the submiting and destroying of torque
# jobs.
# 
########################################################################

use strict;
use warnings;

use CRI::Test;
use Carp;

use Torque::Test::Qstat::Utils qw(
                                    parse_qstat_fx
                                 );
use Torque::Job::Utils         qw(
                                    cleanupJobId
                                    detaintJobId
                                 );

use base 'Exporter';

our @EXPORT = qw(
                qsub
                submitJob
                submitSleepJob
                submitCheckpointJob
                runJobs 
                delJobs
                );

#------------------------------------------------------------------------------
# $job_id = qsub(); 
# $job_id = qsub({
#                 'qsub_loc'     => '/usr/test/torque/tools/qsub.special.pl',
#                 'flags'        => '-A account1',
#                 'cmd'          => 'sleep 600',
#                 'user'         => 'testuser1',
#                 'runcmd_flags' => { 'test_fail_die' => 1 }
#               }); 
# $job_id = qsub({
#                 'qsub_loc'     => '/usr/test/torque/tools/qsub.special.pl',
#                 'flags'        => '-A account1',
#                 'user'         => 'testuser1',
#                 'runcmd_flags' => { 'test_fail_die' => 1 },
#                 'script'       => '/tmp/test.pl',
#                 'full_jobid'   => 1
#               }); 
#------------------------------------------------------------------------------
#
# Submits a job using the qsub command and returns the job id.
#
# NOTE: If the 'script' parameter is given it will override the 'cmd' parameter
#       if it is given as well.
#
#------------------------------------------------------------------------------
sub qsub #($) 
  {
  
  my ($params) = @_; 

  # Variables
  my $qsub_loc     = $params->{ 'qsub_loc'     } || $props->get_property("Torque.Home.Dir") . "/bin/qsub";
  my $flags        = $params->{ 'flags'        } || '';
  my $cmd          = $params->{ 'cmd'          } || 'sleep 300' unless exists $params->{script};
  my $user         = $params->{ 'user'         } || $props->get_property("User.1");
  my $runcmd_flags = $params->{ 'runcmd_flags' } || { 'test_success_die' => 1 };
  my $full_jobid   = $params->{ 'full_jobid'   } || 0;
  my $script 	   = $params->{ 'script'       } if exists $params->{script};

  # Create the command
  my $qsub     = $qsub_loc;
  $qsub       .= " $flags" 
    if defined $flags;
  $qsub        = "echo '$cmd' | $qsub"
    if defined $cmd;
  $qsub        = "$qsub $script"
    if defined $script;

  my $job_regex = undef;
  if ($full_jobid)
    {

    $job_regex = qr/(\S+)/;

    } # END if ($full_jobid)
  else
    {

    $job_regex = qr/(\d+)/;

    } # END else

  # Submit the job
  my %qsub_result = runCommandAs($user, $qsub, %$runcmd_flags );

  # Get the job_id, using the regex detaints the value as well
  my $job_id = $qsub_result{ 'STDOUT' };
  if ($job_id =~ $job_regex)  
    { 
  
    $job_id = $1; 
  
    } # END if ($job_id =~ /(\S+)/)  
  else
    {

    $job_id = undef;

    } # END else

  return $job_id;

  } # END qsub #($) 

#------------------------------------------------------------------------------
# my $job_id = submitSleepJob({
#                              'user'       => 'user1',
#                              'torque_bin' => '/usr/test/torque/bin'
#                            });
# my $job_id = submitSleepJob({
#                              'user'       => 'user1',
#                              'torque_bin' => '/usr/test/torque/bin'
#                              'sleep_time' => 200,
#                              'add_args'   => '-l walltime=30:00'
#                            });
#
#------------------------------------------------------------------------------
#
# NOTE: Deprecated.  Use qsub().
#
# Takes a hashref of parameters and submits a job.  It return the job_id.  The 
# following hash illustrates the possible values:
#
# my $params = {
#                'user'       => 'user1',
#                'torque_bin' => '/var/spool/torque/bin',
#                'sleep_time' => 200,
#                'add_args'   => '-l walltime=30:00'
#              };
#
#------------------------------------------------------------------------------
sub submitSleepJob #($) 
  {
  
  my ($params) = @_; 

  # Variables
  my $user       = $params->{ 'user'       } || carp '"user" not given';
  my $torque_bin = $params->{ 'torque_bin' } || carp '"torque_bin" not given';
  my $sleep_time = $params->{ 'sleep_time' } || '300';
  my $add_args   = $params->{ 'add_args'   } || '';

  # Make sure torque_bin ends in a / 
  $torque_bin .= '/';

  # Return value
  my $job_id;  
  
  # Submit a job
  my $qsub_cmd = "echo sleep $sleep_time | ${torque_bin}qsub $add_args";
  diag("Submitting a sleep job as '$user' with the command: '$qsub_cmd'");
  my %qsub     = runCommandAs($user, $qsub_cmd);

  if ($qsub{ 'EXIT_CODE' } == 0)
    {

    $job_id = $qsub{ 'STDOUT' };
    $job_id = cleanupJobId($job_id);

    pass("Successfully submitted sleep job '$job_id'");

    } # END if ($qsub{ 'EXIT_CODE' } == 0)
  else
    {

    $job_id = -1;
    fail('Unable to submit a sleep job: ' . $qsub{ 'STDERR' });

    } # END else

  return $job_id;

  } # END submitSleepJob #($) 

#------------------------------------------------------------------------------
# my $job_id = submitCheckpointJob({
#                                   'user'       => 'user1',
#                                   'torque_bin' => '/usr/test/torque/bin',
#                                   'app'        => '/tmp/checkpoint.pl',
#                                 });
# my $job_id = submitCheckpointJob({
#                                   'user'       => 'user1',
#                                   'torque_bin' => '/usr/test/torque/bin',
#                                   'app'        => '/tmp/checkpoint.pl',
#                                   'add_args'   => '-l walltime=30:00',
#                                   'c_value'    => 'shutdown'
#                                 });
#
#------------------------------------------------------------------------------
#
# NOTE: Deprecated.  Use qsub().
#
# Takes a hashref of parameters and submits a job with checkpointing enabled.  
# It returns the job_id.  The following hash illustrates the possible values:
#
# my $params = {
#               'user'       => 'user1',
#               'torque_bin' => '/var/spool/torque/bin',
#               'app'        => '/tmp/app.pl',
#               'add_args'   => '-l walltime=30:00',
#               'c_value'    => 'shutdown'
#             };
# 
#------------------------------------------------------------------------------
sub submitCheckpointJob #($) 
  {
  
  my ($params) = @_; 

  # Parameters
  my $user       = $params->{ 'user'       } || carp '"user" not given';
  my $torque_bin = $params->{ 'torque_bin' } || carp '"torque_bin" not given';
  my $app        = $params->{ 'app'        } || carp '"app" not give';
  my $add_args   = $params->{ 'add_args'   } || '';
  my $c_value    = $params->{ 'c_value'    } || 'enabled';
 
  # Variables
  my $ld_library_path = $props->get_property( 'LD_LIBRARY_PATH' );

  # Make sure torque_bin ends in a / 
  $torque_bin .= '/';

  # Return value
  my $job_id;  
  
  # Submit a job
  my $qsub_cmd = "${torque_bin}qsub -c $c_value -v LD_LIBRARY_PATH=$ld_library_path $add_args $app";
  my %qsub     = runCommandAs($user, $qsub_cmd);

  if ($qsub{ 'EXIT_CODE' } == 0)
    {

    $job_id = $qsub{ 'STDOUT' };
    $job_id = cleanupJobId($job_id);

    pass("Successfully submitted a checkpoint job '$job_id'");

    } # END if ($qsub{ 'EXIT_CODE' } == 0)
  else
    {

    $job_id = -1;
    fail('Unable to submit a checkpoint job: ' . $qsub{ 'STDERR' });

    } # END else

  return $job_id;

  } # END submitSleepJob #($) 

#------------------------------------------------------------------------------
# my $job_id = submitJob({
#                         'user'       => 'user1',
#                         'torque_bin' => '/usr/test/torque/bin'
#                         'app'        => '/tmp/app.pl'
#                       });
# my $job_id = submitJob({
#                         'user'       => 'user1',
#                         'torque_bin' => '/usr/test/torque/bin'
#                         'app'        => '/tmp/app.pl'
#                         'add_args'   => '-l walltime=30:00'
#                       });
#
#------------------------------------------------------------------------------
#
# NOTE: Deprecated.  Use qsub().
#
# Takes a hashref of parameters and submits a job.  It return the job_id.  The 
# following hash illustrates the possible values:
#
# my $params = {
#                'user'       => 'user1',
#                'torque_bin' => '/var/spool/torque/bin',
#                'app'        => '/tmp/app.pl'
#                'add_args'   => '-l walltime=30:00'
#              };
#
#------------------------------------------------------------------------------
sub submitJob #($) 
  {
  
  my ($params) = @_; 

  # Parameters
  my $user       = $params->{ 'user'       } || carp '"user" not given';
  my $torque_bin = $params->{ 'torque_bin' } || carp '"torque_bin" not given';
  my $app        = $params->{ 'app'        } || carp '"app" not give';
  my $add_args   = $params->{ 'add_args'   } || '';

  # Make sure torque_bin ends in a / 
  $torque_bin .= '/';

  # Return value
  my $job_id;  
  
  # Submit a job
  my $qsub_cmd = "${torque_bin}qsub $add_args $app";
  my %qsub     = runCommandAs($user, $qsub_cmd);

  if ($qsub{ 'EXIT_CODE' } == 0)
    {

    $job_id = $qsub{ 'STDOUT' };
    $job_id = cleanupJobId($job_id);

    pass("Successfully submitted a job '$job_id'");

    } # END if ($qsub{ 'EXIT_CODE' } == 0)
  else
    {

    $job_id = -1;
    fail('Unable to submit a checkpoint job: ' . $qsub{ 'STDERR' });

    } # END else

  return $job_id;

  } # END submitJob #($) 

#------------------------------------------------------------------------------
# runJobs('job.1', 'job.2', 'job.3');
#------------------------------------------------------------------------------
#
# Takes a list of job ids and attempts to run the jobs with the qrun command.
#
#------------------------------------------------------------------------------
sub runJobs #(@)
  {

  my @job_ids = @_;

  foreach my $job_id (@job_ids)
    {
  
    $job_id = cleanupJobId($job_id);

    my $cmd  = "qrun $job_id";
    my %qrun = runCommand($cmd);
    ok($qrun{ 'EXIT_CODE' } == 0, "Checking that '$cmd' ran successfully")
      or diag ("STDERR: $qrun{ 'STDERR' }");

    } # END foreach my $job_id (@job_ids)

  } # END runJobs #(@)

#------------------------------------------------------------------------------
# delJobs('job.1', 'job.2', 'job.3');
#------------------------------------------------------------------------------
#
# Takes a list of job ids and attempts to delete the jobs with the qdel 
# command.
#
#------------------------------------------------------------------------------
sub delJobs #(@)
  {

  my @job_ids = @_;

  # Variables
  my $job_str = join(" ", @job_ids) || 'ALL';
  my $cmd     = "qdel -p $job_str";
  my $result;

  # Run and check the command
  my %qdel = runCommand($cmd);

  if ($qdel{ 'EXIT_CODE' } == 0)
    {

    $result = 1;
    pass("Successfully deleted job(s) '$job_str'");

    } # END if ($qsub{ 'EXIT_CODE' } == 0)
  else
    {

    $result = 0;
    fail("Unable to delete job(s) '$job_str'");

    } # END else

  return $result;

  } # END sub delJobs #(@)

1;

__END__
