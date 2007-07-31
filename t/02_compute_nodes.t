#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More tests => 8;

# pbs_server --about - Directory Information
my @pbs_server = `pbs_server --about`;
ok(scalar @pbs_server, 'Directory Information') or
  BAIL_OUT('Cannot get directory infromation from pbs_server');

# TORQUE Home Directory
my $torquehome = undef;
foreach my $line (@pbs_server)
  {
  if ($line =~ /^serverhome:\s+(.+)\s*$/i)
    {
    $torquehome = $1;
    last;
    }
  }
ok(defined $torquehome, 'TORQUE Home Directory') or
  BAIL_OUT('Cannot determine server home directory from pbs_server');
ok(-d $torquehome, 'TORQUE Home Directory Exists') or
  BAIL_OUT("The server home directory ($torquehome) does not exist");

# pbsnodes - Node List
my $nodesfile = "$torquehome/server_priv/nodes";
   $nodesfile =~ s#//#/#g;
ok(-T $nodesfile, 'Nodes File Exists') or
  BAIL_OUT("TORQUE 'nodes' file not created, setup $nodesfile and restart pbs_server - see Section 1.2.2");
my @pbsnodes = map  { s/\s+$//g; $_ }
               grep { /^\S/         }
               `pbsnodes -a`;
ok(scalar @pbsnodes, 'pbsnodes Reports Nodes') or
  BAIL_OUT("TORQUE 'nodes' file not created, setup $nodesfile and restart pbs_server - see Section 1.2.2");
ok($pbsnodes[0] ne 'no nodes', 'pbsnodes Lists Nodes') or
  BAIL_OUT("TORQUE 'nodes' file not created, setup $nodesfile and restart pbs_server - see Section 1.2.2");

# Node Count
my $nodecount = scalar @pbsnodes;
ok($nodecount, 'Node Count') or
  BAIL_OUT('TORQUE reporting zero nodes available');

# Active Nodes
my @upnodes = grep { /down|unknown/i }
              grep { /^\s+state\s+=\s+/i }
              `pbsnodes -a`;
ok(!scalar(@upnodes), 'Nodes Have Connected to Server') or
  do {
  diag
    (
    "Compute nodes have not contacted pbs_server, verify the following:\n"
    . "- pbs_mom daemons are running on hosts\n"
    . "- compute hostnames match names listed in $torquehome/server_priv/nodes\n"
    . "- compute hosts are network-accessible from the head node\n"
    . "- the command 'momctl -d3 -h $pbsnodes[0]' does not report any errors\n"
    );
  BAIL_OUT('Compute nodes have not contacted pbs_server');
  }

