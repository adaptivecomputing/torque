package CRI::Util::Wiki;

use strict;
use warnings;

use TestLibFinder;
use Carp;

use lib test_lib_loc();

use CRI::Test;
use Moab::Ctrl;

use base 'Exporter';

our @EXPORT_OK = qw(
                    parse_wiki_file
                   );

#------------------------------------------------------------------------------
# my $wiki_href = parse_wiki_file({ 'file' => '/tmp/wiki.txt' });
#------------------------------------------------------------------------------
#
# Parses the specified Wiki file.
#
# PARAMETERS:
#  o file - Location of the wiki file
#
# RETURNS:
#  A hashref containing the contents of the wiki file
#
# SEE ALSO:
#  o http://www.clusterresources.com/products/mwm/docs/wiki/index.shtml
#
#------------------------------------------------------------------------------
sub parse_wiki_file #($)
  {

  my ($params) = @_;

  # Parameters
  my $file = $params->{ 'file' } || croak "Please provide the 'file'";

  # Variables
  my $rtn_href = {};

  # Slurp in the wiki file contents
  open(WIKI, "<$file")
    or croak "Unable to open '$file': $!";

  my @lines = <WIKI>;

  close WIKI
    or croak "Unable to close '$file': $!";

  # Parse the contents of the wiki file
  foreach my $line (@lines)
    {

    my @attrs    = split(/\s+/, $line);
    my $line_key = shift @attrs;

    $rtn_href->{ $line_key } = {};

    foreach my $attr (@attrs)
      {

      my ($attr_key, $attr_value) = split(/=/, $attr);
      $rtn_href->{ $line_key }{ $attr_key } = $attr_value;

      } # END foreach my $attr (@attrs)

    } # END foreach my $line (@lines)

  return $rtn_href;

  } # END sub parse_wiki_file #($)

1;
