package Torque::Ctrl;

use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Expect;
use Carp;

use CRI::Util qw(
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
                 setupMultiMOM
                );

# Global Variables
our $torque_home      = $props->get_property('Torque.Home.Dir');
our $torque_sbin      = "$torque_home/sbin/";
our $mom_lock_file    = "$torque_home/mom_priv/mom.lock";
our $server_lock_file = "$torque_home/server_priv/server.lock";
our $localhost        = $props->get_property('Test.Host');

our $ps_list        = sub{
  my $process = pop;
  my %cmd = runCommand(@_, logging_off => 1);
  my @matches = ($cmd{STDOUT} =~ /^\w+\s+(\d+).+$process(?:\s+.*)?$/gm);
  return join(" ",@matches);
};
our $remote_ps_list = sub{
    my %cmd     = runCommandSsh($_[0], $_[1], 'logging_off' => 1);
    my @matches = ($cmd{STDOUT} =~ /^\w+\s+(\d+).+$_[2](?:\s+.*)?$/gm);
    return join(" ",@matches);
};

#--------------------------------------------------------------------------------------
# Starts all necessary components of Torque and performs some configuration generation
#  in certain cases.
#
# Currently, a local Torque MOM and local Torque server are always started, with remote MOMs
#  being optional.
#
# Returns a boolean indicating pass or fail.
#--------------------------------------------------------------------------------------
# PARAMETERS:
# - pbs_mom    => hashref to be passed to startPbsmom()
# - pbs_server => hashref to be passed to startPbsserver() or startPbsserverClean()
# - multi_mom  => hashref to be passed to setupMultiMOM()
# - remote_moms => arrayref of remote nodes to be used for MOMs
# - clean_start => boolean that indicates to start all components in a clean environment.
#    Also generates some necessary setup, like node files and server setup. {DEFAULT: 0}
#--------------------------------------------------------------------------------------
sub startTorque
  {
  
  my ($cfg) = @_;

  # Config variables
  my $pbs_mom_cfg    = $cfg->{ 'pbs_mom'     } || {};
  my $pbs_server_cfg = $cfg->{ 'pbs_server'  } || {};
  my $remote_moms    = $cfg->{ 'remote_moms' } || [];
  my $multi_mom      = $cfg->{multi_mom}       || undef;
  my $start_clean    = $cfg->{clean_start}     || 0;

  # Return value
  my $torque_rtn = 1;

  # pbs_mom params
  my @mom_hosts = ($props->get_property('Test.Host'));
  push @mom_hosts, @$remote_moms if scalar @$remote_moms;

  my $mom_params        = {
                             'args' => $pbs_mom_cfg->{ 'args' }
                          };
  my $remote_mom_params = {
                             'args'  => $pbs_mom_cfg->{ 'args' },
                             'nodes' => $remote_moms
                          };

  ##########################
  # trqauthd Section
  ##########################
  runCommand('trqauthd', test_success => 1);
  runCommandSsh($_, 'trqauthd', test_success => 1) foreach @$remote_moms;
  
  ##########################
  # Pbs_mom Section
  ##########################

  $mom_params->{clean} = $cfg->{clean_start};
  $remote_mom_params->{clean} = $cfg->{clean_start};

  if( $multi_mom )
  {
    $multi_mom->{host_list} = \@mom_hosts unless exists $multi_mom->{host_list};
    my $ndata = setupMultiMOM($multi_mom);

    startMultiMom({
        node_data  => $ndata,
        mom_params => $pbs_mom_cfg,
      });
  }
  else
  {
    $torque_rtn = startPbsmom($mom_params);

    if( scalar @$remote_moms )
    {
      my $remote_moms_rtn = startPbsmom($remote_mom_params);

      $torque_rtn = $torque_rtn && $remote_moms_rtn;
    }
  }

  ##########################
  # Pbs_server Section
  ##########################
 
  if($cfg->{clean_start} && !defined $multi_mom)
  {
    createPbsserverNodes({hosts => \@mom_hosts});
  }

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

    my $wait = 120;
    my $endtime = time() + 120;
    diag "Waiting for All Torque Components to Sync (${wait}s Timeout)";

    my $ready = sub{ 
	my $nodes  = qx/$check_cmd/;
	my $result = 1;

	$result = $result && $nodes !~ /^$_\s+.*?(?:down|unknown)/m foreach @_;

	return $result;
    };

    sleep 1 while time <= $endtime && !&$ready( @$mom_hosts );

    die "Torque Components Failed to Sync!\n".qx/$check_cmd 2>&1/ unless time <= $endtime;

    return 1;
}

