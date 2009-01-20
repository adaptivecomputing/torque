#! /usr/bin/perl 
#  This test stops both pbs_server and pbs_mom

use CRI::Test;
plan('no_plan');
setDesc('Shutdown Torque');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Stop the pbs server
unless(runCommand("pgrep -x pbs_server")) { 
    runCommand("qterm -t quick"); 
    sleep 2;
    runCommand("killall -s KILL pbs_server");
    sleep 2;

    ok(sleep 2,'Waiting for torque to complete shutdown');

    runCommand("pgrep -x pbs_server") or  die("pbs_server is not shutdown"); # Use or because we want pgrep to fail (return 1)
}
pass("No pbs servers running");

# Stop the pbs mom on all compute nodes
unless(runCommand("pgrep -x pbs_mom")){ 
    runCommand("momctl -s");
    sleep 2;
    runCommand("killall -s KILL pbs_mom");
    sleep 2;

    runCommand("pgrep -x pbs_mom") or die("pbs_mom is not shutdown");
}
pass("No pbs moms  running");
