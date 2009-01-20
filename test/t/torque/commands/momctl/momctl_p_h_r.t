#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


# Test Modules
use CRI::Test;

# Test Description
plan('no_plan');
setDesc('Momctl -p <PORT> -p $port -h <HOST> -r <CONFIGFILE>');

# Variables
my %momctl;
my $host                     = $props->get_property('MoabHost');
my $port                     = $props->get_property('mom.host.port');
my $reconfig_file            = $props->get_property('mom.reconfig.file');
my $config_file              = $props->get_property('mom.config.file');
my $reconfig_check_poll_time = $props->get_property('tmp.mom.config.check_poll_time');
my $config_check_poll_time   = $props->get_property('mom.config.check_poll_time');
my $stdout;
my $check_cmd                = "momctl -p $port -h $host -q check_poll_time";

# Reconfigure mom to new configuration
%momctl = reconfig($host, $port, $reconfig_file);
$stdout = $momctl{ 'STDOUT' };
ok($stdout =~ /reconfig successful on ${host}/i, "Checking output of 'momctl -p $port -h $host -r $reconfig_file'");

# Check that mom was really reconfigured
%momctl = runCommand($check_cmd);
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that '$check_cmd' ran")
  or die "Couldn't run '$check_cmd'";
$stdout = $momctl{ 'STDOUT' };
ok($stdout =~ /${host}:\s+check_poll_time\s=\s\'check_poll_time=${reconfig_check_poll_time}\'/,
  "Verifying that check_poll_time was changed to '$reconfig_check_poll_time'");

# Reconfigure mom to old configuration
%momctl = reconfig($host, $port, $config_file);
$stdout = $momctl{ 'STDOUT' };
ok($stdout =~ /reconfig successful on ${host}/i, "Checking output of 'momctl -p $port -h $host -r $config_file'");

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
 
  my ($host, $port, $cfg_file) = @_;
  my %result;
  my $cmd = "momctl -p $port -h $host -r $cfg_file";

  %result = runCommand($cmd);
  ok($result{ 'EXIT_CODE' } == 0,
    "Checking that '$cmd' ran")
   or die "Couldn't run '$cmd'";

  return %result;

  } # END sub reconfig #($)
