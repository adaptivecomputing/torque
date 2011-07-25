#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
use Torque::Util qw(
                            list2array
                            node_info
                          );
plan('no_plan');
setDesc('pbs_server -R');

# Variables
my $host        = $props->get_property('Test.Host');
my $port        = '3334';
my @remote_moms = list2array($props->get_property('Torque.Remote.Nodes'));

# Params
my $mom_params = {
                          'args'  => "-R $port",
                          'nodes' => \@remote_moms,
			  'local_node' => 1,
                        };

# Hashes
my %pgrep;
my %pbs_server;
my %node_info;


diag("Restarting pbs_moms to listen on the port '$port'");
stopTorque({ 'remote_moms' => \@remote_moms, 'pbs_mom' => $mom_params });

startPbsmom($mom_params);

# Perform Test
startPbsserver({ 'args' => "-R $port" });
syncServerMom({ 'mom_hosts' => [ $host, @remote_moms ] });

# Test that we can query node state
%node_info = node_info();

cmp_ok($node_info{$_}{state},'ne','down', "Checking communication with '$_' mom") foreach( $host, @remote_moms );

# Restart Torque on default settings
stopTorque({ 'remote_moms' => \@remote_moms });
startTorque({ 'remote_moms' => \@remote_moms });
