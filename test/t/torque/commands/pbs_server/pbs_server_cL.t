#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('pbs_server -L');

my $log_file    = "/tmp/pbs_server-". time() . ".log";

stopPbsserver();
startPbsserver({ 'args' => "-L $log_file" });

ok(-e $log_file, "Checking for file '$log_file'");
