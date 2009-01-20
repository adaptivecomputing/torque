#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc("Quick Install 'Berkley Lab Checkpoint/Restart (BLCR) for Linux'");

my $testbase = $props->get_property('test.base') . "blcr/install";

execute_tests(
        "$testbase/010_get_blcr_src.t",
        "$testbase/030_configure_blcr.t",
        "$testbase/040_make_blcr.t",
        "$testbase/050_make_insmod_blcr.t",
#        "$testbase/060_make_check_blcr.t",
        "$testbase/070_make_install_blcr.t",
        );

#execute_tests(
#        "$testbase/environment_config.bat",
#        );
