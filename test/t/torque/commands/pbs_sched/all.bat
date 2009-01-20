#!/usr/bin/perl 

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

setDesc('ALL pbs_sched Compatibility Tests');

my $testbase = $FindBin::Bin;


execute_tests(
              "$testbase/pbs_sched.t",
             ); 
