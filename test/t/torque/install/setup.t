#! /usr/bin/perl 
#* This test runs torque.setup which runs pbs_server -t create
#* and runs a series of $qmgr commands to define admins, create queues, etc.
#* Finally, it shuts torque back down.

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;
use CRI::Utils   qw(
                     run_and_check_cmd
                   );
use Torque::Ctrl qw(
                    stopTorque
                   );
use Expect;

plan('no_plan');
setDesc('Setup Torque with multiple queues');

###############################################################################
# Variables
###############################################################################
my $user     = 'root';
my $bin_dir  = $props->get_property('Torque.Home.Dir') . "/bin";
my $sbin_dir = $props->get_property('Torque.Home.Dir') . "/sbin";
my $qmgr     = "$bin_dir/qmgr";
my $q1       = $props->get_property('torque.queue.one');
my $q2       = $props->get_property('torque.queue.two');
my $hostname = $props->get_property('Test.Host');
my %result;

###############################################################################
# stopTorque
###############################################################################
stopTorque();

###############################################################################
# enable operator privileges
###############################################################################
$user = "$user\@$hostname";

# queues
diag "initializing TORQUE (admin: $user)\n";

my $pbs_cmd = "$sbin_dir/pbs_server -t create";
my $exp     = Expect->spawn($pbs_cmd)
 or die "Cannot spawn '$pbs_cmd'";

$exp->expect(5,
             [
               qr/do you wish to continue/ => sub {
                            
                                                    my ($exp) = @_;
                                                    $exp->send("y\n");
                                                    exp_continue();
                           
                                                  } # END sub
             ],
            );


%result = run_and_check_cmd('echo set server operators += $user | $qmgr');

if ($result{ 'EXIT_CODE' } != 0)
  {

  diag "ERROR: cannot set TORQUE admins";
  stopTorque();
  exit 1;

  } # END if ($result{ 'EXIT_CODE' } != 0)

run_and_check_cmd("echo set server managers += $user | $qmgr");

run_and_check_cmd("$qmgr -c 'set server scheduling = true'");
run_and_check_cmd("$qmgr -c 'set server keep_completed = 300'");
run_and_check_cmd("$qmgr -c 'set server mom_job_sync = true'");

# create default queue

run_and_check_cmd("$qmgr -c 'create queue $q1'");
run_and_check_cmd("$qmgr -c 'set queue $q1 queue_type = execution'");
run_and_check_cmd("$qmgr -c 'set queue $q1 started = true'");
run_and_check_cmd("$qmgr -c 'set queue $q1 enabled = true'");
run_and_check_cmd("$qmgr -c 'set queue $q1 resources_default.walltime = 1:00:00'");
run_and_check_cmd("$qmgr -c 'set queue $q1 resources_default.nodes = 1'");

run_and_check_cmd("$qmgr -c 'set server default_queue = $q1'");

# create the second queue
run_and_check_cmd("$qmgr -c 'create queue $q2'");
run_and_check_cmd("$qmgr -c 'set queue $q2 queue_type = execution'");
run_and_check_cmd("$qmgr -c 'set queue $q2 started = false'");
run_and_check_cmd("$qmgr -c 'set queue $q2 enabled = false'");
run_and_check_cmd("$qmgr -c 'set queue $q2 resources_default.walltime = 1:00:00'");
run_and_check_cmd("$qmgr -c 'set queue $q2 resources_default.nodes = 1'");
run_and_check_cmd("$qmgr -c 'set queue $q2 resources_max.cput = 10:10:10'");
run_and_check_cmd("$qmgr -c 'set queue $q2 resources_max.mem = 2044mb'");
run_and_check_cmd("$qmgr -c 'set queue $q2 resources_max.walltime = 10:10:10'");
run_and_check_cmd("$qmgr -c 'set queue $q2 resources_max.nodect = 10'");
run_and_check_cmd("$qmgr -c 'set queue $q2 max_running = 2'");

# Shutdown torque
stopTorque();
