#! /usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

use Torque::Util::Qstat qw( qstat_tfx                );
use Torque::Job::Ctrl   qw( qsub             delJobs );
use Torque::Job::Utils  qw( generateArrayIds         );

plan('no_plan');
setDesc('Qsub -t');

# Variables
my %qhash       = ();
my $qref        = {};
my $id_exp      = '5-6';
my $jid         = undef;
my @jaids       = ();


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

  } # END foreach my $jaid (@jaids)

# Cleanup
delJobs();
