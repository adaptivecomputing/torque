#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More tests => 7;

# pbs_server - TORQUE Install
my $pbs_server = `which pbs_server` || undef;
ok(defined $pbs_server, 'TORQUE Install') or
  BAIL_OUT('Cannot locate TORQUE server daemon - install TORQUE or add TORQUE install directory to $PATH');

# qstat - Client Tools
my $qstat = `which qstat` || undef;
ok(defined $qstat, 'Client Tools') or
  BAIL_OUT('Cannot locate TORQUE clients, install TORQUE or add TORQUE install directory to $PATH');

# version - TORQUE version
my $version = `qstat --version 2>&1`;
   $version =~ /^version:\s*([\d.]+)\s*$/i;
   $version = $1 || undef;
ok(defined $version, 'TORQUE Version') or
  BAIL_OUT('Cannot determine TORQUE version');

# sbin/ - Install Location
my $sbindir = $pbs_server;
   $sbindir =~ s/pbs_server\s*$//;
ok($sbindir ne '', 'Determine sbin') or
  BAIL_OUT('Cannot determine SBIN directory');
ok(-d $sbindir,'sbin Exists') or
  BAIL_OUT("$sbindir does not seem to exist");

# PID - TORQUE Currently Running
my $pid = `ps -ef | grep pbs_server | grep -v grep`;
   $pid =~ /^\S+\s+(\d+)\s+/;
   $pid = $1 || undef;
ok(defined $pid, 'TORQUE Currently Running') or
  BAIL_OUT('pbs_server not running, start pbs_server and rerun test - see Section 1.2.4');

# qmgr - TORQUE Database Created
my $qmgr = `qmgr -c 'p s'` || undef;
ok(defined $qmgr, 'Database Created') or
  BAIL_OUT("TORQUE database not created, restart pbs_server using 'pbs_server -t create' - see Section 1.2.1");

