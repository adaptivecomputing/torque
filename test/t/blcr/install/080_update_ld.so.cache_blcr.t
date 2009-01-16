#!/usr/bin/perl

use strict;
use warnings;

use CRI::Test;

plan('no_plan');
setDesc('Update the ld.so.cache');

ok(! `/sbin/ldconfig`, "Running ldconfig");
