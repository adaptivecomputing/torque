#! /usr/bin/perl 

use CRI::Test;
plan('no_plan');
setDesc('Remove Torque');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";


my $build_dir  = $props->get_property('torque.build.dir');
$build_dir =~ s%/$%%;    # Remove trailing /

# Remove Torque install directory
runCommand("rm -rf " . $props->get_property('torque.home.dir'));
ok(!-d $props->get_property('torque.home.dir'),"removing " . $props->get_property('torque.home.dir')) or die("Torque install directory still exists but shouldn't");

# Remove Torque build directory (and tarball)
runCommand("rm -rf " . $props->get_property('torque.build.dir'));
ok(!-d $props->get_property('torque.build.dir'), "removing  " . $props->get_property('torque.build.dir')) or die("Torque build directory still exists but shouldn't");

# Remove SVN directory, if it exists -- it makes a test in torque/install/get_latest_rcs.t fail
my $torqueSource = $ENV{'test.src.dir'} . "/torque";
runCommand("rm -rf $torqueSource");
ok(!-d "$torqueSource","Torque source removed?") or die("Torque test.src.directory still exists but shouldn't");

# Remove the /var/spool stuff 

runCommand('rm -rf /var/spool/torque');
