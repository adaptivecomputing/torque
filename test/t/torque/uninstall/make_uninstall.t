#! /usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;
use CRI::Utils qw(
                   run_and_check_cmd
                 );

plan('no_plan');
setDesc('Uninstall Torque (make uninstall)');


my $build_dir = "$FindBin::Bin/../../../../";
my $cmd       = "make uninstall";

ok(chdir $build_dir, "Changing directory to '$build_dir'")
  or die("Error: $!");
 
run_and_check_cmd($cmd);
