#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";


use CRI::Test;

my $blcr_conf_loc = "/etc/ld.so.conf.d/blcr.conf";

plan('no_plan');
setDesc("Adding '$blcr_conf_loc' to local node");

my $cfg = $props->get_property('blcr.home.dir') . "/lib";

my $cmd  = "echo '$cfg' > $blcr_conf_loc";
my %echo = runCommand($cmd);
ok($echo{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'");
