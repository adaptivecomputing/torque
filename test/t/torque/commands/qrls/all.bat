#!/usr/bin/perl

use CRI::Test;
plan('no_plan');

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


my $testbase = $FindBin::Bin;

setDesc("ALL qrls tests");

execute_tests("$testbase/setup.t")
  or die("Could not set up for qrls tests");

execute_tests(
              "$testbase/qrls.t",
              "$testbase/qrls_h_u.t",
              "$testbase/qrls_h_o.t",
              "$testbase/qrls_h_s.t",
              "$testbase/qrls_h_sou.t",
);

execute_tests("$testbase/cleanup.t");
