#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


local $| = 1;  # Turn on the autoflush

my ($sleep_time) = $ARGV[0] || 300;

foreach my $i (1..$sleep_time)
  {

  print "i = $i\n";
  sleep(1);

  } # END foreach my $i (0..100)
