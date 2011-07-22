#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();
 
#* This test creates the mom_priv/config file on each client node

use CRI::Test;
plan('no_plan');
setDesc('Configure Torque on the Compute Nodes');

my $host = $props->get_property('Test.Host');

# Create/edit the mom_priv/config file on each node
my $configFile = $props->get_property('Torque.Home.Dir') . "/mom_priv/config";

ok(open(CONFIG, ">$configFile"),"Opening Torque mom config file")
  or die("Couldn't open torque mom config file");

print CONFIG << "EOF";
\$pbsserver     $host  # note: hostname running pbs_server
\$logevent       255
EOF

ok(close CONFIG, 'Closing Torque mom config file')
  or die("Couldn't close Torque mom config file!");
ok(chmod(0644, $configFile), 'Setting Torque mom config file permissions')
  or die("Torque file permissions couldn't be set");
