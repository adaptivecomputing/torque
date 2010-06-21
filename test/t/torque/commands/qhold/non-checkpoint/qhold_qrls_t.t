#! /usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";

use CRI::Test;

use Torque::Util::Qstat qw( qstat_tfx                );
use Torque::Job::Ctrl   qw( qsub             delJobs );
use Torque::Job::Utils  qw( generateArrayIds         );

plan('no_plan');
setDesc('qhold and qrls -t');

# Variables
my %qhash       = ();
my $moab_home   = $props->get_property('Torque.Home.Dir');
my $qhold       = "$moab_home/bin/qhold";
my $qrls        = "$moab_home/bin/qrls";
my $qref        = {};
my $id_exp      = '1-2';
my $jid         = undef;
my @jaids       = ();
my $cmd         = undef;

# Perform the test
$qref = {
          'flags' => "-t $id_exp",
        };
$jid = qsub($qref);
sleep_diag(1, "allow time for the job to queue");

@jaids = generateArrayIds({
                           'job_id' => $jid,
                           'id_exp' => $id_exp,
                         });

%qhash = qstat_tfx();
foreach my $jaid (@jaids)
  {

  ok(defined $qhash{ $jaid }, "Checking job:$jid for subjob:$jaid");
  cmp_ok($qhash{ $jaid }{ 'job_array_request' }, 'eq', $id_exp, "Verifying 'job_array_request' for subjob:$jaid");
  cmp_ok($qhash{ $jaid }{ 'job_state'         }, 'eq', 'Q',     "Verifying 'job_state' for subjob:$jaid");

  } # END foreach my $jaid (@jaids)

$cmd = "$qhold $jid -t $id_exp";
runCommand($cmd, ("test_success" => 1));

%qhash = qstat_tfx();
foreach my $jaid (@jaids)
  {

  ok(defined $qhash{ $jaid }, "Checking job:$jid for subjob:$jaid");
  cmp_ok($qhash{ $jaid }{ 'job_state'         }, 'eq', 'H',     "Verifying 'job_state' for subjob:$jaid");

  } # END foreach my $jaid (@jaids)

$cmd = "$qrls $jid -t $id_exp";
runCommand($cmd, ("test_success" => 1));

%qhash = qstat_tfx();
foreach my $jaid (@jaids)
  {

  ok(defined $qhash{ $jaid }, "Checking job:$jid for subjob:$jaid");
  cmp_ok($qhash{ $jaid }{ 'job_state'         }, 'eq', 'Q',     "Verifying 'job_state' for subjob:$jaid");

  } # END foreach my $jaid (@jaids)

# Purge all jobs
delJobs();
