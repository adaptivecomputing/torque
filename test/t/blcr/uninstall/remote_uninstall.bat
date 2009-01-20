#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc("Uninstall 'Berkley Lab Checkpoint/Restart (BLCR) for Linux'");

my $testbase = $props->get_property('test.base') . "blcr/uninstall";

execute_tests(
        "$testbase/015_remote_make_uninstall_blcr.t"
        );

