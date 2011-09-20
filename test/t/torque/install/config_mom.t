#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();
 
use CRI::Test;
use CRI::Util::Props qw( remote_props );
plan('no_plan');
setDesc('Configure Torque MOMs on the Compute Nodes');

my $host    = $props->get_property('Test.Host');
my @nodes   = split ',', ($props->get_property('Torque.Remote.Nodes') || '');


foreach my $name ($host, @nodes)
{
  my $not_localhost = 1 if $name ne $host;
  
  my $cust_props = $not_localhost ? remote_props({ host => $name }): $props;

  my $final_loc = $cust_props->get_property('Torque.Home.Dir') . "/mom_priv/config";
  my $tmp_loc   = $not_localhost ? '/tmp/config' : $final_loc;

  open CONFIG, ">$tmp_loc"
    or die "Couldn't open torque mom config file $tmp_loc: $!";

  print CONFIG <<EOF;
\$pbsserver     $host  # note: hostname running pbs_server
\$logevent       255
EOF

  close CONFIG;

  if($not_localhost)
  {
    runCommand("scp -Bv $tmp_loc $host:$final_loc", test_success => 1, msg => "Copying Torque MOM config to Remote Client $name");
    runCommandSsh($name, "chmod 0644 $final_loc", test_success => 1, msg => "Setting File Permissions on Remote Client $name");
  }
  else
  {
    ok(chmod(0644, $final_loc), 'Setting Torque mom config file permissions');
  }
}
