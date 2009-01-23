#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../lib";

use Sys::Hostname;

###############################################################################
# Variables
###############################################################################
# General Variables
my %action_taken        = (
                            '01 CONF CREATED'                => 'no',
                            '02 USERS ADDED'                 => 'no',
                            '03 PROPS COPIED'                => 'no',
                            '04 PATHS MODIFIED'              => 'no',
                            '05 COREDUMP SIZE'               => 'no',
                            '06 COREDUMP TEMPLATE'           => 'no',
                            '07 PROGRAMS INSTALLED'          => 'no',
                            '08 PERLLIBS INSTALLED'          => 'no',
                          );
my $div                 = "\n" . "=" x 80 . "\n\n";
my $yes_regex           = '^(y|yes)$';
my $stdin;

# User variables
my $root                = 'root';
my $group1              = 'testgroup1';
my $group2              = 'testgroup2';
my $user1               = 'testuser1';
my $user2               = 'testuser2';
my $root_bashrc_loc     = '/root/.bashrc';
my $user1_bashrc_loc    = "/home/$user1/.bashrc";
my $user2_bashrc_loc    = "/home/$user2/.bashrc";

# Conf file parameters
my $data_conf_loc       = "/etc/clustertest.conf";
my $data_props_loc      = "/tmp/clustertest.props";
my $test_host           = hostname();
my $log_dir             = "/tmp/testoutput/";
my $torque_home_dir     = "/usr/test/torque/";
my $moab_home_dir       = "/usr/test/moab/";
my $torque_remote_nodes = '';
my $moab_remote_nodes   = '';
my $moab_test_prefix    = '/usr/local/qatests/src/moab/';
my $torque_test_prefix  = '/usr/local/qatests/src/torque/';

# Template props loc
my $props_loc = resolve_path("$FindBin::Bin/../etc/props/torque.props");

###############################################################################
# Check that the user is $root
###############################################################################
my $user = $ENV{ 'USER' };

die("This script should be run as $root")
  unless ($user eq $root);

###############################################################################
# Explain the script
###############################################################################
my $script_explain =<<EXPLAIN;

This script is somewhat naive. It will attempt each step, but doesn't actually 
check if the step was successfull. Please mind the output of the commands it 
runs, and be sure that each part completes without errors

Press any key to continue, or ctrl-c to quit
EXPLAIN

chomp $script_explain;
print $script_explain;
$stdin = <STDIN>;

###############################################################################
# Copy the property files
###############################################################################
my $conf_explain =<<CONF_EXPLAIN;

I am going to create the file '$data_conf_loc'.  This file contains important 
properties that are used in CRI tests.

Is this ok? [y/N] 
CONF_EXPLAIN

print $div;
chomp $conf_explain;
print $conf_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  # Get the Test.Host
  print "\nWhat is the Test.Host? [$test_host] ";
  $stdin           = <STDIN>;
  chomp $stdin;
  $test_host       = $stdin
    unless $stdin eq '';

  # Get the Log.Dir
  print "\nWhere is the Log.Dir? [$log_dir] ";
  $stdin = <STDIN>;
  chomp $stdin;
  $log_dir = $stdin
    unless $stdin eq '';

  # Get the Data.Props.Dir
  print "\nWhere is the Data.Props.Dir? [$data_props_loc] ";
  $stdin = <STDIN>;
  chomp $stdin;
  $data_props_loc = $stdin
    unless $stdin eq '';

  # Get the Moab.Home
  print "\nWhere is the Moab.Home.Dir? [$moab_home_dir] ";
  $stdin = <STDIN>;
  chomp $stdin;
  $moab_home_dir = $stdin
    unless $stdin eq '';

  # Get the Torque.Home
  print "\nWhere is the Torque.Home.Dir? [$torque_home_dir] ";
  $stdin = <STDIN>;
  chomp $stdin;
  $torque_home_dir = $stdin
    unless $stdin eq '';

  # Get the Moab.Remote.Nodes
  print "\nWhere are the Moab.Remote.Nodes? This is a comma seperated list. [$moab_remote_nodes] ";
  $stdin = <STDIN>;
  chomp $stdin;
  $moab_remote_nodes = $stdin
    unless $stdin eq '';

  # Get the Torque.Remote.Nodes
  print "\nWhere are the Torque.Remote.Nodes? This is a comma seperated list. [$torque_remote_nodes] ";
  $stdin = <STDIN>;
  chomp $stdin;
  $torque_remote_nodes = $stdin
    unless $stdin eq '';

  if (open (CONF, ">$data_conf_loc"))
    {

    my $conf_file =<<CONF_FILE;
# QAtests Global Test Parameters File

# The hostname for the Moab/TORQUE server
Test.Host=$test_host

# Enter full path for test-log output
Log.Dir=$log_dir

# Enter full path to locate of test-specific parameters file
Data.Props.Loc=$data_props_loc

# Home Directories
Moab.Home.Dir=$moab_home_dir
Torque.Home.Dir=$torque_home_dir

# Remote nodes
Moab.Remote.Nodes=$moab_remote_nodes
Torque.Remote.Nodes=$torque_remote_nodes

# These are for QA Testing Machines only
# Moab.Test.Prefix=$moab_test_prefix
# Torque.Test.Prefix=$torque_test_prefix
CONF_FILE

    print CONF $conf_file;
    close CONF;


    print "\nThe file '$data_conf_loc' has been created.  You can edit CRI global test variables at this location.\n";

    } # END if (open (CONF, ">$data_conf_loc"))
  else
    {

    warn "WARNING: Unable to open '$data_conf_loc': $!";

    } # END else  

  # Update the action taken
  $action_taken{ '01 CONF CREATED' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot creating '$data_conf_loc'!\n";

  } # END else

