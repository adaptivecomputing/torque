#! /usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan'); 
setDesc('Configure Torque');

my $build_dir   = test_lib_loc().'/../..';
my $home_dir    = $props->get_property('Torque.Home.Dir'); 
my $config_vars = $props->get_property('torque.config.variables');
my $config_args = $props->get_property('torque.config.args');

if( -e "$build_dir/autogen.sh" )
{
  
  my $allowed_attempts = 3;
  my $attempt_num      = 1;
  my $autogen_success  = 1;

  # Workaround for TRQ-304
  while (($allowed_attempts > $attempt_num) && ($autogen_success == 1))
  {
    diag("autogen.sh attempt $attempt_num of $allowed_attempts");
    $autogen_success = runCommand("cd $build_dir; ./autogen.sh");

    $attempt_num++
    if $autogen_success;
  }

  cmp_ok($autogen_success , '==', 0, "autogen.sh succeeded on attempt $attempt_num of $allowed_attempts")
    or die "autogen.sh failed";

}

my $config_cmd = "./configure"
                 . " --prefix="           . $home_dir
                 . " --with-server-home=" . $home_dir 
                 . " --with-debug"
                 . " --enable-syslog"
                 . " $config_args"
                 . " $config_vars";

runCommand("cd $build_dir; $config_cmd", test_success => 1);
