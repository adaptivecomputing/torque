#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


# Test Modules
use CRI::Test;

# Test Description
plan('no_plan');
setDesc('Momctl -r <CONFIGFILE>');

# Variables
my %momctl;
my $host                     = 'localhost';
my $reconfig_file            = $props->get_property('mom.reconfig.file');
my $config_file              = $props->get_property('mom.config.file');
my $reconfig_check_poll_time = $props->get_property('tmp.mom.config.check_poll_time');
my $config_check_poll_time   = $props->get_property('mom.config.check_poll_time');
my $stdout;
my $check_cmd                = "momctl -q check_poll_time";

# Reconfigure mom to new configuration
%momctl = reconfig($reconfig_file);
$stdout = $momctl{ 'STDOUT' };
ok($stdout =~ /reconfig successful on ${host}/i, "Checking output of 'momctl -r $reconfig_file'");

# Check that mom was really reconfigured
%momctl = runCommand($check_cmd);
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that '$check_cmd' ran")
  or die "Couldn't run '$check_cmd'";
$stdout = $momctl{ 'STDOUT' };
ok($stdout =~ /${host}:\s+check_poll_time\s=\s\'check_poll_time=${reconfig_check_poll_time}\'/,
  "Verifying that check_poll_time was changed to '$reconfig_check_poll_time'");

# Reconfigure mom to old configuration
%momctl = reconfig($config_file);
$stdout = $momctl{ 'STDOUT' };
ok($stdout =~ /reconfig successful on ${host}/i, "Checking output of 'momctl -r $config_file'");

# Check that mom was really reconfigured
%momctl = runCommand($check_cmd);
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that '$check_cmd' ran")
  or die "Couldn't run '$check_cmd'";
$stdout = $momctl{ 'STDOUT' };
ok($stdout =~ /${host}:\s+check_poll_time\s=\s\'check_poll_time=${config_check_poll_time}\'/,
  "Verifying that check_poll_time was changed to '$config_check_poll_time'");


###############################################################################
# reconfig($cfg_file) - Run the reconfig command
###############################################################################
sub reconfig #($)
  {
 
  my ($cfg_file) = @_;
  my %result;
  my $cmd = "momctl -r $cfg_file";

  %result = runCommand($cmd);
  ok($result{ 'EXIT_CODE' } == 0,
    "Checking that '$cmd' ran")
   or die "Couldn't run '$cmd'";

  return %result;

  } # END sub reconfig #($)
