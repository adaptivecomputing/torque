package Torque::Ctrl;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../lib/";


use CRI::Test;

use Torque::Test::Utils qw(
                            is_running
                            is_running_remote
                          );

use base 'Exporter';

our @EXPORT = qw(
                 startTorque
                 stopTorque
                 startPbsmom
                 stopPbsmom
                 startPbsserver
                 stopPbsserver
                 startPbssched
                 stopPbssched
                );


###############################################################################
# startTorque ($)
###############################################################################
sub startTorque #($)# 
  {
  
  my ($cfg) = @_;

  # Config variables
  my $pbs_mom_cfg    = $cfg->{ 'pbs_mom'     } || {};
  my $pbs_server_cfg = $cfg->{ 'pbs_server'  } || {};
  my $remote_moms    = $cfg->{ 'remote_moms' } || [];

  # Return values
  my $local_mom_rtn;
  my $remote_moms_rtn;
  my $pbs_server_rtn;
  my $torque_rtn;

  # pbs_mom parmas
  my $mom_params        = {
                             'args' => $pbs_mom_cfg->{ 'args' }
                          };
  my $remote_mom_params = {
                             'args'  => $pbs_mom_cfg->{ 'args' },
                             'nodes' => $remote_moms
                          };

  diag("Start pbs_mom on local compute node");
  $local_mom_rtn = startPbsmom($mom_params);

  diag("Start pbs_mom on remote compute nodes");
  $remote_moms_rtn = startPbsmom($remote_mom_params);

  diag("Start pbs_server");
  $pbs_server_rtn = startPbsserver($pbs_server_cfg);

  $torque_rtn = (
                     $local_mom_rtn 
                 and $remote_moms_rtn
                 and $pbs_server_rtn
                );

  ok($torque_rtn, "Checking if all torque components started");
  return $torque_rtn;

  } # END sub startTorque #($)# 

###############################################################################
# stopTorque
###############################################################################
sub stopTorque
  {

  my ($cfg) = @_;

  # Config variables
  my $remote_moms = $cfg->{ 'remote_moms' } || [];

  # Return values
  my $local_mom_rtn;
  my $remote_moms_rtn;
  my $pbs_server_rtn;
  my $torque_rtn;

  # pbs_mom parmas
  my $remote_mom_params = {
                             'nodes' => $remote_moms
                          };

  diag("Stop local pbs_mom");
  $local_mom_rtn   = stopPbsmom();

  diag("Stop remote pbs_moms");
  $remote_moms_rtn = stopPbsmom($remote_mom_params);

  diag("Stop pbs_server");
  $pbs_server_rtn  = stopPbsserver();

  $torque_rtn = (
                     $local_mom_rtn 
                 and $remote_moms_rtn
                 and $pbs_server_rtn
                );

  ok($torque_rtn, "Checking if all torque components stopped");
  return $torque_rtn;

} # END sub stopTorque #($)

###############################################################################
# startPbsmom ($)
###############################################################################
sub startPbsmom #($)# 
  {
  
  my ($cfg) = @_;

  # Config variables
  my $args       = $cfg->{ 'args'       } || '';
  my $node       = $cfg->{ 'node'       } || undef;
  my $nodes      = $cfg->{ 'nodes'      } || [];
  my $local_node = $cfg->{ 'local_node' } || 0;

  push(@$nodes, $node) 
    if $node;

  # Assume that we want to start the local node if no remote nodes are passed
  $local_node = 1
    if scalar @$nodes == 0;

  # Return values
  my $return           = 1;

  # Mom executables
  my $torque_sbin = $props->get_property( 'Torque.Home.Dir' ) . "/sbin/" || '';

  # Set up the commands
  my $pbs_mom_cmd  = "${torque_sbin}pbs_mom ";
  $pbs_mom_cmd    .= $args;

  # Decide if the mom is on a remote host
  if (scalar @$nodes)
    {

    # Make sure pbs_mom is stopped
    stopPbsmom({ 'nodes'      => $nodes,
                 'local_node' => $local_node });

    foreach my $n (@$nodes)
      {

      my $ssh_cmd = "$pbs_mom_cmd";
      my %ssh     = runCommandSsh($n, $ssh_cmd);

      if (! ok(is_running_remote('pbs_mom', $n), "Checking if pbs_mom was successfully started on '$n'"))
        {

        diag("pbs_mom STDERR: $ssh{ 'STDERR' }");
        $return = 0;

        } # END if (! ok(is_running_remote('pbs_mom', $n), "Checking if pbs_mom was successfully started on '$n'"))

      } # END foreach my $n (@nodes)

    } # END if (scalar @$nodes)
  if ($local_node)
    {

    # Make sure the pbs_mom is stopped
    stopPbsmom({ 'local_node' => $local_node });

    # Start the pbs mom on local compute nodes
    my %pbs_mom = runCommand($pbs_mom_cmd);
    if(! ok(is_running('pbs_mom'), "Checking if pbs_mom was successfully started on local node"))
      {

      diag("pbs_mom STDERR: $pbs_mom{ 'STDERR' }");
      $return = 0;

      } # END if(! ok(is_running('pbs_mom'), "Checking if pbs_mom was successfully started on local node"))

    } # END else

  return $return;

  } # END sub startPbsmom #($)#

