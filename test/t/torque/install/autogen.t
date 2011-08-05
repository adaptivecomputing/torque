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

  my $build_dir   = test_lib_loc()."/../..";

  ok(-d $build_dir, "Checking if torque build dir exists") 
    or die("Torque build dir doesn't exist");

  # Run autogen
  my $autogen_cmd = "cd $build_dir; ./autogen.sh";
  my $ec = 0;
  my $max_tries = 3;
  my $total_tries = 0;
  
  do
  {
    $ec = runCommand($autogen_cmd);

  }until($max_tries == ++$total_tries || $ec == 0);

} # END MAIN
