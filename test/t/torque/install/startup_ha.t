#! /usr/bin/perl 
#* This test starts both pbs_server and pbs_mom

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;

plan('no_plan'); 
setDesc('Startup Torque in HA mode');

# What is this testing? Are we testing if we can freshly start an instance
# of pbs_mom/server? Or are we making sure one is running? If it is the 
# former, uncomment the die portions of the runCommand('pbs_mom/server') 
# lines below

# Start the pbs mom on all compute nodes
runCommand('pbs_mom');# && die("pbs_mom did not start correctly");
runCommand("ps -ef | grep pbs_mom | grep -v grep") && die("pbs_mom is not running");

# Start the pbs server
runCommand('pbs_server --ha');
runCommand('pbs_server --ha </dev/null >/dev/null 2>/dev/null &');
runCommand("ps -ef | grep pbs_server | grep -v grep") && die("pbs_server is not running");

# Unfortunately),0, it takes 15 seconds to stabilize in its current incantation
ok(sleep 15);