###############################################################################
# Copy the props file
###############################################################################
my $cp_cmd = "cp $props_loc $data_props_loc";
my $props_explain =<<PROPS_EXPLAIN;
I am going to create the $data_props_loc by running the command $cp_cmd.

Is this ok? [y/N] 
PROPS_EXPLAIN

print $div;
chomp $props_explain;
print $props_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  my $cp = `$cp_cmd`; 

  if ($? != 0)
    {

    warn "'$cp_cmd' failed: $cp";

    } # END if ($? != 0)

  $action_taken{ '03 PROPS COPIED' } = 'yes';
  print "\nProps file copied\n";

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot copying the props file!\n";

  } # END else

###############################################################################
# Add the testing users
###############################################################################
my $users_explain =<<USERS_EXPLAIN;
I am going to add two new groups and users

 users: $user1, $user2
groups: $group1, $group2

Is this ok? [y/N] 
USERS_EXPLAIN

print $div;
chomp $users_explain;
print $users_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  my $groupadd1 = `groupadd $group1`;
  my $groupadd2 = `groupadd $group2`;
  my $useradd1  = `useradd -m -s /bin/bash -g $group1 -G testgroup1 $user1`;
  my $useradd2  = `useradd -m -s /bin/bash -g $group2 -G testgroup2 $user2`;

  print "\nNOTE: No passwords were set for these users. (not needed for testing).\n";
  $action_taken{ '02 USERS ADDED' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot adding users!\n";

  } # END else

###############################################################################
# Add binaries to /usr/bin
###############################################################################
my $test_bin     = resolve_path("$FindBin::Bin/../bin");
my $torque_bin   = "$torque_home_dir/bin";
my $torque_sbin  = "$torque_home_dir/sbin";
my $moab_bin     = "$moab_home_dir/bin";
my $moab_sbin    = "$moab_home_dir/sbin";

my $path         = "$test_bin:$torque_bin:$torque_sbin:$moab_bin:$moab_sbin:\$PATH";
my $path_export  = "export PATH=\"$path\"";

my $path_explain =<<PATH_EXPLAIN;

I am going to add the following line to '$root_bashrc_loc':

$path_export 

Is this ok? [y/N] 
PATH_EXPLAIN

print $div;
chomp $path_explain;
print $path_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

 if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
    {

    print ROOT_BASHRC "\n# Added by CRI::Test quick_setup.pl\n";
    print ROOT_BASHRC "$path_export\n";

    } # END if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
  else
    {

    warn "WARNING: Unable to open '$root_bashrc_loc': $!";

    } # END else  

  # Update the action taken
  $action_taken{ '04 PATHS MODIFIED' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot exporting the PATH in '$root_bashrc_loc'!\n";

  } # END else

###############################################################################
# Enable Unlimited Coredumping
###############################################################################
my $coreulimit_explain =<<COREULIMIT_EXPLAIN;
I am going to set the coredump file size to unlimited by adding 'ulimit -c unlimited' to the .bashrc for root

NOTE: Needed for automated Coredump detection in CRI::Test module

Is this ok? [y/N] 
COREULIMIT_EXPLAIN