###############################################################################
# stopPbsmom
###############################################################################
sub stopPbsmom
  {

  my ($cfg) = @_;

  # Parameters
  my $node       = $cfg->{ 'node'       } || undef;
  my $nodes      = $cfg->{ 'nodes'      } || [];
  my $local_node = $cfg->{ 'local_node' } || 0;
  
  my $return = 1;

  push(@$nodes, $node) 
    if $node;

  # Assume that we want to start the local node if no remote nodes are passed
  $local_node = 1
    if scalar @$nodes == 0;

  # Mom executables
  my $torque_sbin = $props->get_property( 'Torque.Home.Dir' ) . "sbin/" || '';

  # Decide if the mom is on a remote host
  if (scalar @$nodes)
    {

    foreach my $n (@$nodes)
      {

      # Stop the pbs mom on the remote node
      if (is_running_remote('pbs_mom', $n))
        { 

        my $momctl_cmd = "${torque_sbin}momctl -s";
    	  my %momctl     = runCommandSsh($n, $momctl_cmd);
    	  sleep 2;

        if (is_running_remote('pbs_mom', $n))
          {

          my $kill_cmd = "pkill -9 -x pbs_mom";
    	    my %kill     = runCommandSsh($n, $kill_cmd);
    	    sleep 2;

          } # END if (is_running_remote('pbs_mom', $n))

        # Fail if pbs is still running
        ok(! is_running_remote('pbs_mom', $n), "Checking if pbs_mom is stopped on '$n'")
          or $return = 0;

        } # END if (! runCommand("pgrep -x pbs_mom"))

      } # END foreach my $n (@nodes)

    } # END if (scalar @$node)
  if ($local_node)
    {

    # Stop the pbs mom on the local node
    if (is_running('pbs_mom'))
      { 

      my $momctl_cmd = 'momctl -s';
    	my %momctl     = runCommand($momctl_cmd);
    	sleep 2;

      if (is_running('pbs_mom'))
        {

        my $kill_cmd = "pkill -9 -x pbs_mom";
      	my %kill     = runCommand($kill_cmd);
      	sleep 2;

        } # END if (is_running('pbs_mom'))


      ok(! is_running('pbs_mom'), "Checking if pbs_mom is stopped on the local_node")
         or $return = 0;

      } # END if (! runCommand("pgrep -x pbs_mom"))

    } # END else

  return $return;

} # END sub stopPbsmom

###############################################################################
# startPbsserver 
##############################################################################
sub startPbsserver #($)
  {

  my ($cfg) = @_;

  my $args  = $cfg->{ 'args' } || '';

  # pbs_server command
  my $pbs_server_cmd  = 'pbs_server ';
  $pbs_server_cmd    .= $args;

  # Stop the pbs_server
  stopPbsserver();

  # Start the pbs server
  runCommand($pbs_server_cmd);
  ok(is_running('pbs_server'), "Checking if the pbs_server was started")
    or return 0;

  # Unfortunately, it takes 15 seconds to stabilize in its current incantation
  diag("Waiting for pbs_server to stabilize...");
  sleep 15;

  return 1;

  } # END startPbsserver

###############################################################################
# stopPbsserver
###############################################################################
sub stopPbsserver 
  {

  # runCommand hashes
  my %qterm;
  my %kill;

  # Return value
  my $return = 1;

  # Commands  
  my $qterm_cmd   = "qterm -t quick";
  my $kill_cmd    = "pkill -9 -x pbs_server";

  if (is_running('pbs_server')) 
    { 

	  %qterm = runCommand($qterm_cmd); 
	  sleep 2;

    if (is_running('pbs_server')) 
      {

	    %kill = runCommand($kill_cmd);
	    sleep 4;

      } # END if (is_running('pbs_server')) 

	  # Use or because we want pgrep to fail (return 1)

    ok(! is_running('pbs_server'), "Checking if the pbs_server is stopped")
       or $return = 0;

    }

  return $return;

  } # END sub stopPbsserver

