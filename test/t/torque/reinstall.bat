#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../lib/";


use CRI::Test;

plan('no_plan');
setDesc("Reinstall Latest Torque (from subversion) to use BLCR checkpointing");

my $testbase = "$FindBin::Bin/../";

execute_tests(
               "${testbase}blcr/reinstall.bat",
               "${testbase}torque/uninstall/uninstall.bat",
               "${testbase}torque/install/install_latest_rcs.bat",
             );
