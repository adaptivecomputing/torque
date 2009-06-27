#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


use CRI::Test;

use Expect;

use Torque::Util qw(
                             job_info
                             list2array
                          );
use Torque::Ctrl        qw(
                             stopPbsserver
                             startPbsserver
                             startPbsmom
                          );
use Torque::Job::Ctrl   qw(
                             submitSleepJob
                             runJobs
                             delJobs
                          );

plan('no_plan');
setDesc("pbs_server -t cold (Rerunnable jobs)");

# Expect object Variables
my $exp = new Expect;
$exp->raw_pty(1);
$exp->log_stdout(0);
$exp->log_file(\&logIt);

my $timeout  = '10';
my $pPattern = 'do you wish to continue y\/\(n\)\?';

# Variables
my @job_ids;
my @remote_moms = list2array($props->get_property('Torque.Remote.Nodes'));

# Commands
my $pbs_server_cmd   = "pbs_server -t cold";
my $ps_cmd           = "ps aux | grep 'pbs_server -t cold' | grep -v grep";

# Hashes
my %pbs_server;
my %job_info;
my %ps;

# Params
my $rerun_params     = {
                         'torque_bin' => $props->get_property('Torque.Home.Dir') . "/bin/",
                         'user'       => $props->get_property('torque.user.one'),
                         'add_args'   => '-r y'
                       };
my $non_rerun_params = {
                         'torque_bin' => $props->get_property('Torque.Home.Dir') . "/bin/",
                         'user'       => $props->get_property('torque.user.one'),
                         'add_args'   => '-r n'
                       };
my $mom_params       = {
                         'nodes'      => \@remote_moms,
                         'local_node' => 1
                       };

###############################################################################
# Restart pbs_mom
###############################################################################
startPbsmom($mom_params);
diag(" Waiting for pbs_moms to stabilize...");
sleep 15; # Give some time for things to stabilize

###############################################################################
# Submit a job that can be rerun and one that cannot be rerun
###############################################################################
# Submit two jobs
push(@job_ids, submitSleepJob($rerun_params));
push(@job_ids, submitSleepJob($non_rerun_params));

# Run the jobs
runJobs(@job_ids);

###############################################################################
# Restart the pbs_server cold
###############################################################################
stopPbsserver();

diag("Using expect to run the command '$pbs_server_cmd'");
$exp->spawn($pbs_server_cmd);

if ($exp->expect($timeout, '-re', $pPattern))
  {
  $exp->send("y\n");
  }
else
  {
  fail("Expect did not see '$pPattern'");
  }

# Verify that the pbs_server is running
%ps = runCommand($ps_cmd);
die("pbs_server not running.  Unable to continue test")
  if ($ps{ 'EXIT_CODE' } != 0);

diag("Waiting for the pbs_server to stabilize...");
sleep 15;

###############################################################################
# Perform the test
###############################################################################
%job_info = job_info();

ok(! exists $job_info{ $job_ids[0] }{ 'job_state' }, 
   "Checking that job '$job_ids[0]' doesn't exists");
ok(! exists $job_info{ $job_ids[1] }{ 'job_state' }, 
   "Checking that job '$job_ids[1]' doesn't exists");

###############################################################################
# Restart the pbs_server - 
###############################################################################
startPbsserver();

###############################################################################
# logIt
###############################################################################
sub logIt 
  {

    foreach my $logline (@_)
      {

   	  logMsg("Expect log: $logline");

      } # END foreach my $logline (@_)

  } # END foreach my $logline (@_)
