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
setDesc('Qsub -t -W (afternotokarray)');

# Variables
my %qhash       = ();
my $qref        = {};
my $id_exp      = '0-1';
my $jid1        = undef;
my $jid2        = undef;
my @jaids       = ();
my $test_host   = `hostname -f`;
chomp $test_host;

# Perform the test
$qref = {
          'cmd'   => "sleep 5",
          'flags' => "-t $id_exp",
        };
$jid1 = qsub($qref);
sleep_diag(1, "Allow time for the job to queue");

@jaids = generateArrayIds({
                           'job_id' => $jid1,
                           'id_exp' => $id_exp,
                         });


$qref = {
          'flags' => "-W depend=afternotokarray:$jid1",
        };
$jid2 = qsub($qref);


sleep_diag(1, "Allow time for the job to queue");


%qhash = qstat_tfx();

foreach my $jaid (@jaids)
  {

  ok(defined $qhash{ $jaid }, "Checking job:$jid1 for subjob:$jaid");
  cmp_ok($qhash{ $jaid }{ 'job_array_request' }, 'eq', $id_exp, "Verifying 'job_array_request' for subjob:$jaid");

  } # END foreach my $jaid (@jaids)

cmp_ok($qhash{ $jid2 }{ 'job_state'  }, 'eq', "H",                                 "Verifying the dependent job:$jid2 'job_state'" );
cmp_ok($qhash{ $jid2 }{ 'Hold_Types' }, 'eq', "s",                                 "Verifying the dependent job:$jid2 'Hold_Types'"); 
cmp_ok($qhash{ $jid2 }{ 'depend'     }, 'eq', "afternotokarray:$jid1\@$test_host", "Verifying the dependent job:$jid2 'depend'"    );

# Cleanup
delJobs();
