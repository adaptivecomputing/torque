#! /usr/bin/perl 

use strict;
use warnings;

use CRI::Test;
use Torque::Test::Utils qw(list2array);

plan('no_plan');
setDesc('Install pbs_mom on remote compute nodes');

# Variables
my $scp_cmd;
my %scp;
my $ssh_cmd;
my %ssh;

my $nodes_str              = $props->get_property('torque.remote.nodes');
my @nodes                  = list2array($nodes_str);
my $torque_build_dir       = $props->get_property('torque.build.dir');
my $torque_install_dir     = $props->get_property('torque.home.dir');
my $mom_priv_config        = $torque_install_dir . "mom_priv/config";

SKIP:
  {

  skip("No remote nodes to install", 1)
    if (! scalar @nodes);

  # Figure out the torque_package_mom_*.sh
  my $ls_cmd      = "ls ${torque_build_dir}torque-package-mom-*.sh";
  my %ls          =  runCommand($ls_cmd);

  ok($ls{ 'EXIT_CODE' } == 0, "Checking exit code of '$ls_cmd'")
    or die "'$ls_cmd' failed: $ls{ 'STDERR' }";

  my $mom_sh_path = $ls{ 'STDOUT' };
  chomp $mom_sh_path;

  # Get just the file name
  $mom_sh_path   =~ /(torque-package-mom-.+\.sh)/;
  my $mom_sh     = $1;
  my $target_mom = "/tmp/$mom_sh";

  foreach my $node (@nodes)
    {

    # Copy the mom install file over
    $scp_cmd = "scp -v $mom_sh_path $node:$target_mom";
    %scp     = runCommand($scp_cmd);
    ok($scp{ 'EXIT_CODE' } == 0, "Checking exit code of '$scp_cmd'")
      or die "'$scp_cmd' failed: $scp{ 'STDERR' }";

    # Remove the install directory
    $ssh_cmd = "rm -rf $torque_install_dir";
    %ssh     = runCommand($node, $ssh_cmd);

    # Run the installation
    $ssh_cmd = "sh $target_mom --install --overwriteconf";
    %ssh     = runCommandSsh($node, $ssh_cmd);
    ok($ssh{ 'EXIT_CODE' } == 0, "Checking exit code of '$ssh_cmd'")
      or die "'$ssh_cmd' failed: $ssh{ 'STDERR' }";

    # Scp over the mom_priv/config file
    $scp_cmd = "scp $mom_priv_config $node:$mom_priv_config";
    %scp     = runCommand($scp_cmd);
    ok($scp{ 'EXIT_CODE' } == 0, "Checking exit code of '$scp_cmd'")
      or die "'$scp_cmd' failed: $scp{ 'STDERR' }";

    } # END foreach my $node (@nodes)

  }; # END SKIP:
