#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../lib/";


use CRI::Test;

plan('no_plan');
setDesc("Reinstall TORQUE to use BLCR checkpointing (NIGHTLY)");

my $testbase = "$FindBin::Bin/../";

execute_tests(
               "${testbase}update_source.t",
               "${testbase}blcr/reinstall.bat",
               "${testbase}torque/uninstall/uninstall.bat",
               "${testbase}torque/install/nightly_install_latest_rcs.bat",
             );
