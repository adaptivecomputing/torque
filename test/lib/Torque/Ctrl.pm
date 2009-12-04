package Torque::Ctrl;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../lib/";


use CRI::Test;
use Expect;
use Carp;

use Torque::Util qw(
                            is_running
                            is_running_remote
                          );
use CRI::Utils          qw(
                            run_and_check_cmd
                          );

use base 'Exporter';

our @EXPORT = qw(
                 startTorque
                 startTorqueClean
                 stopTorque
                 startPbsmom
                 stopPbsmom
                 startPbsserver
                 startPbsserverClean
                 stopPbsserver
                 startPbssched
                 stopPbssched
                 createMomCfg
                 restoreMomCfg
		 createPbsserverNodes
                );

# Global Variables
our $torque_home      = $props->get_property('Torque.Home.Dir');
our $torque_sbin      = "$torque_home/sbin/";
our $mom_lock_file    = "$torque_home/mom_priv/mom.lock";
our $server_lock_file = "$torque_home/server_priv/server.lock";

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

  # pbs_mom params
  my $mom_params        = {
                             'args' => $pbs_mom_cfg->{ 'args' }
                          };
  my $remote_mom_params = {
                             'args'  => $pbs_mom_cfg->{ 'args' },
                             'nodes' => $remote_moms
                          };

  diag("Start pbs_mom on local compute node");
  $local_mom_rtn = startPbsmom($mom_params);

  if( scalar @$remote_moms )
  {
      diag("Start pbs_mom on remote compute nodes");
      $remote_moms_rtn = startPbsmom($remote_mom_params);
  }
  else
  {
      $remote_moms_rtn = 1;
  }

  $pbs_server_rtn =   !exists $cfg->{clean_start}
		    ? startPbsserver($pbs_server_cfg)
		    : startPbsserverClean($pbs_server_cfg);

  $torque_rtn = (
                     $local_mom_rtn 
                 and $remote_moms_rtn
                 and $pbs_server_rtn
                );

  ok($torque_rtn, "All Torque Components Started");
  return $torque_rtn;

  } # END sub startTorque #($)# 

###############################################################################
# startTorqueClean ($)
###############################################################################
sub startTorqueClean
{ 
  my ($cfg) = @_;

  $cfg->{clean_start} = 1;

  return startTorque($cfg);
}

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
    my $remote_mom_params = { 'nodes' => $remote_moms };

    diag("Stop local pbs_mom");
    $local_mom_rtn   = stopPbsmom();

    if( scalar @$remote_moms )
    {
	diag("Stop remote pbs_moms");
	$remote_moms_rtn = stopPbsmom($remote_mom_params);
    }
    else
    {
	$remote_moms_rtn = 1;
    }

    diag("Stop pbs_server");
    $pbs_server_rtn  = stopPbsserver();

    $torque_rtn = (
	$local_mom_rtn 
	    and $remote_moms_rtn
	    and $pbs_server_rtn
    );

    return ok($torque_rtn, "All Torque Components Stopped");
}

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

    # Set up the commands
    my $pbs_mom_cmd  = "${torque_sbin}pbs_mom ";
    $pbs_mom_cmd    .= $args;

    # Start any Remote mom's
    if (scalar @$nodes)
    {
	# Make sure pbs_mom is stopped
	stopPbsmom({
	    'nodes'      => $nodes,
	    'local_node' => $local_node
	});

	foreach my $n (@$nodes)
	{
	    my %ssh     = runCommandSsh($n, $pbs_mom_cmd, 'test_success_die' => 1);

	    if (! ok(is_running_remote('pbs_mom', $n), "Checking if pbs_mom was successfully started on '$n'"))
	    {
		diag("pbs_mom STDERR: $ssh{ 'STDERR' }");
		$return = 0;

	    } # END if (! ok(is_running_remote('pbs_mom', $n), "Checking if pbs_mom was successfully started on '$n'"))

	} # END foreach my $n (@nodes)

    } # END if (scalar @$nodes)
    
    # Start local mom
    if ($local_node)
    {
	my %pbs_mom = runCommand($pbs_mom_cmd);
	
	if( $pbs_mom{EXIT_CODE} != 0 && $pbs_mom{STDERR} =~ /another mom running/ )
	{
	    pass "A Local pbs_mom is already Running!";
	}
	else
	{
	    my $check_cmd = -e $mom_lock_file
	                  ? "ps aux | grep \`cat $mom_lock_file\` | grep -v grep"
	                  : "ps aux | grep pbs_server | grep -v grep";

	    my $wait = 30;
	    diag "Waiting at most $wait seconds for Local pbs_mom to Start";

	    do
	    {
		sleep 1;
		$wait--;
	    }
	    while( $wait && `$check_cmd` !~ /pbs_mom/);

	    if( $wait != 0 )
	    {
		pass "Local pbs_server is now Running!";
	    }
	    else
	    {
		fail "Local pbs_server Failed to Start";
		$return = 0;
	    }
	}
    }

    return $return;
}

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

    push(@$nodes, $node) if $node;

    $local_node = 1 if scalar @$nodes == 0;

    my $momctl_cmd = "${torque_sbin}momctl -s";

    # Stop any remote pbs_mom's
    if (scalar @$nodes)
    {
	foreach my $n (@$nodes)
	{
	    if (is_running_remote('pbs_mom', $n))
	    { 
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

	    }
	}
    }
    
    # Stop local pbs_mom
    if ($local_node)
    {
	my $ps_info = `ps aux | grep \`cat $mom_lock_file\` | grep -v grep`;

	if( $ps_info =~ /pbs_mom/ )
	{ 
	    my %momctl = runCommand($momctl_cmd);

	    if( $momctl{EXIT_CODE} != 0 )
	    {
		my $kill = "kill -9 `cat $mom_lock_file`";

		diag "Normal Shutdown Failed! Attempting to SIGKILL pbs_mom";
		system $kill;
	    }

	    my $wait = 30;
	    diag "Waiting at most $wait seconds for Local pbs_mom to Stop";

	    do
	    {
		sleep 1;
		$wait--;
	    }
	    while( $wait && `ps aux | grep \`cat $mom_lock_file\` | grep -v grep` =~ /pbs_mom/);

	    if( $wait == 0 )
	    {
		fail "Unable to Stop Local pbs_mom!";
		return 0;
	    }
	}
	
	pass "Local pbs_mom is now Stopped";
    }

    return $return;
}

