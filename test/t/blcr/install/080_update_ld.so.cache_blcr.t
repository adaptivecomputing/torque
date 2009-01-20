#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc('Update the ld.so.cache');

ok(! `/sbin/ldconfig`, "Running ldconfig");
