#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;

# Test Description
plan('no_plan');
setDesc('pbsnodes -o <HOST>');
SKIP:
  {

  skip("Test for pbsnodes -o are sufficiently done while doing tests for pbsnodes -l", 1);

  ok(1, "Test for pbsnodes -o are sufficiently done while doing tests for pbsnodes -l");

  };