###############################################################################
# stopTorque
###############################################################################
sub stopTorque
{
    my ($cfg) = @_;

    my $remote_moms = $cfg->{ 'remote_moms' } || undef;
    my $mom_cfg     = $cfg->{pbs_mom}    || {};
    my $server_cfg  = $cfg->{pbs_server} || {};
    my $torque_rtn  = 1;

    if( defined $cfg->{host} )
    {
      $mom_cfg->{node} = $cfg->{host};
      $server_cfg->{host} = $cfg->{host};
    }

    if( defined $remote_moms && ref $remote_moms ne 'ARRAY' )
    {
      $remote_moms = [split ',', $props->get_property('Torque.Remote.Nodes')];
      $remote_moms = [grep {defined $_} @$remote_moms];
    }
    elsif( !defined $remote_moms )
    {
      $remote_moms = [];
    }

    ##########################
    # Pbs_mom Section
    ##########################
    my $remote_mom_params = { 'nodes' => $remote_moms, %$mom_cfg };

    $torque_rtn = $torque_rtn && stopPbsmom($mom_cfg);

    if( scalar @$remote_moms )
    {
	$torque_rtn = $torque_rtn && stopPbsmom($remote_mom_params);
    }

    ##########################
    # Pbs_server Section
    ##########################
    $torque_rtn = $torque_rtn && stopPbsserver($server_cfg);

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
  my $ports      = $cfg->{ports}          || {};
  my $node       = $cfg->{ 'node'       } || undef;
  my $nodes      = $cfg->{ 'nodes'      } || [];
  my $local_node = $cfg->{ 'local_node' } || 0;
  my $do_clean   = $cfg->{clean}          || 0;

  # Set Defaults
  push(@$nodes, $node) if $node;

  $ports->{service_port} ||= 15002;
  $ports->{manager_port} ||= 15003;

  # Assume that we want to start the local node if no remote nodes are passed
  $local_node = 1 if scalar @$nodes == 0;

  my $check_cmd = "netstat -nap | grep -e $ports->{service_port} -e LISTEN | grep pbs_mom";

  my $pbs_mom_cmd  = "${torque_sbin}pbs_mom";
  $pbs_mom_cmd    .= " $args";

  my $clean_cmd = "rm -f $torque_home/mom_priv/jobs/*; rm -f $torque_home/mom_logs/*";

  my @hosts;
  push @hosts, undef if $local_node;
  push @hosts, @$nodes;

  foreach my $n (@hosts)
  {
    my $add_msg = '';
    $add_msg   .= " on Remote Host $n" if defined $n;

    if( $do_clean )
    {
      runCommand($clean_cmd, host => $n, test_success => 1, msg => "Cleaning Torque MOM Files$add_msg...");
    }

    runCommand($pbs_mom_cmd, host => $n, test_success_die => 1, msg => "Starting New Remote PBS_Mom Process$add_msg...");

    my $ps_info = sub{ my %cmd = runCommand($check_cmd, host => $n, logging_off => 1,); return $cmd{STDOUT} };

    my $wait = 30;
    my $endtime = time() + $wait;
    diag "Waiting for Remote PBS_Mom to Start$add_msg... (${wait}s Timeout)";

    sleep 1 while time() <= $endtime && &$ps_info eq '';

    if( time > $endtime )
    {
      die "PBS_Mom Failed to Start$add_msg!";
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
  my $flags      = $cfg->{flags}          || undef;

  my $check_cmd = 'ps aux | grep pbs_mom | grep -v grep';

  push(@$nodes, $node) if $node;

  $local_node = 1 if scalar @$nodes == 0;

  my $momctl_cmd = "${torque_sbin}momctl -s";
  $momctl_cmd   .= " $flags" if defined $flags;

  # Stop any remote pbs_mom's
  if (scalar @$nodes)
  {
    foreach my $n (@$nodes)
    {
      my $ps_info   = sub{ return &$remote_ps_list($n, $check_cmd, 'pbs_mom'); };

      unless( &$ps_info eq '')
      { 
        my $wait = 30;
        diag "Stopping Remote Torque MOM on Host $n... (${wait}s Timeout)";

        my %momctl = runCommandSsh($n, $momctl_cmd);

        if( ($momctl{EXIT_CODE} != 0 || $momctl{STDERR} =~ /^ERROR:/) && &$ps_info ne '' )
        {
          my $kill_cmd = 'kill -9 '.&$ps_info;

          diag "Normal Shutdown Failed! Attempting to SIGKILL Remote PBS_Mom";
          runCommandSsh($n, $kill_cmd);
        }

        sleep 1 while $wait-- > 0 && &$ps_info ne '';

        if( $wait <= 0 )
        {
          die "Unable to Stop Remote PBS_Mom on Host $n!";
        }

      }
    }
  }

  # Stop local pbs_mom
  if ($local_node)
  {
    my $ps_info   = sub{ return &$ps_list($check_cmd, 'pbs_mom'); };

    unless( &$ps_info eq '' )
    { 
      my $wait = 30;
      diag "Stopping Local Torque MOM... (${wait}s Timeout)";

      my %momctl = runCommand($momctl_cmd);

      if( ($momctl{EXIT_CODE} != 0 || $momctl{STDERR} =~ /^ERROR:/) && &$ps_info ne '' )
      {
        my $kill = "kill -9 ".&$ps_info;

        diag "Normal Shutdown Failed! Attempting to SIGKILL pbs_mom";
        system $kill;
      }

      sleep 1 while $wait-- > 0 && &$ps_info ne '';

      if( $wait <= 0 )
      {
        die "Unable to Stop Local PBS_Mom!";
      }
    }
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
  my $endtime = time + 30;
  diag "Waiting for PBS_Server to Start... (${wait}s Timeout)";

  sleep 1 while time <= $endtime && &$ps_info eq '';

  if( time > $endtime )
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

  my $localhost = $props->get_property('Test.Host');
  my $hosts = $cfg->{hosts} || [$localhost];
  my $add_queues = $cfg->{add_queues} || [];

  # Variables
  my $default_queue = 'batch';
  my @queues_cfgs = (
    {
      $default_queue => {
        queue_type => 'execution',
        started    => 'true',
        enabled    => 'true',
        'resources_default.walltime' => '1:00:00',
        'resources_default.nodes' => 1,
      },
    },
    @$add_queues,
  );

  my $qmgr_cmd          = "$torque_home/bin/qmgr";
  my $pbs_server_cmd    = "${torque_sbin}pbs_server";
  my $operator          = 'root';
  my $manager           = 'root';
  my $setup_str         = '';

  # Set access to pbs_server
  foreach( @$hosts )
  {
    $setup_str .= <<SETUP;
    $qmgr_cmd -c 'set server operators += $operator\@$_'
    $qmgr_cmd -c 'set server managers += $manager\@$_'
    $qmgr_cmd -c 'set server acl_hosts += $_'
SETUP
  }

  # Set pbs_server settings
  $setup_str .= <<SETUP;
  $qmgr_cmd -c 'set server scheduling = true'
  $qmgr_cmd -c 'set server keep_completed = 300'
  $qmgr_cmd -c 'set server mom_job_sync = true'
SETUP

  # Setup various specified queues
  foreach my $qref (@queues_cfgs)
  {
    my ($name) = keys %$qref;
    $qref = $qref->{$name};

    $setup_str .= "$qmgr_cmd -c 'create queue $name'\n";

    while( my ($key, $val) = each(%$qref) )
    {
      $setup_str .= "$qmgr_cmd -c 'set queue $name $key = $val'\n";
    }
  }

  $setup_str .= "$qmgr_cmd -c 'set server default_queue = $default_queue'\n";

  # Configuration Variables
  my @setup_lines      = split /\n/, ($cfg->{setup_lines} || $setup_str);
  push @setup_lines, @{$cfg->{add_setup_lines}} if $cfg->{add_setup_lines};

  # pbs_server command
  my $pbs_cmd  = "$pbs_server_cmd -t create";

  # Clean server files
  my $clean_cmd = <<CMD;
rm -f $torque_home/server_priv/jobs/*;
rm -f $torque_home/server_priv/queues/*;
rm -f $torque_home/server_priv/accounting/*;
rm -f $torque_home/server_logs/*
CMD
  runCommand($clean_cmd, test_success => 1, msg => 'Cleaning Torque Server Files...');

  # Back-up Nodes file, if it exists
  if( runCommand("ls $torque_home/server_priv/nodes", logging_off => 1 ) == 0 )
  {
    runCommand("mv $torque_home/server_priv/nodes $torque_home/nodes.bak", test_success => 1, msg => 'Backing up existing nodes file');
  }

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
  my $endtime = time() + $wait;
  diag "Waiting for PBS_Server to Start Clean... (${wait}s Timeout)";

  sleep 1 while time() <= $endtime && &$ps_info eq '';

  if( time > $endtime )
  {
    fail "Local pbs_server Failed to Start\n".qx/$check_cmd 2>&1/;
    return 0;
  }

  diag("Re-Configure Torque");
  runCommand($_, 'test_success_die' => 1) foreach @setup_lines;

  sleep_diag 3;

  stopPbsserver();
  
  if( runCommand("ls $torque_home/nodes.bak", logging_off => 1 ) == 0 )
  {
    runCommand("mv $torque_home/nodes.bak $torque_home/server_priv/nodes", test_success => 1, msg => 'Restoring existing nodes file');
  }
  
  startPbsserver($cfg);

  return 1;
}

###############################################################################
# stopPbsserver
###############################################################################
sub stopPbsserver 
{
  my ($params) = @_;

  my $host  = $params->{host}  || undef;
  my $flags = $params->{flags} || undef;

  my $return = 1;

  # Commands  
  my $qterm_cmd = "qterm -t quick";
  $qterm_cmd .= " $flags" if defined $flags;
  my $check_cmd = "ps aux | grep pbs_server | grep -v grep";

  my $ps_info = sub{ return &$ps_list($check_cmd, host => $host, 'pbs_server'); };

  unless( &$ps_info eq '' )
  { 
    my $wait = 30;
    my $endtime = time + $wait;
    diag "Stopping Torque Server... (${wait}s Timeout)";

    my %qterm = runCommand($qterm_cmd, host => $host);

    if( $qterm{EXIT_CODE} != 0 && &$ps_info ne '' )
    {
      my $kill = 'kill -9 '.&$ps_info;
      diag "Normal Shutdown Failed! Attempting to SIGKILL pbs_server";
      runCommand($kill, host => $host, logging_off => 1);
    }


    sleep 1 while time <= $endtime && &$ps_info ne '';

    if( time > $endtime )
    {
      fail "Unable to Stop PBS_Server!";
      runCommand("$check_cmd 2>&1", host => $host);
      return 0;
    }
  }

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
#    attr_vals => {
#      'pbsserver'   => 'host1',
#      'clienthost'  => 'host1',
#      'logevent'    => '255',
#      'restricted'  => 'host1',
#    },
#    'no_backup'   => 1,
# );
# createMomCfg(
#   'body'        => $mom_cfg_body,
#   'no_backup'   => 1,
# );
#------------------------------------------------------------------------------
#
# Creates a mom_priv/config file while backing up the old file.
#
#------------------------------------------------------------------------------
sub createMomCfg #($)
{

  my ($params)    = @_;

  my $test_host   = $props->get_property("Test.Host");

  my %default_attr_vals = (
    pbsserver => $test_host,
    logevent  => 255,
  );

  my $host        = $params->{host}        || undef;
  my $attr_vals   = $params->{attr_vals}   || \%default_attr_vals;
  my $mom_cfg_loc = $params->{mom_cfg_loc} || $props->get_property('Torque.Home.Dir') . "/mom_priv/config"; 
  my $no_backup   = $params->{no_backup}   || 0;
  my $mom_cfg     = $params->{body}        || undef;
  my $mom_cfg_sav = $mom_cfg_loc . ".sav";

  my $tmp_loc = defined $host ? '/tmp/config' : $mom_cfg_loc;

  my $no_log_runcmd = sub{ return runCommand($_[0], host => $host, logging_off => 1); };
  my $runcmd        = sub{ return runCommand(shift, host => $host, @_); };

  unless( defined $mom_cfg )
  {
    # Make sure defaults are present
    $attr_vals = { %default_attr_vals, %$attr_vals };

    # We want some attrs to be at the top of the config, for readability's sake
    my @print_order = qw( pbsserver clienthost restricted logevent );
    @print_order = grep { exists $attr_vals->{$_} } @print_order;

    $mom_cfg = join("\n", map { "\$$_    ".$attr_vals->{$_} }  @print_order)."\n";

    delete $attr_vals->{$_} foreach @print_order;

    $mom_cfg .= join("\n", map { "\$$_    ".$attr_vals->{$_} }  sort keys %$attr_vals);
  }

  # Backup moab.cfg if it hasn't been yet
  if ( ! $no_backup &&  &$no_log_runcmd("ls $mom_cfg_sav") != 0 && &$no_log_runcmd("ls $mom_cfg_loc") == 1 )
  {
    &$runcmd("mv $mom_cfg_loc $mom_cfg_sav", test_success => 1, msg => 'Backing-up Existing Torque MOM config');
  }

  open MOM_CFG, ">$tmp_loc"
    or die "Unable to open $tmp_loc: $!";

  print MOM_CFG $mom_cfg;

  close MOM_CFG;

  # For logging purposes
  runCommand("cat $tmp_loc");

  if( defined $host )
  {
    runCommand("scp -B $tmp_loc $host:$mom_cfg_loc", test_success => 1, msg => "Copying Torque MOM config to Remote Client $host");
    runCommandSsh($host, "chmod 0644 $mom_cfg_loc", test_success => 1, msg => "Setting File Permissions on Remote Client $host");
  }

  return 1;
}

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
# Creates the server_priv/nodes file for Torque Server.
#--------------------------------------------------------------
sub createPbsserverNodes
{
  my ($params) = @_;

  my $nodes      = $params->{hosts} || [$props->get_property('Test.Host')];
  my $node_attrs = $params->{node_attrs} || {};
  my $node_file  = "$torque_home/server_priv/nodes";
  my $node_args  = $props->get_property('torque.node.args');

  open NODES, ">$node_file"
    or die "Cannot open $node_file: $!";

  foreach my $node ( @$nodes )
  {
    my $line = "$node $node_args";
    $line .= ' '.join(' ', map { "$_=".$node_attrs->{$node}{$_} } keys %{$node_attrs->{$node}}) if exists $node_attrs->{$node};

    print NODES "$line\n";
  }

  close NODES;

  runCommand("cat $node_file", test_success_die => 1, msg => "Created Torque Server Nodes File");

  return 1;
}

#-------------------------------------------------------------------------------------
# Ensures that the proper setup is in place (manipulates /etc/hosts, creates server
#  nodes file) for multi-mom.
# Returns a hashref of all moms and their relevant info, like IP address, ports, etc.
#
# NOTE: Please make sure to run teardownMultiMom once testing is complete.
#-------------------------------------------------------------------------------------
# PARAMETERS:
# x = mutually-exclusive
#
#  x total_moms => integer of the total number of MOMs desired. Function will look at 
#     the hosts listed in Torque.Remote.Nodes and even distribute MOMs across them.
#     (includes localhost, as well)
#  x mom_struct => hashref of hosts with the value being the number of MOMs desired on
#     that specific host.
#  - host_list  => arrayref of hosts to use for distributing MOMs. (Optional)
#     {DEFAULT: Torque.Remote.Nodes + localhost}
#
#  NOTE: When neither total_moms nor mom_struct are supplied, the function will
#    generate two MOMs on each host in 'host_list'
#-------------------------------------------------------------------------------------
sub setupMultiMOM
{
  my ($params) = @_;

  # Handle passed in parameters
  die "The 'total_moms' and 'mom_struct' parameters are mutually-excuslive!" if exists $params->{total_moms} && exists $params->{mom_struct};

  my $localhost  = $props->get_property('Test.Host');
  my $hosts      = $params->{host_list}  || [$localhost, split ',', $props->get_property('Torque.Remote.Nodes')];
  my $mom_struct = $params->{mom_struct} || undef;
  my $total_moms = $params->{total_moms} || 2 * @$hosts;

  my (%node_data, %node_file);

  # If using total_moms, determine the mom_struct equivalent accounting for host_list
  if($total_moms && !defined $mom_struct)
  {
    my $moms_per_host = int($total_moms / @$hosts);

    $mom_struct->{$_} = $moms_per_host foreach @$hosts;

    # Handle when things don't divide evenly
    my $remaining = $total_moms - $moms_per_host * @$hosts;
    until( $remaining == 0 )
    {
      foreach( reverse @$hosts )
      {
        last if $remaining == 0;
        $mom_struct->{$_}++;
        $remaining--;
      }
    }
  }

  # Generate and "distribute" new /etc/hosts file
  # Also, determine some node information
  my $host_string;
  my $marker = '#QA-DATA-MARKER';
  foreach my $host ( @$hosts )
  {
    my %ping = runCommand("ping -c 1 $host", logging_off => 1);
    $ping{STDOUT} =~ /^PING \S+ \((\S+)\) /m;

    my $ip_address = $1;
    $node_data{$host}{ip_address} = $ip_address;

    my @moms = ($host);
    push @moms, "$host-m".$_ for 2 .. $mom_struct->{$host} + 1;
    push @{$node_file{hosts}}, @moms;

    $node_data{$host}{mom_aliases} = \@moms;

    $host_string .= "$ip_address  ".join(' ', @moms)."\n";

    # Select port numbers
    my $start_port = 30001;
    foreach( @moms )
    {
      $node_data{$host}{ports}{$_} = {
        service_port => $start_port++,
        manager_port => $start_port++,
      };

      $node_file{node_attrs}{$_}{mom_service_port} = $node_data{$host}{ports}{$_}{service_port};
      $node_file{node_attrs}{$_}{mom_manager_port} = $node_data{$host}{ports}{$_}{manager_port};
    }
  }
  $host_string = "$marker\n$host_string$marker";

  foreach my $host (@$hosts)
  {
    my $tmp_loc = '/tmp/tmp_hosts';
    if( $host eq $localhost )
    {
      $tmp_loc = '/etc/hosts';
    }
    else
    {
      runCommand("scp -B $host:/etc/hosts $tmp_loc");
    }

    open HOSTS, "<$tmp_loc"
      or die "Unable to open $tmp_loc: $!";

    my @lines = <HOSTS>;
    close HOSTS;

    # Erase previous QA data from file
    my $remove_lines = 0;
    my @filtered_lines;
    foreach(@lines)
    {
      if($remove_lines && /^$marker/)
      {
        $remove_lines = 0;
      }
      elsif(/^$marker/)
      {
        $remove_lines = 1;
      }
      elsif( $remove_lines )
      {
        next;
      }
      else
      {
        push @filtered_lines, $_;
      }
    }

    open HOSTS, ">$tmp_loc"
      or die "Unable to write: $!";
    print HOSTS join('', @filtered_lines)."\n$host_string";
    close HOSTS;

    unless($host eq $localhost)
    {
      runCommand("scp -B $tmp_loc $host:/etc/hosts");
    }

    runCommand("cat $tmp_loc", test_success => 1, msg => "Edited /etc/hosts file for Host $host");
  }

  # Create the node file for the server
  createPbsserverNodes(\%node_file);

  return \%node_data;
}

#-------------------------------------------------------------------------------------
# Starts up all Torque MOMs in the special way Multi-MOM must be started.
#-------------------------------------------------------------------------------------
# PARAMETERS:
# - node_data => hashref returned by setupMultiMOM() (preferrably) (REQUIRED)
#-------------------------------------------------------------------------------------
sub startMultiMom
{
  my ($params) = @_;

  my $node_data  = $params->{node_data} || die "Must provide node data via the 'node_data' key!";
  my $mom_params = $params->{mom_params} || {};

  foreach my $node (keys %$node_data)
  {
    foreach my $mom (@{$node_data->{$node}{mom_aliases}})
    {
      my $href = {%$mom_params};
      my $args .= "-m -M $node_data->{$node}{ports}{$mom}{service_port} -R $node_data->{$node}{ports}{$mom}{manager_port} -A $mom";

      $href->{args}  = $args.' '.($href->{args} || '');
      $href->{ports} = $node_data->{$node}{ports}{$mom};
      $href->{node}  = $node unless $node eq $localhost;

      startPbsmom($href);
    }
  }

  return 1;
}

#-------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------
sub teardownMultiMom
{
}

1;
