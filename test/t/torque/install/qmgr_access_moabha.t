#! /usr/bin/perl 
#* This tests that permissions are set on torque to allow a Moab fallback server to access job lists

use CRI::Test;
plan('no_plan'); 
setDesc('Set Torque Management Permissions');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


my $primary = $props->get_property('MoabHost');
my $fallback = $props->get_property('fallback.hostname');

# Set permissions of Torque server to allow remote machine access (by Moab fallback server)
runCommand("qmgr -c 's s acl_hosts += $primary'");
runCommand("qmgr -c 's s acl_hosts += $fallback'");
runCommand("qmgr -c 's s managers+=root\@$primary'");
runCommand("qmgr -c 's s managers+=root\@$fallback'");
runCommand("qmgr -c 's s operators+=root\@$primary'");
runCommand("qmgr -c 's s operators+=root\@$fallback'");
runCommand("qmgr -c 's s submit_hosts+=$primary'");
runCommand("qmgr -c 's s submit_hosts+=$fallback'");

# Check if managers, operators, and submit_hosts were set successfully on Torque server
my %qmgrCommand = runCommand("qmgr -c 'p s'");
my $access = $qmgrCommand{'STDOUT'};

my $manager_pri 	=  1 	if $access =~ /set server managers\s+\+?=\s+root\@$primary/;
my $manager_fal 	=  1 	if $access =~ /set server managers\s+\+?=\s+root\@$fallback/;
my $operator_pri 	=  1 	if $access =~ /set server operators\s+\+?=\s+root\@$primary/;
my $operator_fal 	=  1 	if $access =~ /set server operators\s+\+?=\s+root\@$fallback/;
my $acl_host_pri 	=  1 	if $access =~ /set server acl_hosts\s+\+?=\s+$primary/;
my $acl_host_fal 	=  1 	if $access =~ /set server acl_hosts\s+\+?=\s+$fallback/;
my $sub_host_pri 	=  1 	if $access =~ /set server submit_hosts\s+\+?=\s+$primary/;
my $sub_host_fal 	=  1 	if $access =~ /set server submit_hosts\s+\+?=\s+$fallback/;

ok(defined $manager_pri && defined $manager_fal, 'Checking manager permissions...')
   or die 'Manager permissions were not set properly';
ok(defined $operator_pri && defined $operator_fal, 'Checking operator permissions...')
   or die 'Operator permissions were not set properly';
ok(defined $acl_host_pri && defined $acl_host_fal, 'Checking allowed allowed-access hosts...')
   or die 'Acl_hosts was not set properly';
ok(defined $sub_host_pri && defined $sub_host_fal, 'Checking allowed submission hosts...')
   or die 'Acl_hosts was not set properly';
