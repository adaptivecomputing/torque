#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

plan('no_plan');
setDesc("Configure the environment for BLRC");

my $testbase = $FindBin::Bin;


execute_tests(
        "$testbase/072_create_blcr_conf.t",
        "$testbase/080_update_ld.so.cache_blcr.t",
        "$testbase/090_check_user_environment_blcr.t",
        );