###############################################################################
# startPbsserver 
##############################################################################
sub startPbsserver #($)
{
    my ($cfg) = @_;

    my $args  = $cfg->{ 'args' } || undef;

    # pbs_server command
    my $pbs_server_cmd  = "${torque_sbin}pbs_server";
    $pbs_server_cmd    .= " $args" if defined $args;

    diag 'Attempting to Start PBS_Server';

    # Start the pbs server
    my %server = runCommand($pbs_server_cmd);

    if( $server{EXIT_CODE} != 0 && $server{STDERR} =~ /another server running/ )
    {
	pass "A Local pbs_server is already Running!";
    }
    else
    {
	my $check_cmd = "ps aux | grep pbs_server | grep -v grep";

	my $wait = 30;
	diag "Waiting at most $wait seconds for Local pbs_server to Start";

	do
	{
	    sleep 1;
	    $wait--;
	}
	while( $wait && `$check_cmd` !~ /pbs_server/);
	
	if( $wait != 0 )
	{
	    pass "Local pbs_server is now Running!";
	}
	else
	{
	    fail "Local pbs_server Failed to Start";
	    return 0;
	}
    }

    # Unfortunately, it takes 15 seconds to stabilize in its current incantation
    #my $wait = 10;
    #diag("Waiting $wait seconds for pbs_server to stabilize...");
    #sleep $wait;

    return 1;
}

