package Torque::Test::Qhold::Utils;

use strict;
use warnings;

use CRI::Test;

use Torque::Test::Utils        qw( run_and_check_cmd    );
use Torque::Test::Regexp       qw( CHECKPOINT_FILE_NAME );
use Torque::Test::Qstat::Utils qw( parse_qstat_fx       );

use base 'Exporter';

our @EXPORT_OK = qw(
                   verify_qhold_chkpt
                   verify_qhold
                   );

###############################################################################
# verify_qhold_chkpt
###############################################################################
sub verify_qhold_chkpt #($)
  {

  my ($job_id) = @_;

  # Variables
  my $count;
  my $checkpoint_name;
  my $checkpoint_path;
  my $job_state        = '';
  my $cmd;
  my %qstat;
  my %job_info;

  # Check every second for two minutes for the checkpoint to be created
  $count = 0;
  while (    $count < 120 
         and (   ! defined $checkpoint_name
              or $job_state ne 'H'))
    {

    sleep 1;

    # We parse the job information manually so we don't spam the test output
    $cmd      = "qstat -fx $job_id";
    %qstat    = runCommand($cmd);
    %job_info = parse_qstat_fx($qstat{ 'STDOUT' });

    if (defined $job_info{ $job_id }{ 'checkpoint_name' })
      {

      $checkpoint_name = $job_info{ $job_id }{ 'checkpoint_name' }; 

      }

    $job_state = $job_info{ $job_id }{ 'job_state' };

    $count++;

    } # END while (    $count < 120 
      #            and (   ! defined $checkpoint_name
      #                 or $job_state ne 'H'))

  # Make sure that we just didn't run out of time
  if (! defined $checkpoint_name)
    {

    fail("'checkpoint_name' attribute not found for job '$job_id'");
    return;

    } # END if (! defined $checkpoint_name)

  # Test for the hold state
  ok($job_state eq 'H', "Checking for a job_state of 'H' for non-checkpointable job '$job_id'")
    or diag("Job '$job_id' state: $job_state");

  # Perform the basic tests
  ok($checkpoint_name  =~ /${\CHECKPOINT_FILE_NAME}/,  "Checking the 'checkpoint_name' attribute of the job")
    or diag("checkpoint name: $checkpoint_name");
  ok(exists $job_info{ $job_id }{ 'checkpoint_time' }, "Checking for the existence of the job attribute 'checkpoint_time'");

  # Check for the actual file 
  $checkpoint_path = $props->get_property('torque.home.dir') . "checkpoint/${job_id}.CK/$checkpoint_name";
  ok(-e $checkpoint_path, "Checking that '$checkpoint_path' exists");

  } # END sub verify_qhold_chkpt #($)

###############################################################################
# verify_qhold
###############################################################################
sub verify_qhold #($)
  {

  my ($job_id) = @_;

  # Variables
  my $count;
  my $checkpoint_name;
  my $checkpoint_path;
  my $job_state        = '';
  my $cmd;
  my %qstat;
  my %qrerun;
  my %job_info;

  # We need to qrerun to make it take effect
  $cmd     = "qrerun $job_id";
  %qrerun  = run_and_check_cmd($cmd);
  ok($qrerun{ 'EXIT_CODE' } == 0, "Checking the exit code of '$cmd'");

  # Check every second for two minutes for the hold to take effect
  $count = 0;
  while (    $count < 120 
         and $job_state ne 'H')
    {

    sleep 1;

    # We parse the job information manually so we don't spam the test output
    $cmd      = "qstat -fx $job_id";
    %qstat    = runCommand($cmd);
    %job_info = parse_qstat_fx($qstat{ 'STDOUT' });

    $job_state = $job_info{ $job_id }{ 'job_state' };

    $count++;

    } # END while ($count < 120 and $checkpoint_name)

  # Test for the hold state
  ok($job_state eq 'H', "Checking for a job_state of 'H' for non-checkpointable job '$job_id'")
    or diag("Job '$job_id' state: $job_state");

  } # END sub verify_qhold_chkpt #($)

1;

=head1 NAME

Torque::Test::Qhold::Utils - Some useful Torque test utilities for the qhold command

=head1 SYNOPSIS

 use Torque::Test::Qhold::Utils qw( 
                                    verify_qhold_chkpt
                                    verify_qhold
                                  );
 use CRI::Test;

 # verify_qhold_chkpt
 verify_qhold_chkpt('1.host1');
 
 # verify_qhold
 verify_qhold('2.host1');

=head1 DESCRIPTION

Some useful methods to use when running the qhold command.

=head1 SUBROUTINES/METHODS

=over 4

=item verify_qhold_chkpt($job_id)

Runs a series of tests on a given job to determine if it has been placed in the hold state and has been checkpointed properly

=item verify_qhold($job_id)

Runs a series of tests on a given job to determine if it has been placed in the hold state properly

=back

=head1 DEPENDENDCIES

Moab::Test, Torque::Test::Regexp, Torque::Test::Qstat::Utils

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
