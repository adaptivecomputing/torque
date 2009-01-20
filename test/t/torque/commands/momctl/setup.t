#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw( run_and_check_cmd      );

use Torque::Ctrl        qw( startTorque stopTorque );

# Describe Test
plan('no_plan');
setDesc('Momctl Setup');

# Torque Params
my @remote_nodes = $props->get_property( 'torque.remote.nodes' );
my $torque_params    = {
                          'remote_moms' => \@remote_nodes
                       };

###############################################################################
# Stop Torque
###############################################################################
stopTorque($torque_params) 
  or die 'Unable to stop Torque';

###############################################################################
# Configure mom on all nodes
###############################################################################

# Configuration File
my $pbsserver            = $props->get_property('MoabHost'                        );
my $pbsclient            = $props->get_property('MoabHost'                        );
my $restricted           = $props->get_property('MoabHost'                        );
my $logevent             = $props->get_property('mom.config.logevent'             );
my $cputmult             = $props->get_property('mom.config.cputmult'             );
my $usecp                = $props->get_property('mom.config.usecp'                );
my $wallmult             = $props->get_property('mom.config.wallmult'             );
my $configversion        = $props->get_property('mom.config.configversion'        );
my $ideal_load           = $props->get_property('mom.config.ideal_load'           );
my $auto_ideal_load      = $props->get_property('mom.config.auto_ideal_load'      );
my $log_file_max_size    = $props->get_property('mom.config.log_file_max_size'    );
my $log_file_roll_depth  = $props->get_property('mom.config.log_file_roll_depth'  );
my $max_load             = $props->get_property('mom.config.max_load'             );
my $auto_max_load        = $props->get_property('mom.config.auto_max_load'        );
my $node_check_script    = $props->get_property('mom.config.node_check_script'    );
my $node_check_interval  = $props->get_property('mom.config.node_check_interval'  );
my $prologalarm          = $props->get_property('mom.config.prologalarm'          );
my $remote_reconfig      = $props->get_property('mom.config.remote_reconfig'      );
my $timeout              = $props->get_property('mom.config.timeout'              );
my $tmpdir               = $props->get_property('mom.config.tmpdir'               );
my $varattr              = $props->get_property('mom.config.varattr'              );
my $xauthpath            = $props->get_property('mom.config.xauthpath'            );
my $ignwalltime          = $props->get_property('mom.config.ignwalltime'          );
my $mom_host             = $props->get_property('MoabHost'                        );
my $status_update_time   = $props->get_property('mom.config.status_update_time'   );
my $check_poll_time      = $props->get_property('mom.config.check_poll_time'      );
my $job_start_block_time = $props->get_property('mom.config.jobstartblocktime'    );
my $loglevel             = $props->get_property('mom.config.loglevel'             );
my $rcpcmd               = $props->get_property('mom.config.rcpcmd'               );
my $tape8mm              = $props->get_property('mom.config.tape8mm'              );

# The function Data::Properties::get_property() doesn't return a value if it is zero,
# so we have to go through the back door to get 0 values.
my $enable_mom_restart   = $props->{ '_props' }{ 'mom.config.enablemomrestart' };
my $down_on_error        = $props->{ '_props' }{ 'mom.config.down_on_error'    };

my $mom_cfg_file         = $props->get_property('mom.config.file');
my $mom_cfg_file_bak     = $props->get_property('mom.config.file') . ".bak";

my $mom_cfg              =<<CFG;
# Config file for momctl tests
\$pbsserver            $pbsserver
\$pbsclient            $pbsclient

\$logevent             $logevent

\$restricted           $restricted  

