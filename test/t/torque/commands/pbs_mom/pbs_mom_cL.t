#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('pbs_mom -L');

my $log_file    = "/tmp/mom-" . time() . ".log";

# Make sure pbs_mom is stopped
stopPbsmom();
startPbsmom({ 'args' => "-L $log_file" });

ok(-e $log_file, "Checking that '$log_file' exists");
