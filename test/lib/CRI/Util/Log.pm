package CRI::Util::Log;

use strict;
use warnings;

use TestLibFinder;
use Carp;

use lib test_lib_loc();

use CRI::Test;

use base 'Exporter';

our @EXPORT_OK = qw(
                     append_file
                   );

my $log_dir = $ENV{ 'CRILogDir' } || croak "Unable to determine the 'CRILogDir' environment variable";

#------------------------------------------------------------------------------
# append_file({
#              'file' => 'stats.txt',
#              'lines' => [ '10,10,12', '8,7,8' ]
#            });
# append_file({
#              'file' => 'stats.txt',
#              'lines' => '12,9,7'
#            });
#------------------------------------------------------------------------------
#
# Appends the given line to the given file in the current log dir (as given by 
# the environment variable CRILogDir).
#
# PARAMETERS
#  o file  - (REQUIRED) The name of the file found in the CRILogDir (current
#            log directory)
#  o lines - (REQUIRED) An arrayref or a string of lines to append to the file 
#
#------------------------------------------------------------------------------
sub append_file #($)
  {

  my ($params) = @_;

  my $file  = $params->{ 'file'  } || croak "Please provide the 'file' parameter";
  my $lines = $params->{ 'lines' } || croak "Please provide the 'lines' parameter";

  if (ref $lines eq '')
    {

    $lines = [ $lines ];

    } # END if (ref $lines eq '')
  elsif (ref $lines ne 'ARRAY' )
    {
  
    croak "'lines' parameter should be a string or an ARRAYREF";
   
    } # END  elsif (ref $lines ne 'ARRAY' )
  

  my $file_loc = "$log_dir/$file";

  open(FILE, ">>$file_loc")
    or croak "Unable to open '$file_loc': $!";

  foreach my $line (@$lines)
    {

    $line =~ s/\n$//; # Remove the end new line.  We will add it

    print FILE $line . "\n"; 

    } # END foreach my $line (@$lines)

  close FILE;

  } # END sub delete_mail #($)

1;

__END__
