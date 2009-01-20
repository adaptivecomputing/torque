#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


local $| = 1;  # Turn on the autoflush

my ($sleep_time) = $ARGV[0] || 300;

foreach my $i (1..$sleep_time)
  {

  print "i = $i\n";
  sleep(1);

  } # END foreach my $i (0..100)
