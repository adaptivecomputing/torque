#! /usr/bin/perl

#############################################################################
#
# autogen.t
#
# Copyright (C) 2010 by Adaptive Computing Enterprises, Inc.
# All Rights Reserved.
#
#############################################################################
#
#############################################################################


use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

MAIN:
{

  plan('no_plan'); 
  setDesc('Torque autogen.sh');

  my $build_dir   = "$FindBin::Bin/../../../../";

  ok(-d ($build_dir), "Checking if torque build dir exists") 
    or die("Torque build dir doesn't exist");

  # Change directory to build dir
  ok(chdir $build_dir, "Changing directory to " . $build_dir) 
    or die("Couldn't change to torque build directory");

  # Run autogen
  my $autogen_cmd = "./autogen.sh";

  runCommand($autogen_cmd, ("test_success_die" => 1));

} # END MAIN
