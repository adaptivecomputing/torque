#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;

my $torque_conf_loc = "/etc/ld.so.conf.d/torque.conf";

plan('no_plan');
setDesc("Adding '$torque_conf_loc' to local node");

my $cfg = $props->get_property('torque.home.dir') . "/lib";

my $echo_cmd  = "echo '$cfg' > $torque_conf_loc";
my %echo      = runCommand($echo_cmd);
ok($echo{ 'EXIT_CODE' } == 0, "Checking exit code of '$echo_cmd'");

my $ldconfig_cmd = "ldconfig";
my %ldconfig     = runCommand($ldconfig_cmd);
ok($ldconfig{ 'EXIT_CODE' } == 0, "Checking exit code of '$ldconfig_cmd'");