\$cputmult             $cputmult
\$usecp                $usecp
\$wallmult             $wallmult
\$configversion        $configversion
\$ideal_load           $ideal_load
\$auto_ideal_load      $auto_ideal_load
\$log_file_max_size    $log_file_max_size
\$log_file_roll_depth  $log_file_roll_depth
\$max_load             $max_load
\$auto_max_load        $auto_max_load
\$node_check_script    $node_check_script
\$node_check_interval  $node_check_interval
\$prologalarm          $prologalarm
\$remote_reconfig      $remote_reconfig
\$timeout              $timeout
\$tmpdir               $tmpdir
\$varattr              $varattr
\$xauthpath            $xauthpath
\$ignwalltime          $ignwalltime
\$mom_host             $mom_host
\$status_update_time   $status_update_time
\$check_poll_time      $check_poll_time
\$jobstartblocktime    $job_start_block_time
\$enablemomrestart     $enable_mom_restart
\$down_on_error        $down_on_error
\$loglevel             $loglevel
\$rcpcmd               $rcpcmd

# Static Values
tape8mm               $tape8mm
CFG

eval
  {

  # Backup the old configuration file
  if (-e $mom_cfg_file)
    {

    `cp -f $mom_cfg_file $mom_cfg_file_bak`

    }

  open(MOMCFG, ">$mom_cfg_file")
   or die "Unable to write to '$mom_cfg_file'";
  print MOMCFG $mom_cfg;
  close(MOMCFG);

  }; # END eval

ok(! $@, "Writing out mom configuration to '$mom_cfg_file'");

# Copy the mom_cfg_file to the remotenodes
foreach my $node (@remote_nodes)
  {

  # Backup the original configuration file
  my $ssh_cp_cmd = "cp -f $mom_cfg_file $mom_cfg_file_bak";
  my %ssh = runCommandSsh($node, $ssh_cp_cmd);
  ok($ssh{ 'EXIT_CODE' } == 0, "Checking exit code of '$ssh_cp_cmd'");

  # Copy the new file
  run_and_check_cmd("scp $mom_cfg_file $node:$mom_cfg_file");


  } # END foreach my $node (@nodes)

###############################################################################
# Create the alternate reconfig file
###############################################################################

# Generate a reconfig file
my $tmp_check_poll_time = $props->get_property('tmp.mom.config.check_poll_time');

my $mom_recfg_file      = $props->get_property('mom.reconfig.file');

my $mom_recfg =<<RECFG;
# Reconfig file for momctl tests
\$pbsserver            $pbsserver
\$pbsclient            $pbsclient

\$logevent             $logevent

\$restricted           $restricted  

\$check_poll_time      $tmp_check_poll_time
RECFG

eval
  {

  open(MOMRECFG, ">$mom_recfg_file")
    or die "Unable to write to '$mom_recfg_file'";
  print MOMRECFG $mom_recfg;
  close(MOMRECFG);

  }; # END eval

ok(! $@, "Writing out mom reconfiguration to '$mom_recfg_file'");

###############################################################################
# Create a hosts list file
###############################################################################
my $host_list      = $props->get_property('MoabHost') 
                     . "," . $props->get_property('torque.remote.nodes');
my $host_list_file = $props->get_property('mom.host.list.file');
eval
  {

  open(HOSTLIST, ">$host_list_file")
    or die "Unable to write to '$host_list_file'";
  print HOSTLIST $host_list;
  close HOSTLIST;

  }; # END eval

ok(! $@, "Writing out host list to '$host_list_file'");

# Create a hosts list file with a single host
$host_list      = $props->get_property('MoabHost');
$host_list_file = $props->get_property('mom.single.host.list.file');
eval
  {

  open(HOSTLIST, ">$host_list_file")
    or die "Unable to write to '$host_list_file'";
  print HOSTLIST $host_list;
  close HOSTLIST;

  }; # END eval

ok(! $@, "Writing out host list with a single host to '$host_list_file'");

###############################################################################
# Start Torque
###############################################################################

# Get Torque variables
# TODO: Need to get torque to stop and start on this port
my $port = $props->get_property('mom.host.port');  

# Restart Torque
startTorque($torque_params) 
  or die 'Unable to start Torque';
