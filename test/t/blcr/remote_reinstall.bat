#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc("Reinstall 'Berkley Lab Checkpoint/Restart (BLCR) for Linux'");

my $testbase = $props->get_property('test.base') . "blcr";

execute_tests(
        "$testbase/uninstall/remote_uninstall.bat",
        "$testbase/install/remote_install.bat",
        );
