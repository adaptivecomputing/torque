#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan'); 
setDesc('Configure Torque');

ok(-d ($props->get_property('torque.build.dir')),"Checking if torque build dir exists") or die("Torque build dir doesn't exist");

# Change directory to build dir
ok(chdir $props->get_property('torque.build.dir'),"Changing directory to " . $props->get_property('torque.build.dir')) or die("Couldn't change to torque build directory");

if( -e 'autogen.sh' )
{
  
  my $allowed_attempts = 3;
  my $attempt_num      = 1;
  my $autogen_success  = 0;
  my %autogen_result   = ();

  # Workaround for TRQ-304
  while (($allowed_attempts > $attempt_num) && ($autogen_success == 0))
  {

  diag("autogen.sh attempt $attempt_num of $allowed_attempts");
  %autogen_result = runCommand('./autogen.sh');

  $autogen_success = 1
    if $autogen_result{'EXIT_CODE'} == 0;

  $attempt_num++
    unless $autogen_success;

  } # END  while (($allowed_attempts > $attempt_num) && ($autogen_success == 0))

  ok($autogen_success, "autogen.sh succeeded on attempt $attempt_num of $allowed_attempts")
    or die "autogen.sh failed: $autogen_result{STDERR}";

}

# Run configure
runCommand("./configure --prefix=" . $props->get_property('Torque.Home.Dir') . " --with-server-home=" . $props->get_property('Torque.Home.Dir') . " " . $props->get_property('torque.config.args') . " --with-debug --enable-syslog") && die("Torque configure failed!");
