#! /usr/bin/perl 
#* This test creates the mom_priv/config file on each client node

use CRI::Test;
plan('no_plan');
setDesc('Configure Torque on the Compute Nodes');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Create/edit the mom_priv/config file on each node
my $configFile = $props->get_property('torque.home.dir') . "/mom_priv/config";

my %hostnameCommand = runCommand("hostname --ip-address","Getting hostname IP address");
my $ipAddress = $hostnameCommand{'STDOUT'};
chomp($ipAddress);

# BLCR Variables
my $checkpoint_script   = $props->get_property('torque.checkpoint_script'  );
my $restart_script      = $props->get_property('torque.restart_script'     );
my $checkpoint_run_exe  = $props->get_property('torque.checkpoint_run_exe' );
my $checkpoint_interval = $props->get_property('torque.checkpoint_interval');
my $loglevel            = $props->get_property('mom.config.loglevel'       );

ok(open(CONFIG, ">$configFile"),"Opening Torque mom config file") or die("Couldn't open torque mom config file");
print CONFIG << "EOF";
\$logevent       255
\$loglevel       $loglevel
\$restricted     $ipAddress  # note: IP address of host running pbs_server

\$checkpoint_script   $checkpoint_script
\$restart_script      $restart_script
\$checkpoint_run_exe  $checkpoint_run_exe
\$checkpoint_interval $checkpoint_interval
EOF

ok('close CONFIG','Closing Torque mom config file') or die("Couldn't close Torque mom config file!");
ok('chmod 0644, $configFile','Setting Torque mom config file permissions') or die("Torque file permissions couldn't be set");
runCommand("grep $ipAddress ". $props->get_property('torque.home.dir') ."/mom_priv/config") && die("Mom config was not properly set");
