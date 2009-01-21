#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc('ALL tracejob Compatibility Tests');

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die('Could not setup tracejob tests');

execute_tests(
              "$testbase/tracejob.t",
             ); 

execute_tests("$testbase/cleanup.t"); 
