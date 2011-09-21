#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();
 
use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('Configure Torque MOMs on the Compute Nodes');

my @nodes   = split ',', ($props->get_property('Torque.Remote.Nodes') || '');

createMomCfg();
createMomCfg({host => $_}) foreach @nodes;
