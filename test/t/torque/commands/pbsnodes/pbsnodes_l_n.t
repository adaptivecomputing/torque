#!/usr/bin/perl

use strict;
use warnings;

# Test Modules
use CRI::Test;

# Test Description
plan('no_plan');
setDesc('pbsnodes -l -n <HOST>');

SKIP:
  {
  
  skip("Test sufficiently handled by pbsnodes -o -N tests", 1);

  ok(1, "Test sufficiently handled by pbsnodes -o -N tests");

  }; # END Skip

