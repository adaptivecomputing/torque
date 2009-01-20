#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc("Uninstall 'Berkley Lab Checkpoint/Restart (BLCR) for Linux'");

my $testbase = $FindBin::Bin;


execute_tests(
        "$testbase/010_make_uninstall_blcr.t"
        );

