package CRI::Util;

#######################################################################
# 
# File   :  CRI/Util.pm
# History:  14-sep-2009 (jdayley) Added this header and made format and
#                       comment changes.
#
########################################################################
#
# Utility subroutines for CRI::Test scripts.
# 
########################################################################

use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Carp;
use XML::Simple;
#use XML::Simple qw(:strict);

use File::Copy;

use base 'Exporter';

our @EXPORT_OK = qw(
                    parseXML
		    end_wait_script
                    list2array
                    DDHHMMSS_to_secs
                    secs_to_DDHHMMSS
                    cleanup_output
                    is_running
                    is_running_remote
                    backup_file
                    restore_file
                   );

#------------------------------------------------------------------------------
# Takes XML input, validates it (via XML::LibXML), and converts it (via XML::Simple)
#   to an equivalent Perl hashref of hash/arrays.
#------------------------------------------------------------------------------
# PARAMETERS:
#  - xml = the XML string to parse
#  - options = specify options that XML::Simple XMLin() would expect
#------------------------------------------------------------------------------
sub parseXML
{
  my ($params) = @_;

  my $xml         = $params->{xml}         || die "Must specify XML string via 'xml' key";
  my $options     = $params->{options}     || {};

  # XMLin strict mode requires this key to exist
  $options->{ForceArray} = [] unless exists $options->{ForceArray};
  
  # Make sure that KeyAttr keys are in ForceArray
  if( exists $options->{KeyAttr} )
  {
    if( ref $options->{ForceArray} eq 'ARRAY' )
    {
      my %tmp;
      $tmp{$_} = 1 foreach @{$options->{ForceArray}};

      foreach( keys %{$options->{KeyAttr}} )
      {
        push @{$options->{ForceArray}}, $_ unless exists $tmp{$_};
      }
    }
  }
  else
  {
    $options->{KeyAttr} = {};
  }
  
  my $xml_ref = {};

  eval{ $xml_ref = XMLin( $xml, %$options ); };
  die "parseXML() failed: $@" if $@ && scalar keys %$xml_ref < 1;

  return $xml_ref;
}


#------------------------------------------------------------------------------
# end_wait_script({
# 	jobs   => Array of jobs running the wait.pl script,
# 	ecodes => Array of exit states (pass or fail) for 'jobs' array (sync with 'jobs' array),
# 	rm     => Name of RM being used (determines pattern matching),
# });
#------------------------------------------------------------------------------
# Signals Jobs that are using the wait.pl script to exit either pass or fail.
#------------------------------------------------------------------------------
sub end_wait_script
{
  my ($params) = @_;

  my $jobs   = $params->{jobs}   || die "Must specify Job ID Array via 'jobs' key";
  my $ecodes = $params->{ecodes} || die "Must specify Exit Code Array via 'ecodes' key";
  my $rm     = $params->{rm}     || 'torque';

  my $sig_file = '/tmp/wait.in';
  
  for( my $i=0; $i < scalar @$jobs; $i++)
  {
    my $pattern = _get_pattern($rm, $jobs->[$i]);

    my %cmd    = runCommand("pgrep $pattern");
    my $output = $cmd{STDOUT};
    $output = (split "\n", $output)[0];

    qx/echo '$output $ecodes->[$i]' >> $sig_file/;
  }

  sub _get_pattern
  {
    my $rm  = shift;
    my $job = shift;

    if( $rm =~ /torque/i )
    {
      my $trq_home = $props->get_property('Torque.Home.Dir');
      return "-f $trq_home/mom_priv/jobs/$job\\.";
    }
    elsif( $rm =~ /slurm/i )
    {
      return "-f job0*$job";
    }
  }
}

#------------------------------------------------------------------------------
# @array = list2array('item1,item2|item3 item4')
#------------------------------------------------------------------------------
#
# Takes a string list seperated by either a ',', '|', or a space character and
# returns an array of the items.
#
#------------------------------------------------------------------------------
sub list2array #($)
  {

  my ($list) = @_;

  my @rtn_values = split(/,|\s+/, $list);

  return @rtn_values;

  } # END sub hostlist2array

#------------------------------------------------------------------------------
# my $seconds = DDHHMMSS_to_secs( '21:13:40:59' );
#------------------------------------------------------------------------------
#
#  Takes a time formatted as Days:Hours:Minutes:Seconds and converts it to
#  seconds. The syantax is [[DD:][HH:]MM:SS.
#
#------------------------------------------------------------------------------
sub DDHHMMSS_to_secs #($)
  {

  my ($time_string) = @_;

  my @time = split ':', $time_string;
  unshift @time, 0 while scalar @time < 4;

  return $time[0] * 24 * 60 * 60 + $time[1] * 60 * 60 + $time[2] * 60 + $time[3];

  } # END sub DDHHMMSS_to_secs #($)

#------------------------------------------------------------------------------
# my $formatted_time = secs_to_DDHHMMSS( 456025 );
#------------------------------------------------------------------------------
#  Takes a time in seconds and converts it to Days:Hours:Minutes:Seconds.
#  Returns MM:SS by default.
#------------------------------------------------------------------------------
sub secs_to_DDHHMMSS #($)
  {
  my $time = $_[0];

  my $negative = 1 if $time < 0;
  $time *= -1 if $negative;

  my $format = undef;

  my $days = int( $time / 86400 );
  $time   -= $days * 86400;
  $format .= sprintf('%d:', $days) unless $days < 1;

  my $hrs  = int( $time / 3600 );
  $time   -= $hrs * 3600;
  if( $hrs > 0 )
  {
    $format .= sprintf('%d:', $hrs);
  }
  else
  {
    $format .= sprintf('%02d:', $hrs);
  }

  my $mins = int( $time / 60 );
  $time   -= $mins * 60;
  $format .= sprintf('%02d:', $mins);

  $format .= sprintf('%02d', $time);

  $format = "-$format" if $negative;

  return $format;

  }

