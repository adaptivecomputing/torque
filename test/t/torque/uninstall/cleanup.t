#! /usr/bin/perl 
#* This test deletes jobs, removes log files, etc.

use CRI::Test;
plan('no_plan');
setDesc('Cleanup Torque');

use strict;
use warnings;

ok(chdir $props->get_property('torque.home.dir'),"Changing to torque install dir") or die("Unable to change directory to torque install dir");

# Delete all jobs
unlink <server_priv/jobs/*>;

ok(!<server_priv/jobs/*>,"Verifying that no jobs exist") or die("Failed to remove job files");

unlink <mom_priv/jobs/*/*>;
ok(!<mom_priv/jobs/*/*>,"Verifying that no jobs exist") or die("Failed to remove job files");

my @dirs = <mom_priv/jobs/*>;
foreach (@dirs) {rmdir($_)};
ok(!<mom_priv/jobs/*>,"Verifying that no jobs exist") or die("Failed to remove job files");


# Clear out the log files
unlink <mom_logs/*>;
ok(!<mom_logs/*>,"Verifying that no mom logs exist") or die("Failed to remove mom log files"); 

unlink <server_logs/*>;
ok(!<server_logs/*>,'Verifying that no server logs exist') or die("Failed to remove server log files"); 

unlink <sched_logs/*>;
ok(!<sched_logs/*>,'Verifying that no sched logs exist') or die('Failed to remove sched log files');

unlink <server_priv/accounting/*>;
ok(!<server_priv/accounting/*>,"Verifying that no jobs exist") or die("Failed to remove job files");
