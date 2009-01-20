#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw(list2array);

my $blcr_conf_loc = "/etc/ld.so.conf.d/blcr.conf";

plan('no_plan');
setDesc("Adding '$blcr_conf_loc' to remote nodes");

my $cfg           = $props->get_property('blcr.home.dir') . "/lib";
my @remote_nodes  = list2array($props->get_property('torque.remote.nodes'));

foreach my $remote_node (@remote_nodes)
  {

  my $cmd  = "echo '$cfg' > $blcr_conf_loc";
  my %echo = runCommandSsh($remote_node, $cmd);
  ok($echo{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'");

  } # END foreach my $remote_node (@remote_nodes)