#------------------------------------------------------------------------------
# my $new_output = cleanup_output( $output);
#------------------------------------------------------------------------------
#
# Cleans up string by removing all leading and trailing whitespace in the
# string. Intended for command output cleanup.
#
#------------------------------------------------------------------------------
sub cleanup_output #($)
  {

  my ($string) = @_;

  $string =~ s/^\s+//s;
  $string =~ s/\s+$//s;

  return $string;

  } # END sub cleanup_output #($)

#------------------------------------------------------------------------------
# my $success = is_running('moab');
#------------------------------------------------------------------------------
#
# Makes sure that a given process is running and not just a zombie (defunct
# process). pgrep -x will return zombie processes.  Returns 1 if a non-zombie
# process is running and 0 if not.
#
#------------------------------------------------------------------------------
sub is_running #($)
  {

  my ($process) = @_;

  # Variables
  my $running = 0;

  # Commands
  my $pgrep_cmd = "pgrep -d , -x $process";
  my $ps_cmd;

  # Return Hashes
  my %pgrep;
  my %ps;

  # Look for running processes
  %pgrep = runCommand($pgrep_cmd);

  return $running
    if $pgrep{ 'EXIT_CODE' } == 1;

  # Only count non-zombie processes
  my @pids = list2array($pgrep{ 'STDOUT' });

  foreach my $pid (@pids)
    {

    # Detaint the pid, ignore invalid pids
    if ($pid =~ /^(\d+)$/)
      {

      $pid = $1;

      } # END if ($pid =~ /^(\d+)$/)
    else
      {

      diag("Bad pid: '$pid'");
      next;

      } # END if ($pid =~ /^(\d+)$/)

    $ps_cmd = "ps -o pid,stat ax | grep $pid | grep -v grep | grep -v 'Z'";
    %ps     = runCommand($ps_cmd);

    if ($ps{ 'EXIT_CODE' } == 0)
      {

      $running = 1;
      next;

      } # END if ( $ps{ 'EXIT_CODE' } == 0 )

    } # END foreach my $pid (@pids)

  return $running;

  } # END sub is_running #($)

#------------------------------------------------------------------------------
# my $success = is_running_remote('moab', 'node1');
#------------------------------------------------------------------------------
#
# Checks that a given process is running on a remote node and is not just a
# zombie (defunct process). pgrep -x will return zombie processes.  Returns 1
# if a non-zombie process is running and 0 if not.
#
#------------------------------------------------------------------------------
sub is_running_remote #($$)
  {

  my ($process, $node) = @_;

  # Variables
  my $running = 0;

  # Commands
  my $pgrep_cmd = "pgrep -d , -x $process";
  my $ps_cmd;

  # Return Hashes
  my %pgrep;
  my %ps;

  # Look for running processes
  %pgrep = runCommandSsh($node, $pgrep_cmd);

  return $running
    if $pgrep{ 'EXIT_CODE' } == 1;

  # Only count non-zombie processes
  my @pids = list2array($pgrep{ 'STDOUT' });

  foreach my $pid (@pids)
    {

    # Detaint the pid, ignore invalid pids
    if ($pid =~ /^(\d+)$/)
      {

      $pid = $1;

      } # END if ($pid =~ /^(\d+)$/)
    else
      {

      diag("Bad pid: '$pid'");
      next;

      } # END if ($pid =~ /^(\d+)$/)

    $ps_cmd = "ps -o pid,stat ax | grep $pid | grep -v grep | grep -v Z";
    %ps = runCommandSsh($node, $ps_cmd);

    if ($ps{ 'EXIT_CODE' } == 0)
      {

      $running = 1;
      next;

      } # END if ( $ps{ 'EXIT_CODE' } == 0 )

    } # END foreach my $pid (@pids)

  return $running;

  } # END sub is_running_remote #($)

#------------------------------------------------------------------------------
# backup_file({ 'filename' => '/tmp/file.txt' });
#------------------------------------------------------------------------------
#
# Backs up the the given filename to a backup location.  The backup location is
# the filename with a .bak appended at the end
#
#------------------------------------------------------------------------------
sub backup_file #($)
{

my ($params) = @_;

my $filename = $params->{ 'filename' } 
  or croak "Please provide the 'filename'";

my $filename_bak = "$filename.bak";

ok(copy($filename, $filename_bak), "Backing up $filename to $filename_bak")
  or croak "Unable to copy $filename to $filename.bak: $!";

return 1;

} # END sub backup_file #($)

#------------------------------------------------------------------------------
# restore_file({ 'filename' => '/tmp/file.txt' });
#------------------------------------------------------------------------------
#
# Takes the given filename and looks for the backup file created by the 
# backup_file() subroutine and restores the backup file to the original file
# location.
#
#------------------------------------------------------------------------------
sub restore_file #($)
{

my ($params) = @_;

my $filename = $params->{ 'filename' } 
  or croak "Please provide the 'filename'";

my $filename_bak = "$filename.bak";

if(-e $filename_bak)
{

  ok(copy($filename_bak, $filename), "Restoring $filename_bak to $filename")
    or croak "Unable to copy $filename to $filename_bak: $!";
} # END if(-e $filename_bak)
else
{

cluck("No backup file found for $filename");

} # END else

return 1;

} # END sub restore_file #($)

1;

__END__
