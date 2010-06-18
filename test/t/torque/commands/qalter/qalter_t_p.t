#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;

use Torque::Job::Ctrl    qw(
                             qsub
                             runJobs
                             delJobs
                           );
use Torque::Util::Qstat  qw( qstat_tfx );
use Torque::Job::Utils   qw( generateArrayIds );

# Test Description
plan('no_plan');
setDesc("qalter -p -t");

# Variables
my $torque_home = $props->get_property("Torque.Home.Dir");
my $qalter      = "$torque_home/bin/qalter";

my %qhash  = ();
my $qref   = {};
my $id_exp = '1-2';
my $jid    = undef;
my @jaids  = ();

# Perform the test
$qref = {
          'flags' => "-t $id_exp",
        };
$jid  = qsub($qref);
sleep_diag(1, "Allow time for the job to queue");

@jaids = generateArrayIds({
                           'job_id' => $jid,
                           'id_exp' => $id_exp
                         });

runCommand("$qalter -t $id_exp -p -256 $jid", ("test_success" => 1));

%qhash = qstat_tfx();

foreach my $jaid (@jaids)
  {

  cmp_ok($qhash{ $jaid }{ 'Priority' }, 'eq', -256, "Verify Priority of subjob:$jaid");

  } # END foreach my $jaid (@jaids)

# Delete the jobs
delJobs();
