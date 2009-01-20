#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


# Turn on the autoflush
local $| = 1;

use sigtrap qw(
               handler 
               PRINT_SIGNAL
               HUP
               INT
               QUIT
               ILL
               TRAP
               ABRT
               BUS
               FPE
               USR1
               SEGV
               USR2
               PIPE
               ALRM
               TERM
              );

# A simple loop
my ($sleep_time) = $ARGV[0] || 300;

foreach my $i (1..$sleep_time)
  {

  sleep(1);

  } # END foreach my $i (0..100)


###############################################################################
# PRINT_SIGNAL
###############################################################################
sub PRINT_SIGNAL #($)
  {

  my ($signal) = @_;

  print STDOUT "SIGNAL: $signal\n";

  } # END print_signal
