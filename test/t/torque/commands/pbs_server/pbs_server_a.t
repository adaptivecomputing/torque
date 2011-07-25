#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

use Torque::Ctrl              qw(
                                   stopPbsserver
                                );
use Torque::Util::Qmgr qw(
                                   list_server_info
                                );


plan('no_plan');
setDesc('pbs_server');

# Variables
my $scheduling_false = 'False';
my $scheduling_true  = 'True';

# Commands
my $pgrep_cmd = 'pgrep -x pbs_server';
my $pbs_server_cmd;

# Hashes
my %pgrep;
my %pbs_server;
my %server_info;

# Arrays
my @true_values  = qw(
                       True
                       true
                       T
                       t
                       1
                     );
my @false_values = qw(
                       False
                       false
                       F
                       f
                       0
                     );

#####################################################################
# Test the false '-a' values
#####################################################################
diag("Test the false '-a' values");
foreach my $false_value (@false_values)
  {

  # Make sure pbs_server is stopped
  stopPbsserver();

  $pbs_server_cmd = "pbs_server -a $false_value";

  # Start pbs_server
  %pbs_server = runCommand($pbs_server_cmd);
  cmp_ok($pbs_server{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$pbs_server_cmd'");

  # Make sure that pbs_server has started
  %pgrep = runCommand($pgrep_cmd);
  cmp_ok($pgrep{ 'EXIT_CODE' }, '==', 0, "Verifying that pbs_server is running");

  # Make sure that scheduling is set to false
  %server_info = list_server_info();
  cmp_ok($server_info{ 'scheduling' }, 'eq', $scheduling_false, "Checking scheduling value for '$scheduling_false'");

  } # END foreach my $false_value (@false_values)

#####################################################################
# Test the true '-a' values
#####################################################################
diag("Test the true '-a' values");
foreach my $true_value (@true_values)
  {

  # Make sure pbs_server is stopped
  stopPbsserver();

  $pbs_server_cmd = "pbs_server -a $true_value";

  # Start pbs_server
  %pbs_server = runCommand($pbs_server_cmd);
  cmp_ok($pbs_server{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$pbs_server_cmd'");

  # Make sure that pbs_server has started
  %pgrep = runCommand($pgrep_cmd);
  cmp_ok($pgrep{ 'EXIT_CODE' }, '==', 0, "Verifying that pbs_server is running");

  # Make sure that scheduling is set to false
  %server_info = list_server_info();
  cmp_ok($server_info{ 'scheduling' }, 'eq', $scheduling_true, "Checking scheduling value for '$scheduling_true'")

  } # END foreach my $true_value (@false_trues)

###############################################################################
# Test defaulting to previous scheduling value when '-a' is not passed
###############################################################################
diag("Test defaulting to previous scheduling value when '-a' is not passed");

# Make sure pbs_server is stopped
stopPbsserver();

$pbs_server_cmd = "pbs_server";

# Start pbs_server
%pbs_server = runCommand($pbs_server_cmd);
ok($pbs_server{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_server_cmd'")
  or diag("EXIT_CODE: $pbs_server{ 'EXIT_CODE' }\nSTDERR: $pbs_server{ 'STDERR' }");

# Make sure that pbs_server has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_server is running");

# Make sure that scheduling is set to false
%server_info = list_server_info();
ok($server_info{ 'scheduling' } eq $scheduling_true, "Checking scheduling value for '$scheduling_true'")
  or diag("Expected: $scheduling_true\nGot: $server_info{ 'scheduling' }");
