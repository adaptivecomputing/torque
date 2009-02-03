#! /usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;
use CRI::Utils   qw(
                     run_and_check_cmd
                   );

plan('no_plan');
setDesc('Remove Torque Home Directory');

my $torque_home_dir = $props->get_property("Torque.Home.Dir");
my $rm_cmd          = "rm -rf $torque_home_dir";

if (-d $torque_home_dir) 
  { 
  
  run_and_check_cmd($rm_cmd);

  } # END if (-d $torque_home_dir)  
else
  { 
  
  pass("'$torque_home_dir' doesn't exists.  No need to remove it."); 
  
  } # END else
