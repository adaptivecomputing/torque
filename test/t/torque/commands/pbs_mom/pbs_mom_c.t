#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

use Torque::Util qw(
                            run_and_check_cmd
                            query_mom_cfg
                          );
use Torque::Ctrl        qw(
                            stopPbsmom
                            startPbsmom
                          );


plan('no_plan');
setDesc('pbs_mom -c');

# Variables
my $reconfig_file           = $props->get_property('mom.reconfig.file');              # This file is created by setup.t
my $tmp_check_poll_time     = $props->get_property('tmp.mom.config.check_poll_time');
my $default_check_poll_time = '45';
my $cur_check_poll_time;

# Commands
my $pgrep_cmd   = 'pgrep -x pbs_mom';
my $pbs_mom_cmd = "pbs_mom -c $reconfig_file";

# Hashes
my %pgrep;
my %pbs_mom;

# Make sure pbs_mom is stopped
stopPbsmom();

# Start pbs_mom
%pbs_mom = runCommand($pbs_mom_cmd);
ok($pbs_mom{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_mom_cmd'")
  or diag("EXIT_CODE: $pbs_mom{ 'EXIT_CODE' }\nSTDERR: $pbs_mom{ 'STDERR' }");

# Make sure that pbs_mom has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_mom is running");

# Check that pbs_mom was reconfigured
sleep 2;
$cur_check_poll_time = query_mom_cfg('check_poll_time');
ok($cur_check_poll_time eq $tmp_check_poll_time, "Verify that mom was reconfigured")
  or diag("Expected check_poll_time: '$tmp_check_poll_time'\nRecieved check_poll_time: '$cur_check_poll_time'");

# Restart pbs_mom to it's default state
diag("Restarting pbs_mom");
stopPbsmom();
startPbsmom();

# Check that pbs_mom was reset to it's original state
sleep 2;
$cur_check_poll_time = query_mom_cfg('check_poll_time');
ok($cur_check_poll_time eq $default_check_poll_time, "Verify that mom was reset to its original state")
  or diag("Expected check_poll_time: '$default_check_poll_time'\nRecieved check_poll_time: '$cur_check_poll_time'");
