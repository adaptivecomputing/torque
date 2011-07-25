#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('pbs_server -S');

my $port = '3334';

stopPbsserver();
startPbsserver({ 'args' => "-S $port" });

# TODO:  In these tests suites we are not doing anything with moab or any other
#        schedular.  We may need to do a test in a moab-torque compatibility tests
