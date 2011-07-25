#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

plan('no_plan');
setDesc('Update the ld.so.cache');

ok(! `/sbin/ldconfig`, "Running ldconfig");
