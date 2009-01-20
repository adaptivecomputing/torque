#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw(list2array);

my $torque_conf_loc = "/etc/ld.so.conf.d/torque.conf";

plan('no_plan');
setDesc("Adding '$torque_conf_loc' to remote nodes");

my $cfg           = $props->get_property('torque.home.dir') . "/lib";
my @remote_nodes  = list2array($props->get_property('torque.remote.nodes'));

foreach my $remote_node (@remote_nodes)
  {

  my $cmd  = "echo '$cfg' > $torque_conf_loc";
  my %echo = runCommandSsh($remote_node, $cmd);
  ok($echo{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'");

  my $ldconfig_cmd = "/sbin/ldconfig";
  my %ldconfig     = runCommandSsh($remote_node, $ldconfig_cmd);
  ok($ldconfig{ 'EXIT_CODE' } == 0, "Checking exit code of '$ldconfig_cmd'");

  } # END foreach my $remote_node (@remote_nodes)
