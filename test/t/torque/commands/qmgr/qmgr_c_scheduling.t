#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

use Torque::Util       qw( run_and_check_cmd );
use Torque::Util::Qmgr qw( list_server_info  );

plan('no_plan');
setDesc("qmgr -c 'set server scheduling=true' test");

# Variables
my $cmd;
my %qmgr;
my $scheduling;
my %server_info;

my $server = $props->get_property('Test.Host');

# Perform the test for scheduling=false
$scheduling = 'false';
$cmd        = "qmgr -c 'set server scheduling=$scheduling' $server";
%qmgr       = run_and_check_cmd($cmd);

# Check the server property
%server_info = list_server_info($server);
ok($server_info{ 'scheduling' } =~ /^${scheduling}$/i, "Checking for 'scheduling=$scheduling'");

# Perform the test for scheduling=true
$scheduling = 'true';
$cmd        = "qmgr -c 'set server scheduling=$scheduling' $server";
%qmgr       = run_and_check_cmd($cmd);

# Check the server property
%server_info = list_server_info($server);
ok($server_info{ 'scheduling' } =~ /^${scheduling}$/i, "Checking for 'scheduling=$scheduling'");
