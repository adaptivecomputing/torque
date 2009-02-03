#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc('Uninstall Torque');

my $testbase = $FindBin::Bin;


execute_tests(
              "$testbase/shutdown.t",
              "$testbase/make_uninstall.t",
              "$testbase/rm_home.t",
             );
