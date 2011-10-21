#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
use Torque::Util qw( verify_job_state );
use Torque::Job::Ctrl qw( qsub runJobs );
plan('no_plan');
setDesc('Test epilogue.user Script');

my ($remote_mom) = split ',', $props->get_property('Torque.Remote.Nodes');
my $script_wait = 5;
my $script = $props->get_property('Torque.Home.Dir').'/mom_priv/epilogue.user';

######################
# Setup Environment
######################
stopTorque({ remote_moms => [$remote_mom] });

ok(open(EPIL, ">$script"), "Created $script on local MOM")
  or die "Unable to create script: $!";

print EPIL <<EOF;
#!/bin/sh
sleep $script_wait
EOF

close EPIL;

chmod 0505, $script
  or die "Unable to set permissions on script: $!";

runCommand("scp -B -p $script $remote_mom:$script", test_success_die => 1, msg => "Copied $script to Remote MOM");

startTorqueClean({ remote_moms => [$remote_mom] });

# Perform tests
my @jobs;

push @jobs, qsub({ user => $data->{users}[0], cmd => 'env', flags => '-l nodes=2', full_jobid => 1  });
push @jobs, qsub({ user => $data->{users}[1], cmd => 'env', flags => '-l nodes=2', full_jobid => 1  });

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
# JR-TRQ-527  cmp_deeply(\@arguments, \@exp_arg_values, 'Epilogue User script was passed expected agruments');

  verify_job_state({ job_id => $job, exp_job_state => 'C' });
}

runCommand("rm -f $script", test_success => 1);
runCommandSsh($remote_mom, "rm -f $script", test_success => 1);