###############################################################################
# startPbssched ($)
###############################################################################
sub startPbssched #($)# 
  {
  
  my ($cfg) = @_;

  # Return value
  my $return = 1;

  # Set up the command
  my $pbs_sched_cmd  = 'pbs_sched ';
  $pbs_sched_cmd    .= $cfg->{ 'args' }
    if defined $cfg->{ 'args' };

  # Start the pbs mom on all compute nodes
  runCommand($pbs_sched_cmd);

  ok(is_running('pbs_sched'), 'Checking if the pbs_sched is running')
    or $return = 0;

  pass("pbs_sched started");
  return $return;

  } # END sub startPbsmom #($)#

###############################################################################
# stopPbssched
###############################################################################
sub stopPbssched
  {

    if (is_running('pbs_sched'))
      { 

    	runCommand("pkill -9 -x pbs_sched");
    	sleep 2;

	    ok(! is_running('pbs_sched'), "Checking that pbs_sched is not running") 
        or return 0; 

      }

    pass('pbs_sched stopped');
    return 1;

  } # END sub stopPbssched

1;

=head1 NAME

Torque::Ctrl - A module to control torque

=head1 SYNOPSIS

 use Torque::Ctrl qw(
                      startTorque
                      stopTorque
                      startPbsmom
                      stopPbsmom
                      startPbsserver
                      stopPbsserver
                      startPbssched
                      stopPbsschec
                    );

 # Start Torque
 my $torque_cfg = {
                   'pbs_mom'    =>  {
                                     'args' => "-M 15003"
                                    },
                   'pbs_server' =>  {
                                     'args' => "-M 15003",
                                    },
                   'remote_moms' => \@remote_nodes,
                 };

 my $torque_started = startTorque($start_torque_cfg);

 # Stop Torque
 my $torque_stopped = stopTorque();

 # Start pbs_mom
 my $mom_cfg = {
                'args'  => '-h host1',
                'node'  => 'host1',
                'nodes' => \@remote_nodes
               };

 my $mom_started = startPbsmom($mom_cfg); 

 # Stop pbs_mom
 my $mom_cfg = {
                'node'  => 'host1',
                'nodes' => \@remote_nodes
               };

 my $mom_stop = stopPbsmom($mom_cfg); 

 # Start pbs_server
 my $server_cfg = {
                    'args' => '-h host1'
                  };

 my $server_started = startPbsserver($server_cfg);
 
 # Stop the pbs_server
 my $server_stopped = stopPbsserver();

 # Start the pbs_sched
 my $sched_cfg = {
                   'args' => '-S 15009'
                 };

 my $sched_started = startPbssched($sched_cfg);
 
 # Stop pbs_sched
 my $sched_stopped = stopPbssched($sched_cfg);

=head1 DESCRIPTION

This module provides methods to control the basic functionality of torque.

=head1 SUBROUTINES/METHODS

=over 4

=item startTorque($start_torque_config)

(Re)Start both the pbs_mom and pbs_server.  The arguments for the commands 'pbs_mom' and pbs_server' can be passed through a hash.  The hash is in the following format:

 my $torque_cfg = {
                   'pbs_mom'    =>  {
                                     'args' => "-M 15003"
                                    },
                   'pbs_server' =>  {
                                     'args' => "-M 15003",
                                    },
                   'remote_moms' => \@remote_nodes,
                 };

=item stopTorque

stopTorque simply stops both the 'pbs_server' and the 'pbs_mom' daemons.

=item startPbsmom($)

(Re)Start the pbsmom server for given node(s) if specified.  Can pass it the 'arg' hash which is argument that will be appended onto the pbs_mom command.  The hash has the following possible arguments:

 my $mom_cfg = {
                'args'  => '-h host1',
                'node'  => 'host1',
                'nodes' => \@remote_nodes
               };

=item stopPbsmom($)

Stops the pbsmom server for given node(s) if specified.  The following are possible arguments:

 my $mom_cfg = {
                'node'  => 'host1',
                'nodes' => \@remote_nodes
               };
 

=item startPbsserver($)

(Re)Starts the pbs_server on the local machine.  Can have the following configuration hashref passed to it:

 my $server_cfg = {
                    'args' => '-h host1'
                  };


=item stopPbsserver

Stops the pbs_server on the local machine

=item startPbssched($)

(Re)Starts pbs_sched on the local machine. Can have teh following configuration hashref passed to it:

 my $sched_cfg = {
                    'args' => '-S 15009'
                 };

=item stopPbsSched

Stops pbs_sched on the local machine.


=back

=head1 DEPENDENDCIES

Moab::Test, Torque::Test::Utils

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
