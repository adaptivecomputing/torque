package CRI::Util::Time;

use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Time::Local;

use base 'Exporter';

our @EXPORT_OK = qw(
  date_to_epoch
  days_in_month
  revertClock
);

#------------------------------------------------------------------------------
# my $epoch = date_to_epoch('12/25/2010 13:45:22');
#------------------------------------------------------------------------------
#  Converts a datetime to a epoch timestamp
#------------------------------------------------------------------------------
sub date_to_epoch
{
  my $date = shift || die 'No Date Specified (Format MM/DD/YYYY hh:mm:ss)';

  if ( $date =~ m/(\d+)\/(\d+)\/(\d+)\s+(\d+):(\d+):(\d+)/ )
  {
    my ($month, $day, $year, $hour, $minute, $second) = ($1, $2, $3, $4, $5, $6);

    return timelocal($second, $minute, $hour, $day, $month - 1, $year - 1900);
  }
  else
  {
    die "Invalid DateTime Format! Must be 'MM/DD/YYYY hh:mm:ss'";
  }
}

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
sub days_in_month
{
  my ($month, $year) = @_;

  # Check bounds
  die "Invalid Month!" if $month < 1 or $month > 12;

  # Convert to localtime format
  my ($tmonth, $tyear) = ($month-1, $year-1900);

  # Get date of one second before next month
  if ( $tmonth == 11 )
    {
    $tmonth = 0;
    $tyear += 1;
    }
  else
    {
    $tmonth += 1;
    }

  # One second before start of next month
  my (undef, undef, undef, $lday, $lmon, $lyear, undef) = localtime(timelocal(0, 0, 0, 1, $tmonth, $tyear) - 1);

  return $lday;
}

#------------------------------------------------------------------------------
# Tries multiple ways to revert clock on machine.
#------------------------------------------------------------------------------
# PARAMETERS:
# - revert_cmd = a string of a user-defined command to use for reverting clock
#------------------------------------------------------------------------------
sub revertClock
{
  my @default_reverts = (
    'ntpdate arc.ac',
    'ntpdate time.nist.gov',
    'ntpdate-debian',
  );

  my ($params) = @_;

  my $custom_revert = $params->{revert_cmd};

  unshift @default_reverts, $custom_revert if defined $custom_revert;

  my $reverted = 0;

  foreach my $cmd (@default_reverts)
  {
    my $exit_code = runCommand($cmd);

    if( $exit_code == 0 )
    {
      $reverted++;
      last;
    }
  }

  return $reverted;
}

1;
