#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;
use Torque::Test::Utils qw( hostlist2array );

# Test Description
plan('no_plan');
setDesc('Momctl -f <HOSTLISTFILE> -r <CONFIGFILE>');

# Variables
my %momctl;
my $host_list_file           = $props->get_property('mom.host.list.file');
my @hosts                    = hostlist2array($host_list_file);
my $reconfig_file            = $props->get_property('mom.reconfig.file');
my $config_file              = $props->get_property('mom.config.file');
my $reconfig_check_poll_time = $props->get_property('tmp.mom.config.check_poll_time');
my $config_check_poll_time   = $props->get_property('mom.config.check_poll_time');
my $stdout;
my $check_cmd                = "momctl -f $host_list_file -q check_poll_time";

# Reconfigure mom to new configuration
%momctl = reconfig($host_list_file, $reconfig_file);
$stdout = $momctl{ 'STDOUT' };
foreach my $host (@hosts)
  {
  ok($stdout =~ /reconfig successful on ${host}/i, "Checking output of 'momctl -f $host_list_file -r $reconfig_file' for $host");
  }

# Check that mom was really reconfigured
%momctl = runCommand($check_cmd);
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that '$check_cmd' ran")
  or die "Couldn't run '$check_cmd'";
$stdout = $momctl{ 'STDOUT' };
foreach my $host (@hosts)
  {

  ok($stdout =~ /${host}:\s+check_poll_time\s=\s\'check_poll_time=${reconfig_check_poll_time}\'/,
     "Verifying that check_poll_time was changed to '$reconfig_check_poll_time' for $host");

  }

# Reconfigure mom to old configuration
%momctl = reconfig($host_list_file, $config_file);
$stdout = $momctl{ 'STDOUT' };
foreach my $host (@hosts)
  {
  ok($stdout =~ /reconfig successful on ${host}/i, "Checking output of 'momctl -f $host_list_file -r $config_file' for $host");
  }

# Check that mom was really reconfigured
%momctl = runCommand($check_cmd);
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that '$check_cmd' ran")
  or die "Couldn't run '$check_cmd'";
$stdout = $momctl{ 'STDOUT' };
foreach my $host (@hosts)
  {

  ok($stdout =~ /${host}:\s+check_poll_time\s=\s\'check_poll_time=${config_check_poll_time}\'/,
    "Verifying that check_poll_time was changed to '$config_check_poll_time' for $host");

  }

###############################################################################
# reconfig($cfg_file) - Run the reconfig command
###############################################################################
sub reconfig #($)
  {
 
  my ($host_list_file, $cfg_file) = @_;
  my %result;
  my $cmd = "momctl -f $host_list_file -r $cfg_file";

  %result = runCommand($cmd);
  ok($result{ 'EXIT_CODE' } == 0,
    "Checking that '$cmd' ran")
   or die "Couldn't run '$cmd'";

  return %result;

  } # END sub reconfig #($)
