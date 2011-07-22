package Torque::Job::Utils;

use strict;
use warnings;

use CRI::Test;
use Carp;

use Torque::Test::Qstat::Utils qw(
                                    parse_qstat_fx
                                 );

use base 'Exporter';

our @EXPORT = qw(
                checkForJob
                detaintJobId
                cleanupJobId
                );
 
###############################################################################
# checkForJob - Checks for a job in the qstat list
###############################################################################
sub checkForJob #($)
  {

  my ($job_id) = @_;

  $job_id = cleanupJobId($job_id);

  # Return value
  my $found = 0; 

  # Get the list of job_ids in the queue
  my $fx_cmd = 'qstat -f -x';
  my %qstat  = runCommand($fx_cmd);
  ok($qstat{ 'EXIT_CODE' } == 0, 'Verifying that the list of jobs in the queue was retrieved')
    or carp 'Unable to retrieved list of jobs';

  # If an empty string then no job_ids
  return $found
    if $qstat{ 'STDOUT' } eq '';

  my %qstat_fx = parse_qstat_fx($qstat{ 'STDOUT' });

  # Look for the job_id in the list
  $found = 1 
    if defined $qstat_fx{ $job_id };

  return $found;

  } # checkForJob #($)

###############################################################################
# detaint_job_id - Launders tainted job_ids
###############################################################################
sub detaint_job_id #($)
  {

    my ($job_id) = @_;

    if ($job_id =~ /^([-\@\w.]+)$/) 
      {
	    $job_id = $1;	# $job_id now untainted
      } 
    else 
      {
      fail("Tainted data in the '$job_id'");
      }

    return $job_id;

  } # END sub detaint_job_id #($)

###############################################################################
# cleanupJobId
###############################################################################
sub cleanupJobId #($)
  {
  
  my ($job_id) = @_;

  # Remove any newlines
  chomp $job_id;

  # Detaint the job_id
  $job_id = detaint_job_id($job_id);

  return $job_id;

  } # END sub cleanupJobId #($)

1;

=head1 NAME

Torque::Job::Utils - A module to control torque jobs

=head1 SYNOPSIS

 use Torque::Job::Utils qw (
                             checkForJob
                             detaintJobId
                             cleanupJobId
                           );

=head1 DESCRIPTION

Utilities for jobs

=head1 SUBROUTINES/METHODS

=over 4

=item checkForJob

Takes a job id and checks for it in the list of jobs returned by qstat -f -x. Returns a 1 if found 0 otherwise.

=item detaintJobId

Takes a job_id and returns a detainted job id

=item cleanupJobId

Takes a job_id and returns the job_id in an expected format.

=back

=head1 DEPENDENDCIES

Moab::Test

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
