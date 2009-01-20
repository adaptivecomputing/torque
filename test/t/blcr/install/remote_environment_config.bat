#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc("Configure the environment for BLRC");

my $testbase = $props->get_property('test.base') . "blcr/install";

execute_tests(
        "$testbase/077_create_blcr_conf_remote.t",
        "$testbase/085_remote_update_ld.so.cache_blcr.t",
#        "$testbase/095_remote_check_user_environment_blcr.t",
        );
