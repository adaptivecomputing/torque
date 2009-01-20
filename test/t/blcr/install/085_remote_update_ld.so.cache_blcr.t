#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw( list2array );

plan('no_plan');
setDesc('Update the ld.so.cache');

my @remote_nodes = list2array($props->get_property('torque.remote.nodes'));

foreach my $remote_node (@remote_nodes)
  {

  my $ldconfig_cmd    = '/sbin/ldconfig';
  my %ldconfig_result = runCommandSsh($remote_node, $ldconfig_cmd);

  ok(! $ldconfig_result{ 'STDOUT' }, "Running ldconfig on $remote_node");

  } # END foreach my $remote_node (@remote_nodes)
