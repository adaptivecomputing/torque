#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
use Torque::Util qw(
                            run_and_check_cmd
                          );
plan('no_plan');
setDesc('pbs_mom -C');

my $checkpoint_dir = $props->get_property('Torque.Home.Dir').'/tmp_checkpoint';

# Create the checkpoint directory
run_and_check_cmd("mkdir -p $checkpoint_dir");
run_and_check_cmd("chmod 1770 $checkpoint_dir");

stopPbsmom();
startPbsmom({ 'args' => "-C $checkpoint_dir" });

# Restart pbs_mom without the checkpoint file
#diag("Restarting pbs_mom");
#stopPbsmom();
#startPbsmom();
