#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
use Torque::Util::Qstat qw( qstat_fx );
use Torque::Job::Ctrl qw( qsub runJobs );
plan('no_plan');
setDesc('Test epilogue.user Script');

my $localhost = $props->get_property('Test.Host');
my ($remote_torque) = split ',', $props->get_property('Torque.Remote.Nodes');
die 'This test requires multiple machines!' unless defined $remote_torque;

my $route_queue = 'iron_rod';

######################
# Setup Environment
######################
stopTorque({ remote_moms => [$remote_torque] });
stopPbsserver({host => $remote_torque});

createMomCfg();
createMomCfg({
    host => $remote_torque,
    attr_vals => {
      pbsserver => $remote_torque,
    },
  });

createPbsserverNodes({host => $remote_torque});

startPbsmom({
    clean => 1,
    node => $remote_torque,
  });
startPbsserverClean({
    host => $remote_torque,
    add_setup_lines => [
      "qmgr -c 's s submit_hosts += $localhost'",
    ],
  });
startTorqueClean({
    pbs_server => {
      add_queues => [
      {
        $route_queue => {
          queue_type => 'Route',
          route_destinations => "batch\@$remote_torque",
          started => 'true',
          enabled => 'true',
        }
      },
      ],
      add_setup_lines => [
        "qmgr -c 's s submit_hosts += $remote_torque'",
      ],
    }
  });

runCommand("qmgr -c 's q $route_queue enabled = true'", test_success_die => 1);

my @jobs;
push @jobs, qsub({ flags => "-q $route_queue" }) for 1 .. 2000;

sleep_diag 3, 'Letting jobs settle...';

my %jhash = qstat_fx({ runcmd_flags => {host => $remote_torque} });

is_deeply([sort keys %jhash], [sort @jobs], 'All jobs migrated to remote server');

=head
# Perform tests
my @jobs;

push @jobs, qsub({ user => $data->{users}[1], cmd => 'env', flags => '-l nodes=2'  });

# Submit one job at a time and make sure the script only runs on the main MOM
my $check_cmd = "ps aux | grep $script | grep -v grep";
for( 0 .. 1 )
{
  my $job = $jobs[$_];
  my $user = $data->{users}[$_];

  runJobs({ job_ids => [$job], flags => "-H ".$props->get_property('Test.Host')  });

  my %cmd;
  my $endtime = time + int($script_wait / 2);
  do
  {
    %cmd = runCommand($check_cmd);
  }
  until( $cmd{EXIT_CODE} == 0 || time > $endtime );

  if( time > $endtime )
  {
    fail("Epilogue User script did not start as expected!");
    next;
  }

  my %remote = runCommandSsh($remote_mom, $check_cmd, logging_off => 1);

  is($remote{STDOUT}, '', 'Epilogue Script did not run on Sister MOM');

  $cmd{STDOUT} =~ /^(\S+)\s.+$script (.+)$/m;
  my $exe_user = $1;
  my @arguments = split /\s+/, $2;

  my @exp_arg_values = (
    $job,
    $user,
    $data->{groups}[$_],
    'STDIN',
    re('\d+'),
    'neednodes=2,nodes=2,walltime=01:00:00',
    'cput=00:00:00,mem=0kb,vmem=0kb,walltime=00:00:00',
    'batch',
    0
  );

  is($exe_user, $user, 'Epilogue User script ran as correct user');
  cmp_deeply(\@arguments, \@exp_arg_values, 'Epilogue User script was passed expected agruments');

  verify_job_state({ job_id => $job, exp_job_state => 'C' });
}

runCommand("rm -f $script", test_success => 1);
runCommandSsh($remote_mom, "rm -f $script", test_success => 1);