print $div;
chomp $coreulimit_explain;
print $coreulimit_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
    {

    print ROOT_BASHRC "\n# Added by CRI::Test quick_setup.pl\n";
    print ROOT_BASHRC "ulimit -c unlimited\n";

    } # END if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
  else
    {

    warn "WARNING: Unable to open '$root_bashrc_loc': $!";

    } # END else

  $action_taken{ '05 COREDUMP SIZE' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot changing default coredump file size!\n";

  } # END else

###############################################################################
# Set Coredump Pattern for Detection
###############################################################################
my $coreformat_explain =<<COREFORMAT_EXPLAIN;
I am going to make it so all coredump files are stored in the format /tmp/<executable>-core.<pid>
by adding the line 'echo '/tmp/%e-core.%p' > /proc/sys/kernel/core_pattern' to ${root}'s .bashrc file

Note: Needed for automated Coredump detection in CRI::Test module

Is this ok? [y/N] 
COREFORMAT_EXPLAIN

print $div;
chomp $coreformat_explain;
print $coreformat_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
    {

    print ROOT_BASHRC "\n# Added by CRI::Test quick_setup.pl\n";
    print ROOT_BASHRC "echo '/tmp/%e-core.%p' > /proc/sys/kernel/core_pattern\n";

    } # END if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
  else
    {

    warn "WARNING: Unable to open '$root_bashrc_loc': $!";

    } # END else

  $action_taken{ '06 COREDUMP TEMPLATE' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot changing default coredump template!\n";

  } # END else

###############################################################################
# Install packages required by tests
###############################################################################
my @programs = qw( 
                   subversion
                   build-essential
                   psmisc
                   autoconf
                   libxml2-dev
                   zlib1g
                   zlib1g-dev
                   bzip2
                   openssl
                   libssl-dev
                   nfs-common
                   ia32-libs
                 );
my $program_list = join("\n", @programs);

my $program_explain =<<PROGRAM_EXPLAIN;
I am going to install needed programs with apt-get.

I will be installing the following:

$program_list

If you are not on Ubuntu or Debian, say no here, and install these programs (or their equivalents) yourself.

Is this ok? [y/N] 
PROGRAM_EXPLAIN

print $div;
chomp $program_explain;
print $program_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  my $apt_update_cmd = "apt-get update";
  print "\nRunning '$apt_update_cmd'\n";

  my $apt_update_result = `apt-get update`;
  warn "$apt_update_cmd failed: $apt_update_result"
    if $? != 0;

  foreach my $program (@programs)
    {

    my $apt_cmd = "apt-get install --assume-yes $program";
    print "Running '$apt_cmd'\n";
    my $apt_result = `$apt_cmd`;
    warn "$apt_cmd failed: $apt_result"
      if $? != 0;

    } # END foreach my $program (@programs)

  $action_taken{ '07 PROGRAMS INSTALLED' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot installing!\n";

  } # END else

###############################################################################
# Install CPAN modules required by tests
###############################################################################
my @modules = qw(
                    IPC::Run3
                    Test::More
                    Data::Properties
                    TAP::Harness
                    XML::LibXML
                    XML::Simple
                    Proc::Daemon
                    Expect
                  );
my $module_list = join("\n", @modules);

my $cpan_explain =<<CPAN_EXPLAIN;
I am going to use CPAN to install needed Perl modules

The modules are:

$module_list

You may need to configure CPAN before this will work (it will prompt you if needed)

Is this ok? [y/N] 
CPAN_EXPLAIN

print $div;
chomp $cpan_explain;
print $cpan_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  foreach my $module (@modules)
    {
   
    my $cpan_cmd    = "cpan $module";
    print "Running '$cpan_cmd'\n";

    my $cpan_result = `$cpan_cmd`;

    warn "'$cpan_cmd' failed: $cpan_result"
      if $? != 0;

    } # END foreach my $module (@modules)

  $action_taken{ '08 PERLLIBS INSTALLED' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot installing!\n";

  } # END else

###############################################################################
# Summary
###############################################################################
print $div;
print "SUMMARY:\n\n";

foreach my $step (sort keys %action_taken)
  {

  print "$step: " . $action_taken{ $step } . "\n"; 

  } # END foreach my $step (sort keys %action_taken)

###############################################################################
# resolve_path
###############################################################################
sub resolve_path #($)
  {

  my ($path) = @_;

  # Remove ../ from the path the coresponding directories
  my $reg_ex = '[^\/]+\/\.\.(\/)?';
  while ($path =~ /${reg_ex}/)
    {

    $path =~ s/${reg_ex}//;   

    } # END while ($path =~ /\.\./)

  return $path;

  } # END sub resolve_path #($)