##############################################################################
# startPbsserverClean
##############################################################################
sub startPbsserverClean #($)
{
  my ($cfg) = @_;

  # Variables
  my $qmgr_cmd          = "$torque_home/bin/qmgr";
  my $pbs_server_cmd    = "${torque_sbin}pbs_server";
  my $operator          = 'root';
  my $manager           = 'root';
  my $hostname          = $props->get_property('Test.Host');
  
  my $default_setup_str =<<DEFAULT;
echo set server operators += $operator\@$hostname | $qmgr_cmd
echo set server managers += $manager\@$hostname | $qmgr_cmd
$qmgr_cmd -c 'set server scheduling = true'
$qmgr_cmd -c 'set server keep_completed = 300'
$qmgr_cmd -c 'set server mom_job_sync = true'
$qmgr_cmd -c 'create queue batch'
$qmgr_cmd -c 'set queue batch queue_type = execution'
$qmgr_cmd -c 'set queue batch started = true'
$qmgr_cmd -c 'set queue batch enabled = true'
$qmgr_cmd -c 'set queue batch resources_default.walltime = 1:00:00'
$qmgr_cmd -c 'set queue batch resources_default.nodes = 1'
$qmgr_cmd -c 'set server default_queue = batch'
DEFAULT

  # Configuration Variables
  my %server_nodes = ();
  $server_nodes{hosts} = $cfg->{hosts} if exists $cfg->{hosts};
  my $setup_str        = $cfg->{ 'setup_str' } || $default_setup_str;
  my @setup_lines      = split(/\n/, $setup_str);

  # pbs_server command
  my $pbs_cmd  = "$pbs_server_cmd -t create";
  
  # Start the pbs server
  diag 'Attempting to Start PBS_Server Clean';
  my $exp     = Expect->spawn($pbs_cmd)
    or die "Cannot spawn '$pbs_cmd'";

  $exp->expect(5,
               [
                 qr/do you wish to continue/ => sub {
                            
                                                      my ($exp) = @_;
                                                      $exp->send("y\n");
                                                      exp_continue();
                           
                                                    } # END sub
               ],
	       [ qr/another server running/ => sub { die "Another pbs_server is Still Running! Stop that pbs_server First"; }
	       ],
              );

  my $check_cmd = "ps aux | grep pbs_server | grep -v grep";

  my $wait = 30;
  diag "Waiting at most $wait seconds for Local pbs_server to Start";

  do
  {
      sleep 1;
      $wait--;
      logMsg "$check_cmd:\n".`$check_cmd`;
  }
  while( $wait && `$check_cmd` !~ /pbs_server/);

  if( $wait != 0 )
  {
      pass "Local pbs_server is now Running!";
  }
  else
  {
      fail "Local pbs_server Failed to Start";
      return 0;
  }

  # Resetup Moab
  diag("Re-Configure Torque");
  runCommand($_, 'test_success_die' => 1) foreach @setup_lines;

  sleep_diag 3;

  stopPbsserver();
  createPbsserverNodes(\%server_nodes);
  startPbsserver();

  return 1;
}

###############################################################################
# stopPbsserver
###############################################################################
sub stopPbsserver 
{
    my $return = 1;

    # Commands  
    my $qterm_cmd = "qterm -t quick";
    my $kill      = 'kill -9 `pgrep -x pbs_server`';
    my $check_cmd = "ps aux | grep pbs_server | grep -v grep";

    my $ps_info = `$check_cmd`;

    if( $ps_info =~ /pbs_server/ )
    { 
	my %qterm = runCommand($qterm_cmd);

	if( $qterm{EXIT_CODE} != 0 )
	{
	    diag "Normal Shutdown Failed! Attempting to SIGKILL pbs_server";
	    system $kill;
	}

	my $wait = 30;
	diag "Waiting at most $wait seconds for pbs_server to Stop";
	logMsg "Using Command '$check_cmd' to Check";

	do
	{
	    sleep 1;
	    $wait--;
	}
	while( $wait && `$check_cmd` =~ /pbs_server/);

	if( $wait == 0 )
	{
	    fail "Unable to Stop Local pbs_server!";
	    return 0;
	}
    }

    pass "Local pbs_server is now Stopped";

    return $return;
}

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

#------------------------------------------------------------------------------
# createMomCfg();
# createMomCfg(
#               'clienthost'  => 'host1',
#               'logevent'    => '255',
#               'restricted'  => 'host1',
#               'no_backup'   => 1,
#               'mom_cfg_loc' => '/tmp/'
#             );
# createMomCfg(
#               'body'        => $mom_cfg_body,
#               'no_backup'   => 1,
#               'mom_cfg_loc' => '/tmp/'
#             );
#------------------------------------------------------------------------------
#
# Creates a mom_priv/config file while backing up the old file.  Returns 1 on 
# success.
#
#------------------------------------------------------------------------------
sub createMomCfg #($)
  {

  my ($params)    = @_;

  my $test_host   = $props->get_property("Test.Host");

  my $clienthost  = $params->{ 'clienthost'  } || $test_host;
  my $logevent    = $params->{ 'logevent'    } || 255;
  my $restricted  = $params->{ 'restricted'  } || $test_host;
  my $mom_cfg_loc = $params->{ 'mom_cfg_loc' } || $props->get_property('Torque.Home.Dir') . "/mom_priv/config"; 
  my $mom_cfg_sav = $mom_cfg_loc . ".sav";
  my $no_backup   = $params->{ 'no_backup'   } || 0;

  my $mom_cfg     = $params->{ 'body'        } || undef;

  diag("Creating new '$mom_cfg_loc'");

  $mom_cfg =<<CFG
\$clienthost $clienthost
\$logevent $logevent

\$restricted $restricted
CFG
    unless defined $mom_cfg;
 
  # Backup moab.cfg if it hasn't been yet
  if (     ! -e $mom_cfg_sav 
        &&   -e $mom_cfg_loc
        && !    $no_backup)
    {
	ok(rename($mom_cfg_loc, $mom_cfg_sav), "Backing-up $mom_cfg_loc to $mom_cfg_sav")
	    or croak "Unable to back-up $mom_cfg_loc: $!";
    } # END if (     ! -e $mom_cfg_sav 
      #           &&   -e $mom_cfg_loc
      #           && !    $no_backup)

  # Open moan.cfg for creation
  ok(open(MOM_CFG, ">$mom_cfg_loc"), "Creating new mom_priv/config")
    or croak "Unable to open $mom_cfg_loc: $!";

  print MOM_CFG $mom_cfg;

  close MOM_CFG;

  } # END sub createMomCfg #($)

