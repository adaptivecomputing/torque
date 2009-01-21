#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc("RELEASE qrls Compatibility Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t")
  or die("Could not setup for qrls tests");

execute_tests(
              "$testbase/qrls.t",
              "$testbase/qrls_h_u.t",
              "$testbase/qrls_h_o.t",
              "$testbase/qrls_h_s.t",
              "$testbase/qrls_h_sou.t",
);

execute_tests("$testbase/cleanup.t");
