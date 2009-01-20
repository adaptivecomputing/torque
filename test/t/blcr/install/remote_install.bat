#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc("Install 'Berkley Lab Checkpoint/Restart (BLCR) for Linux'");

my $testbase = $props->get_property('test.base') . "blcr/install";

execute_tests(
        "$testbase/015_remote_get_blcr_src.t",
        "$testbase/035_remote_configure_blcr.t",
        "$testbase/045_remote_make_blcr.t",
        "$testbase/055_remote_make_insmod_blcr.t",
#        "$testbase/065_remote_make_check_blcr.t",
        "$testbase/075_remote_make_install_blcr.t",
        );

execute_tests(
        "$testbase/remote_environment_config.bat",
        );
