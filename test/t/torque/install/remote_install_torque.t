#! /usr/bin/perl 

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use POSIX ":sys_wait_h";
use CRI::Util qw(
                   list2array
                 );

plan('no_plan');
setDesc('Install pbs_mom on Remote Nodes');

# Variables
my $nodes_str      = $props->get_property('Torque.Remote.Nodes');
my @nodes          = list2array($nodes_str);
my $pbs_server     = $props->get_property('Test.Host');
my $pbs_server_loc = $props->get_property('Torque.Home.Dir') . "/server_name";

pass("No remote nodes given") if ! scalar @nodes;

my $pid = undef;
my %pids2hosts;
foreach my $host (@nodes)
{
  FORK:
  {
    if ($pid = fork())
    {
      # PARENT
      $pids2hosts{$pid} = $host;
    }
    elsif (defined $pid)
    {
      #  CHILD
      useForkEnv({'child_id' => $host});

      eval
      {
        # Install torque
        my $remote_install_bat = "$FindBin::Bin/remote_install.bat";
        my %ssh                   = runCommandSsh($host, $remote_install_bat, test_success_die => 1);

        # Set the pbs_mom server to trust the pbs_server
        my $cmd = "echo '$pbs_server' > $pbs_server_loc";
        %ssh    = runCommandSsh($host, $cmd, test_success_die => 1);
      };

      POSIX::_exit $? >> 8;
    }
    elsif ($! =~ /No more process/)
    {
      # EAGAIN, supposedly recoverable fork error
      sleep 5;
      redo FORK;
    }
    else
    {
      die "Can't fork: $!\n";
    }
  }
}

# Wait for all child processes to complete
my $kid_pid = 0;
my $timeout = 900;                   # Wait 15 minutes
my $endtime = time() + $timeout;

while ($kid_pid != -1)
{
  $kid_pid = waitpid(-1, WNOHANG);

  if( WIFEXITED($?) )
  {
    my $exit_code = WEXITSTATUS($?);

    fail "Host $pids2hosts{$kid_pid} Setup Failed!" if $exit_code != 0;
  }
  else
  {
    die "Time ($timeout seconds) exceeded by chidren processes" if time() > $endtime;
    sleep 1;
  }
}
