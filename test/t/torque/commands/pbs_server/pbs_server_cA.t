#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('pbs_server -A');

my $acct_file   = "/tmp/pbs_server" . time() . ".acct";

stopPbsserver();
startPbsserver({ 'args' => "-A $acct_file" });

ok(-e $acct_file, "Checking for file '$acct_file'");
