#! /usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan'); 
setDesc('Configure Torque to use BLCR checkpointing');

my $build_dir   = "$FindBin::Bin/../../../../";
my $home_dir    = $props->get_property('torque.home.dir'        ); 
my $config_vars = $props->get_property('torque.config.variables');
my $config_args = $props->get_property('torque.config.args'     );

ok(-d ($build_dir), "Checking if torque build dir exists") 
  or die("Torque build dir doesn't exist");

# Change directory to build dir
ok(chdir $build_dir, "Changing directory to " . $build_dir) 
  or die("Couldn't change to torque build directory");

# Run configure
my $config_cmd = "./configure"
                 . " --prefix="           . $home_dir
                 . " --with-server-home=" . $home_dir 
                 . " --with-debug"
                 . " --enable-syslog"
                 . " --enable-unixsockets=no"
                 . " --enable-blcr"
                 . " CFLAGS=\"-g\""
                 . " $config_vars";

my %config = runCommand($config_cmd);
cmp_ok($config{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$config_cmd'")
  or die("Config failed: $config{ 'STDERR' }");

