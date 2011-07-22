#!/usr/bin/perl

use FindBin;
use TestLibFinder;
use lib test_lib_loc();
 
use CRI::Test;
plan('no_plan');
setDesc('Batch: shutdown and remove torque');
use strict;
use warnings;

my $testbase = "$FindBin::Bin";

execute_tests(
    "$testbase/shutdown.t",
    "$testbase/remove.t"
    );
