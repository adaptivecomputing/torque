#!/usr/bin/perl 

use CRI::Test;
plan('no_plan');
setDesc('Install Latest Torque from subversion to use BLCR checkpointing');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";


my $testbase = $props->get_property('test.base') . "torque/install"; 

execute_tests(
"$testbase/get_latest_rcs.t",
"$testbase/configure.t",
"$testbase/make_clean.t",
"$testbase/make.t",
"$testbase/make_install.t",
"$testbase/setup_queues.t",
"$testbase/config_local_mom.t",
"$testbase/cp_chkpt_scripts.t",
"$testbase/make_packages.t",
"$testbase/install_remote_mom.t",
"$testbase/config_server_cluster.t",
"$testbase/create_torque_conf.t",
"$testbase/create_torque_conf_remote.t",
"$testbase/startup_cluster.t",
"$testbase/check_blcr.t",
);
