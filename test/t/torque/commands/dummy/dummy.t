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
setDesc('Dummy test for the new ATS system');

ok(1, "This test should pass");
#ok(0, "This test should fail");