#------------------------------------------------------------------------------
# restoreMomCfg()
# restoreMomCfg({ 
#                'mom_cfg_loc' => '/tmp/mom_config'
#              });
#------------------------------------------------------------------------------
#
# Restores the backup version of the mom_priv/config file.  Returns 1 on 
# success
#
#------------------------------------------------------------------------------
sub restoreMomCfg #($)
{ 

   my ($params)         = @_;

   my $torque_dir      = $props->get_property('Torque.Home.Dir');

   my $mom_cfg_loc     = $params->{ 'mom_cfg_loc'     } || $torque_dir . "/mom_priv/config";
   my $mom_cfg_sav_loc = "$mom_cfg_loc.sav";
 
   diag("Attempting to restore '$mom_cfg_sav_loc' to '$mom_cfg_loc'");

   if(-e $mom_cfg_sav_loc)
     {
  
     ok(rename($mom_cfg_sav_loc, $mom_cfg_loc), "Restoring mom.cfg.sav to mom_priv/config in directory $torque_dir")
       or croak "Unable to restore mom.cfg.sav: $!";  
  
     } # END if(-e $mom_cfg_sav_loc)
   else
     {
 
     pass("'$mom_cfg_sav_loc' doesn't exists.  Unable to restore '$mom_cfg_loc'.");

     } # END else

   return 1;

} # END sub restoreMomCfg #($)

#--------------------------------------------------------------
# createPbsserverNodes();
# createPbsserverNodes({
# 	'hosts' => [ 'g01', 'g02'],
# });
#--------------------------------------------------------------
# Recreates the server_priv/nodes file for PBS Server.
#--------------------------------------------------------------
sub createPbsserverNodes
{
    my ($params) = @_;

    my $nodes     = $params->{hosts} || [$props->get_property('Test.Host')];
    my $node_file = "$torque_home/server_priv/nodes";
    my $node_args = $props->get_property('torque.node.args');

    open NODES, ">$node_file"
	or die "Cannot open $node_file: $!";

    print NODES "$_ $node_args\n" foreach @$nodes;

    close NODES;

    foreach (@$nodes)
    {
	ok(!runCommand("grep $_ $node_file"), "PBS Node File $node_file Contains Node $_")
	    or die "PBS_Server Node file $node_file was NOT setup correctly!";
    }
}

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

=item startTorque($start_torque_config)

Identical to startTorque() except startPbsserverClean() called instead of startPbsserver().

=item stopTorque

stopTorque simply stops both the 'pbs_server' and the 'pbs_mom' daemons.

=item startPbsmom($cfg)

(Re)Start the pbsmom server for given node(s) if specified.  Can pass it the 'arg' hash which is argument that will be appended onto the pbs_mom command.  The hash has the following possible arguments:

 my $mom_cfg = {
                'args'  => '-h host1',
                'node'  => 'host1',
                'nodes' => \@remote_nodes
               };

=item stopPbsmom($cfg)

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

=item startPbsserverClean($)

(Re)Starts the pbs_server on the local machine using the pbs_server -t create command. 'pbs_server -t create' will clean out any configuration files, queues, and jobs, and will initialize the configuration files to default values.  This method can have the following configuration hashref passed to it:

 my $server_cfg = {
                    'setup_str' => $setup_str
                  };

The setup_str is \n delimited list of commands that will be executed afte the pbs_server -t create command is complete.  If this parameter is not passed then the default setup_str will be used.

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

CRI::Test, Torque::Utils, Expect

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
