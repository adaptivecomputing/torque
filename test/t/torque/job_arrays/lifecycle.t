#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
use Torque::Util qw( list2array  verify_job_state );
use Torque::Job::Ctrl qw( qsub  runJobs  delJobs );
use Torque::Job::Utils qw( generateArrayIds );
plan('no_plan');
setDesc('Perform Job Array Life-Cycle Tests');

# Torque params
my $cjob_time = 10;
my $trq_home = $props->get_property('Torque.Home.Dir');
my @remote_moms    = list2array($props->get_property('Torque.Remote.Nodes'));
my $torque_params  = {
  remote_moms => \@remote_moms,
  pbs_server  => {
    add_setup_lines => [
      "qmgr -c 's s keep_completed = $cjob_time'",
    ],
  },
};

stopTorque($torque_params) ;
startTorqueClean($torque_params);

my ($np) = ($props->get_property('torque.node.args') =~ /np=(\d+)/);
$np ||= 1;

my $num_moms = @remote_moms + 1;

my @tests = (
  {
    desc => 'Test Simple Array',
    specs => [1, $np * $num_moms],
    qsub  => {
      cmd => 'sleep '.int($cjob_time / 2),
      full_jobid => 1,
    },
  },
  {
    desc => 'Test Simple Array with Server Restart',
    specs => [1, $np * $num_moms],
    qsub  => {
      cmd => 'sleep '.int($cjob_time / 2),
      full_jobid => 1,
    },
    restart => 1,
  },
  {
    desc => 'Test Array with Slot Limit',
    specs => [1, 2 * ($num_moms + 2), $num_moms + 2],
    qsub  => {
      cmd => 'sleep '.int($cjob_time / 2),
      full_jobid => 1,
    },
  },
  {
    desc => 'Test Array with Slot Limit and Server Restart',
    specs => [1, 2 * ($num_moms + 2), $num_moms + 2],
    qsub  => {
      cmd => 'sleep '.int($cjob_time / 2),
      full_jobid => 1,
    },
    restart => 1,
  },
  {
    desc => 'Test Simple Array (Moab-Compatibility Mode)',
    specs => [1, $np * $num_moms],
    qsub  => {
      cmd => 'sleep '.int($cjob_time / 2),
      full_jobid => 1,
    },
    moab_compat => 1,
  },
  {
    desc => 'Test Simple Array with Server Restart (Moab-Compatibility Mode)',
    specs => [1, $np * $num_moms],
    qsub  => {
      cmd => 'sleep '.int($cjob_time / 2),
      full_jobid => 1,
    },
    restart => 1,
    moab_compat => 1,
  },
  {
    desc => 'Test Array with Slot Limit (Moab-Compatibility Mode)',
    specs => [1, 2 * ($num_moms + 2), $num_moms + 2],
    qsub  => {
      cmd => 'sleep '.int($cjob_time / 2),
      full_jobid => 1,
    },
    moab_compat => 1,
  },
  {
    desc => 'Test Array with Slot Limit and Server Restart (Moab-Compatibility Mode)',
    specs => [1, 2 * ($num_moms + 2), $num_moms + 2],
    qsub  => {
      cmd => 'sleep '.int($cjob_time / 2),
      full_jobid => 1,
    },
    restart => 1,
    moab_compat => 1,
  },
);

foreach my $case (@tests)
{
  newTestInstance($case->{desc});

  my $qsub_href = $case->{qsub} || {};
  my ($lower, $higher, $slot) = @{$case->{specs} || []};

  my $set_moab_compat = $case->{moab_compat} || 0;
  my $restart_server = $case->{restart} || 0;

  if( !defined $lower || !defined $higher )
  {
    fail "Must set the array specifications via 'specs' key";
    next;
  }

  my $array_specs = "$lower-$higher";
  $array_specs   .= "\%$slot" if defined $slot;

  if( exists $qsub_href->{flags} )
  {
    $qsub_href->{flags} = "-t $array_specs ".$qsub_href->{flags};
  }
  else
  {
    $qsub_href->{flags} = "-t $array_specs";
  }

  runCommand("qmgr -c 's s moab_array_compatible = ".( $set_moab_compat ? 'true' : 'false')."'", test_success => 1);

  # submit job
  my $job = qsub($qsub_href);
  my $array_file = "$trq_home/server_priv/arrays/$job.AR";
  $array_file =~ s/\[\]//g;

  # get the subjob names
  my @subjobs =  generateArrayIds({
      job_id => $job,
      id_exp => $array_specs,
    });

  my %job_info;
  if( defined $slot && $set_moab_compat )
  {
    $job_info{$subjobs[$_]}{state} = 'Q' foreach 0 .. $slot - 1;
    $job_info{$subjobs[$_]}{state} = 'H' foreach $slot .. @subjobs - 1;
  }
  else
  {
    $job_info{$_}{state} = 'Q' foreach @subjobs;
  }

  verify_job_state({
      job_id => $_,
      exp_job_state => $job_info{$_}{state},
      wait_time => $cjob_time,
    }) foreach @subjobs;

  # Go until all jobs have completed
  my @done = grep { $job_info{$_}{state} eq 'C' } @subjobs;
  until( @done  == @subjobs )
  {
    # attempt to run them all
    my $results = runJobs({
        job_ids => \@subjobs,
        runcmd_flags => {},
      });

    # keep track of which ones should be running, completed, gone, etc.
    for( my $i=0; $i < @$results; $i++ )
    {
      $job_info{$subjobs[$i]}{state} = 'R' unless $results->[$i]{EXIT_CODE};
    }
    
    my @running = grep { $job_info{$_}{state} eq 'R' } @subjobs;
    
    # Restart the Torque server, if desired
    if( $restart_server )
    {
      stopPbsserver();
      sleep_diag 2;
      startPbsserver();
  
      ok(-e $array_file, "Job Array file still exists after server restart");
    }

    cmp_ok(scalar @running, '<=', $slot, 'Array Slot Limit was Honored (Running subjobs do not exceed slot limit)') if defined $slot;

    verify_job_state({
        job_id => $_,
        exp_job_state => 'C',
        wait_time => $cjob_time,
      }) foreach @running;

    $job_info{$_}{state} = 'C' foreach @running;
    @done = grep { $job_info{$_}{state} eq 'C' } @subjobs;
  }

  # make sure the .AR file goes away at end
  sleep_diag $cjob_time + 2, 'Giving Torque time to remove .AR file';

  ok(!-e $array_file, "Job Array file was removed as expected when completed");

  delJobs();
}
