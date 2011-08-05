#! /usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

use Torque::Util::Qstat qw( qstat_fx                );
use Torque::Job::Ctrl   qw( qsub             delJobs );
use Torque::Job::Utils  qw( generateArrayIds         );

plan('no_plan');
setDesc('Qdel -t');

# Variables
my $qhash       = ();
my $moab_home   = $props->get_property('Torque.Home.Dir');
my $qdel        = "$moab_home/bin/qdel";
my $qref        = {};
my $id_exp      = '1-2';
my $jid         = undef;
my @jaids       = ();
my $cmd         = undef;

# Perform the test
$qref = {
          'flags' => "-t $id_exp",
          full_jobid => 1,
        };
$jid = qsub($qref);
sleep_diag(1, "allow time for the job to queue");

@jaids = generateArrayIds({
                           'job_id' => $jid,
                           'id_exp' => $id_exp,
                         });

$qhash = qstat_fx({flags => '-t'});
foreach my $jaid (@jaids)
  {

  ok(defined $qhash->{ $jaid }, "Checking job:$jid for subjob:$jaid");
  cmp_ok($qhash->{ $jaid }{ 'job_state' }, 'eq', 'Q', "Verifying 'job_state' for subjob:$jaid");

  } # END foreach my $jaid (@jaids)

$cmd = "$qdel $jid -t $id_exp";
runCommand($cmd, ("test_success" => 1));

%qhash = qstat_fx({flags => '-t'});
foreach my $jaid (@jaids)
  {

  ok(defined $qhash->{ $jaid }, "Checking job:$jid for subjob:$jaid");
  cmp_ok($qhash->{ $jaid }{ 'job_state' }, 'eq', 'C', "Verifying 'job_state' for subjob:$jaid");

  } # END foreach my $jaid (@jaids)

# Purge all jobs
delJobs();
