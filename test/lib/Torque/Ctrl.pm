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

use base 'Exporter';

our @EXPORT = qw(
                 startTorque
                 startTorqueClean
		 syncServerMom
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

our $ps_list        = sub{
    my @matches = (qx/$_[0]/ =~ /^\w+\s+(\d+).+$_[1](?:\s+.*)?$/gm);
    return join(" ",@matches);
};
our $remote_ps_list = sub{
    my %cmd     = runCommandSsh($_[0], $_[1], 'logging_off' => 1);
    my @matches = ($cmd{STDOUT} =~ /^\w+\s+(\d+).+$_[2](?:\s+.*)?$/gm);
    return join(" ",@matches);
};

###############################################################################
# startTorque ($)
###############################################################################
sub startTorque #($)# 
  {
  
  my ($cfg) = @_;
  my @mom_hosts;

  # Config variables
  my $pbs_mom_cfg    = $cfg->{ 'pbs_mom'     } || {};
  my $pbs_server_cfg = $cfg->{ 'pbs_server'  } || {};
  my $remote_moms    = $cfg->{ 'remote_moms' } || [];

  # Return value
  my $torque_rtn = 1;

  # pbs_mom params
  my $mom_params        = {
                             'args' => $pbs_mom_cfg->{ 'args' }
                          };
  my $remote_mom_params = {
                             'args'  => $pbs_mom_cfg->{ 'args' },
                             'nodes' => $remote_moms
                          };

  ##########################
  # Clean Start Required
  ##########################
  if( exists $cfg->{clean_start} )
  {
      runCommand(
	  "rm $torque_home/*_logs/* $torque_home/server_priv/accounting/*",
	  'test_success' => 1,
	  'msg' => 'Cleaning up Torque Files'
      );
  }

  ##########################
  # Pbs_mom Section
  ##########################
  
  $torque_rtn = startPbsmom($mom_params);

  push @mom_hosts, $props->get_property('Test.Host');

  if( scalar @$remote_moms )
  {
      my $remote_moms_rtn = startPbsmom($remote_mom_params);

      $torque_rtn = $torque_rtn && $remote_moms_rtn;
      
      push @mom_hosts, @$remote_moms;
  }

  ##########################
  # Pbs_server Section
  ##########################
  
  $pbs_server_cfg->{mom_hosts} = \@mom_hosts;

  my $pbs_server_rtn = !exists $cfg->{clean_start}
		     ? startPbsserver($pbs_server_cfg)
		     : startPbsserverClean($pbs_server_cfg);

  $torque_rtn = $torque_rtn && $pbs_server_rtn;

  ok($torque_rtn, "All Torque Components Started");
  
  syncServerMom({ 'mom_hosts' => \@mom_hosts });
  
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
# syncServerMom
###############################################################################
sub syncServerMom
{
    my ($params) = @_;

    my $check_cmd = 'pbsnodes -l all';
    my $mom_hosts = $params->{mom_hosts} || [ $props->get_property('Test.Host') ];

    my $wait = 30;
    diag "Waiting for All Torque Components to Sync (${wait}s Timeout)";

    my $ready = sub{ 
	my $nodes  = qx/$check_cmd/;
	my $result = 1;

	$result = $result && $nodes !~ /^$_\s+.*?(?:down|unknown)/m foreach @_;

	return $result;
    };

    sleep 1 while $wait-- > 0 && !&$ready( @$mom_hosts );

    die "Torque Components Failed to Sync!\n".qx/$check_cmd 2>&1/ unless $wait > 0;

    return 1;
}

###############################################################################
# stopTorque
###############################################################################
sub stopTorque
{
    my ($cfg) = @_;

    my $remote_moms = $cfg->{ 'remote_moms' } || [];
    my $torque_rtn  = 1;

    ##########################
    # Pbs_mom Section
    ##########################
    my $remote_mom_params = { 'nodes' => $remote_moms };

    $torque_rtn = $torque_rtn && stopPbsmom();

    if( scalar @$remote_moms )
    {
	$torque_rtn = $torque_rtn && stopPbsmom($remote_mom_params);
    }

    ##########################
    # Pbs_server Section
    ##########################
    $torque_rtn = $torque_rtn && stopPbsserver();

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
    $local_node = 1 if scalar @$nodes == 0;

    my $check_cmd = "ps aux | grep pbs_mom | grep -v grep";

    my $pbs_mom_cmd  = "${torque_sbin}pbs_mom";
    $pbs_mom_cmd    .= " $args";

    # Start any Remote mom's
    if (scalar @$nodes)
    {
	foreach my $n (@$nodes)
	{
	    my %ssh = runCommandSsh($n, $pbs_mom_cmd, 'test_success_die' => 1, 'msg' => "Starting New Remote PBS_Mom Process on Host $n...");
	    
	    my $ps_info = sub{ return &$remote_ps_list($n, $check_cmd, 'pbs_mom'); };
	    
	    my $wait = 30;
	    diag "Waiting for Remote PBS_Mom to Start on Host $n... (${wait}s Timeout)";
	    
	    sleep 1 while $wait-- > 0 && &$ps_info eq '';

	    if( $wait > 0 )
	    {
		pass "Remote PBS_Mom is now Running on Host $n";
	    }
	    else
	    {
		die "Remote PBS_Mom Failed to Start on Host $n!";
	    }
	}
    }
    
    # Start local mom
    if ($local_node)
    {
	runCommand($pbs_mom_cmd, 'test_success_die' => 1, 'msg' => 'Starting New Local PBS_Mom Process..');

	my $ps_info = sub{ return &$ps_list($check_cmd, 'pbs_mom'); };

	my $wait = 30;
	diag "Waiting for Local PBS_Mom to Start... (${wait}s Timeout)";

	sleep 1 while $wait-- > 0 && &$ps_info eq '';

	if( $wait > 0 )
	{
	    pass "Local PBS_Mom is now Running";
	}
	else
	{
	    die "Local PBS_Mom Failed to Start!";
	}
    }

    return 1;
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

    my $check_cmd = 'ps aux | grep pbs_mom | grep -v grep';

    push(@$nodes, $node) if $node;

    $local_node = 1 if scalar @$nodes == 0;

    my $momctl_cmd = "${torque_sbin}momctl -s";

    # Stop any remote pbs_mom's
    if (scalar @$nodes)
    {
	foreach my $n (@$nodes)
	{
	    diag "Attempting to Shutdown Remote PBS_Mom on Host $n...";
	    my $ps_info = sub{ return &$remote_ps_list($n, $check_cmd, 'pbs_mom'); };

	    unless( &$ps_info eq '')
	    { 
		my %momctl = runCommandSsh($n, $momctl_cmd);

		if( $momctl{EXIT_CODE} != 0 && &$ps_info ne '' )
		{
		    my $kill_cmd = 'kill -9 '.&$ps_info;
		    
		    diag "Normal Shutdown Failed! Attempting to SIGKILL Remote PBS_Mom";
		    runCommandSsh($n, $kill_cmd, 'logging_off' => 1);
		}
	    
		my $wait = 30;
		diag "Waiting for Remote PBS_Mom to Stop on Host $n... (${wait}s Timeout)";

		sleep 1 while $wait-- > 0 && &$ps_info ne '';

		if( $wait <= 0 )
		{
		    die "Unable to Stop Remote PBS_Mom on Host $n!";
		}

	    }

	    pass "Remote PBS_Mom is now Stopped on Host $n";
	}
    }
    
    # Stop local pbs_mom
    if ($local_node)
    {
	diag "Attempting to Shutdown Local PBS_Mom...";
	my $ps_info   = sub{ return &$ps_list($check_cmd, 'pbs_mom'); };

	unless( &$ps_info eq '' )
	{ 
	    my %momctl = runCommand($momctl_cmd);

	    if( $momctl{EXIT_CODE} != 0 )
	    {
		my $kill = "kill -9 ".&$ps_info;

		diag "Normal Shutdown Failed! Attempting to SIGKILL pbs_mom";
		system $kill;
	    }

	    my $wait = 30;
	    diag "Waiting for Local PBS_Mom to Stop... (${wait}s Timeout)";

	    sleep 1 while $wait-- > 0 && &$ps_info ne '';

	    if( $wait <= 0 )
	    {
		die "Unable to Stop Local PBS_Mom!";
	    }
	}
	
	pass "Local PBS_Mom is now Stopped";
    }

    return 1;
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

    # Start the pbs server
    runCommand($pbs_server_cmd, 'test_success_die' => 1, 'msg' => 'Starting New PBS_Server Process...');

    my $check_cmd = "ps aux | grep pbs_server | grep -v grep";

    my $ps_info = sub{ return &$ps_list($check_cmd, 'pbs_server'); };

    my $wait = 30;
    diag "Waiting for PBS_Server to Start... (${wait}s Timeout)";

    sleep 1 while $wait-- > 0 && &$ps_info eq '';

    if( $wait > 0 )
    {
	pass "PBS_Server is now Running!";
    }
    else
    {
	fail "PBS_Server Failed to Start";
	return 0;
    }

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
  $server_nodes{hosts} = $cfg->{mom_hosts} if exists $cfg->{mom_hosts};
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

  my $ps_info = sub{ return &$ps_list($check_cmd, 'pbs_server'); };
  
  my $wait = 30;
  diag "Waiting for PBS_Server to Start Clean... (${wait}s Timeout)";

  sleep 1 while $wait-- > 0 && &$ps_info eq '';

  if( $wait > 0 )
  {
      pass "Local pbs_server is now Running!";
  }
  else
  {
      fail "Local pbs_server Failed to Start\n".qx/$check_cmd 2>&1/;
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
    my $check_cmd = "ps aux | grep pbs_server | grep -v grep";
  
    my $ps_info = sub{ return &$ps_list($check_cmd, 'pbs_server'); };
    
    unless( $ps_info eq '' )
    { 
	my %qterm = runCommand($qterm_cmd);

	if( $qterm{EXIT_CODE} != 0 && &$ps_info ne '' )
	{
	    my $kill = 'kill -9 '.&$ps_info;
	    diag "Normal Shutdown Failed! Attempting to SIGKILL pbs_server";
	    qx/$kill/;
	}

	my $wait = 30;
	diag "Waiting for PBS_Server to Stop... (${wait}s Timeout)";

	sleep 1 while $wait-- > 0 && &$ps_info ne '';

	if( $wait == 0 )
	{
	    fail "Unable to Stop PBS_Server!\n".qx/$check_cmd 2>&1/;
	    return 0;
	}
    }

    pass "PBS_Server is now Stopped";

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
