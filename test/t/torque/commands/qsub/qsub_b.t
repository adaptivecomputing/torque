#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


use CRI::Test;

use Torque::Ctrl qw(
                    startPbsserver
                    stopPbsserver
                   );

plan('no_plan');
setDesc('qsub -b');

# Stop the pbs_server
diag("Stopping pbs_server...");
stopPbsserver();

# Perform the test
my $sec  = 3;

my $user = $props->get_property('moab.user.one');
my $cmd  = "echo sleep 30 | qsub -b $sec";
my %qsub = runCommandAs($user, $cmd);

my $retry_msg = "Retrying for $sec seconds";
my $stderr    = $qsub{ 'STDERR' };
ok($stderr =~ /${retry_msg}/, "Checking STDERR for '$retry_msg'")
  or diag("STDERR: $stderr");

# Restart the pbs_server
diag("Starting pbs_server...");
startPbsserver();
