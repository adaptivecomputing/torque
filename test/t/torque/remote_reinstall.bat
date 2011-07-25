#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

plan('no_plan');
setDesc("Remote Reinstall TORQUE");

my $testbase = "$FindBin::Bin/../";


execute_tests(
               "${testbase}torque/update_source.t",
               "${testbase}torque/uninstall/uninstall.bat",
               "${testbase}torque/install/remote_install.bat",
             );
