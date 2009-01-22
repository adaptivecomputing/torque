#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw(
                            list2array
                            node_info
                          );
use Torque::Ctrl        qw(
                            startPbsmom
                            stopPbsserver
                            startPbsserver
                          );

plan('no_plan');
setDesc('pbs_server -R');

# Variables
my $host        = $props->get_property('Test.Host');
my $port        = '3334';
my @remote_moms = list2array($props->get_property('Torque.Remote.Nodes'));

# Params
my $remote_mom_params = {
                          'args'  => "-R $port",
                          'nodes' => \@remote_moms
                        };
my $local_mom_params  = {
                          'args'  => "-R $port"
                        };

# Commands
my $pgrep_cmd      = 'pgrep -x pbs_server';
my $pbs_server_cmd = "pbs_server -R $port";

# Hashes
my %pgrep;
my %pbs_server;
my %node_info;


###############################################################################
# Start the pbs_mom
###############################################################################
diag("Restarting pbs_moms to listen on the port '$port'");
startPbsmom($local_mom_params);
startPbsmom($remote_mom_params);

###############################################################################
# Perform the test
###############################################################################
diag("Test '$pbs_server_cmd'");

# Stop the pbs_server
stopPbsserver();

# Start pbs_server
%pbs_server     = runCommand($pbs_server_cmd);
ok($pbs_server{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_server_cmd'")
  or diag("EXIT_CODE: $pbs_server{ 'EXIT_CODE' }\nSTDERR: $pbs_server{ 'STDERR' }");

diag("Waiting for pbs_server to stabilize...");
sleep 15;

# Make sure that pbs_server has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_server is running");

# Check for any error messages
ok($pbs_server{ 'STDERR' } eq '', "Checking for an error message")
  or diag("STDERR: '$pbs_server{ 'STDERR' }'");

# Test that we can query node state
%node_info = node_info();
ok((exists $node_info{ $host }{ 'state' } 
    and    $node_info{ $host }{ 'state' } ne 'down'), "Checking communication with '$host' mom")
  or diag("Node state should not be: 'down'\nNode state: '$node_info{ $host }{ 'state' }'");

foreach my $remote_mom (@remote_moms)
  {

  ok((exists $node_info{ $remote_mom }{ 'state' } 
      and    $node_info{ $remote_mom }{ 'state' } ne 'down'), "Checking communication with '$remote_mom' mom")
    or diag("Node state should not be: 'down'\nNode state: '$node_info{ $remote_mom }{ 'state' }'");

  } # END foreach my $remote_node (@remote_nodes)

###############################################################################
# Restart the pbs_mom
###############################################################################
diag("Restarting pbs_moms to listen on the default port");
delete $local_mom_params->{ 'args' };
delete $remote_mom_params->{ 'args' };
startPbsmom($local_mom_params);
startPbsmom($remote_mom_params);

###############################################################################
# Restart pbs_server to it's default state
###############################################################################
diag("Restarting pbs_server");  
startPbsserver();
