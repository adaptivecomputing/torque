package Torque::Job::Ctrl;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


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
                submitJob
                submitSleepJob
                submitCheckpointJob
                runJobs 
                delJobs
                );

###############################################################################
# submitSleepJob 
###############################################################################
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

###############################################################################
# submitCheckpointJob 
###############################################################################
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

###############################################################################
# submitJob 
###############################################################################
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

###############################################################################
# runJobs - Run TORQUE jobs
###############################################################################
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

###############################################################################
# delJobs - Delete jobs
###############################################################################
sub delJobs #(@)
  {

  my @job_ids = @_;

  # Variables
  my $job_str = join(" ", @job_ids);
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

=head1 NAME

Torque::Job::Ctrl - A module to control torque jobs

=head1 SYNOPSIS

 use Torque::Job::Ctrl qw (
                           submitJob
                           submitSleepJob
                           submitCheckpointJob
                           runJobs
                           delJobs
                          );
 # Submit a job
 my $params = {
               'user'       => $user1,
               'torque_bin' => $torque_bin,
               'app'        => '/tmp/app.pl',
               'add_args'   => '-l walltime=30:00'
             };
 my $job_id = submitJob($params);

 # Submit a sleep job
 my $params = {
               'user'       => $user1,
               'torque_bin' => $torque_bin,
               'sleep_time' => 200,
               'add_args'   => '-l walltime=30:00'
             };
 my $job_id = submitSleepJob($params);

 # Submit a checkpoint job
 my $params = {
               'user'       => $user1,
               'torque_bin' => $torque_bin,
               'app'        => '/tmp/app.pl',
               'add_args'   => '-l walltime=30:00'
             };
 my $job_id = submitCheckpointJob($params);


 # delJobs
 my @job_ids = qw(1.host 2.host);
 delJobs(@job_ids);

 # runJobs
 my @job_ids = qw(1.host 2.host);
 runJobs(@job_ids);

=head1 DESCRIPTION

Utility methods to control torque jobs

=head1 SUBROUTINES/METHODS

=over 4

=item submitCheckpointJob

Takes a hashref of parameters and submits a job.  It returns the job_id.  The following hash illustrates the possible values:

my $params = {
               'user'       => 'user1',
               'torque_bin' => '/var/spool/torque/bin',
               'app'        => '/tmp/app.pl',
               'add_args'   => '-l walltime=30:00',
               'c_value'    => 'shutdown'
             };

=item submitSleepJob

Takes a hashref of parameters and submits a job.  It return the job_id.  The following hash illustrates the possible values:

my $params = {
               'user'       => 'user1',
               'torque_bin' => '/var/spool/torque/bin',
               'sleep_time' => 200,
               'add_args'   => '-l walltime=30:00'
             };

=item submitCheckpointJob

Takes a hashref of parameters and submits a job with checkpointing enabled.  It returns the job_id.  The following hash illustrates the possible values:

my $params = {
               'user'       => 'user1',
               'torque_bin' => '/var/spool/torque/bin',
               'app'        => '/tmp/app.pl',
               'add_args'   => '-l walltime=30:00'
             };


=item delJobs

Takes a list of jobs and attempts to delete them.  Uses the 'qdel -p' command to do so.

=item runJobs

Takes a list of jobs and attempts to run them using hte 'qrun' command.

=back

=head1 DEPENDENDCIES

Moab::Test

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
