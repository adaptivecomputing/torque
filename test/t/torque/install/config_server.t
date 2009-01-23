#! /usr/bin/perl
#* This test creates the server_priv/nodes file

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;
use CRI::Utils qw(
                   list2array
                 );

plan('no_plan'); 
setDesc('Configure Torque Server');

# Variables
my $nodes_cfg;
my %hostname_cmd;
my $hostname;
my $node_args;
my $nodes_str;
my @nodes;
my $grep_cmd;
my %grep;


# server_priv/nodes file
$nodes_cfg = $props->get_property('Torque.Home.Dir') . "/server_priv/nodes";

# Local TORQUE node
$hostname  = $props->get_property('Test.Host');
$node_args = $props->get_property('torque.node.args');

# Remote TORQUE nodes
$nodes_str = $props->get_property('Torque.Remote.Nodes');
@nodes     = list2array($nodes_str);

# Create/edit the server_priv/nodes file for the server
ok(open(NODES, ">$nodes_cfg"), "Opening Torque server config file '$nodes_cfg'") 
  or die("Couldn't open torque server config file: $!");

# Add the local torque node
print NODES "$hostname $node_args\n";

# Add the remote torque nodes
foreach my $node (@nodes)
  {

  print NODES "$node $node_args\n";

  } # END foreach my $node (@nodes)

ok(close NODES, 'Closing Torque server config file') 
  or die("Couldn't close torque server config file");

# Check the configuration file for the localhost
$grep_cmd = "grep $hostname $nodes_cfg";
%grep     = runCommand($grep_cmd);
ok($grep{ 'EXIT_CODE' } == 0, "Checking exit code of '$grep_cmd'")
  or die("Server config is not correct!");

# Check the configuration file for each node
foreach my $node (@nodes)
  {

  $grep_cmd = "grep $node $nodes_cfg";
  %grep     = runCommand($grep_cmd);
  ok($grep{ 'EXIT_CODE' } == 0, "Checking exit code of '$grep_cmd'")
    or die("Server config is not correct!");

  } # END foreach my $node (@nodes)
