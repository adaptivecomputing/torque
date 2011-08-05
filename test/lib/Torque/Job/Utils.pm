package Torque::Job::Utils;

use strict;
use warnings;

use CRI::Test;
use Carp;

use Torque::Util::Qstat qw( qstat_fx );

use base 'Exporter';

our @EXPORT = qw(
                checkForJob
                detaintJobId
                cleanupJobId
                generateArrayIds
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
  my $qstat  = qstat_fx();

  # Look for the job_id in the list
  $found = 1 
    if defined $qstat->{$job_id};

  return $found;
}

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

###############################################################################
# generateArrayIds
###############################################################################
sub generateArrayIds #($)
  {

  my ($params) = @_;

  # Gather parameters
  my $job_id = $params->{ 'job_id' } || die "Please provide a 'job_id'";
  my $id_exp = $params->{ 'id_exp' } || die "Please provide a 'id_exp'";

  # Check job id 
  my $regex = qr/^\d+\[\](\.\S+)?$/;
  die "Please provide a job id that matches $regex. Job id given: $job_id"
    unless $job_id =~ $regex;

  # Variables
  my @array_ids = ();
  my @indexes   = ();

  # Determine array indexes
  ($id_exp) = split(/%/, $id_exp); # Ignore everything after %
  my @tmp_exp = split(/,/, $id_exp);

  foreach my $id_exp (@tmp_exp)
    {

    my ($x, $y) = split('-', $id_exp);

    if (! defined $y)
      {

      push @indexes, $x;
      next;

      } # END if (! defined $y)
    else
      {

      push @indexes, ($x..$y);

      } # END else

    } # END foreach my $id_exp (@tmp_exp)


  # Generate the Array Ids
  foreach my $index (@indexes)
    {

    push @array_ids, $job_id;
    $array_ids[-1] =~ s/\[\]/[$index]/;

    } # END foreach my $index (@indexes)

  return @array_ids;

  } # END sub generateArrayIds #($)

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

=item generateArrayIds

Takes a job array id and a id_exp and returns a list of job ids for the job arrays sub jobs.  The id_exp is the value given to the qsub -t flag.

=back

=head1 DEPENDENDCIES

Moab::Test

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
