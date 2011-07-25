#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;

# Test Description
plan('no_plan');
setDesc('pbsnodes -r <HOST>');

SKIP:
  {
  
  skip("Test sufficiently handled by pbsnodes -s tests", 1);

  ok(1, "Test sufficiently handled by pbsnodes -s tests");

  }; # END Skip
