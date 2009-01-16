#!/usr/bin/perl 

use CRI::Test;
plan('no_plan');
setDesc('Install Latest Torque from subversion on a multi-node cluster');
use strict;
use warnings;

my $testbase = $props->get_property('test.base') . "torque/install"; 

execute_tests(
"$testbase/get_latest_rcs.t",
"$testbase/configure.t",
"$testbase/make_clean.t",
"$testbase/make.t",
"$testbase/make_install.t",
"$testbase/setup_queues.t",
"$testbase/config_mom.t",
"$testbase/make_packages.t",
"$testbase/install_remote_mom.t",
"$testbase/create_torque_conf.t",
"$testbase/create_torque_conf_remote.t",
"$testbase/config_server_cluster.t",
"$testbase/startup_cluster.t",
);
