#! /usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

use Torque::Util::Qstat qw( qstat_fx                        );
use Torque::Job::Ctrl   qw( qsub             delJobs runJobs );
use Torque::Job::Utils  qw( generateArrayIds                 );

plan('no_plan');
setDesc('Qsub -t Slot Limits');

# Variables
my $qhash       = ();
my $qref        = {};
my $id_exp      = '2-3%1';
my $jid         = undef;
my @jaids       = ();


# Perform the test
$qref = {
          'flags' => "-t $id_exp",
        };
$jid = qsub($qref);
sleep_diag(1, "Allow time for the job to queue");

@jaids = generateArrayIds({
                           'job_id' => $jid,
                           'id_exp' => $id_exp,
                         });

runJobs({
         'job_ids' => [ $jaids[0] ]
       });
my $j2_results = runJobs({
                          'job_ids'       => [ $jaids[1] ],
                          'run_cmd_flags' => { 'test_fail' => 1 }
                        });

cmp_ok($j2_results->[0]->{ 'STDERR' }, 
       'eq', 
       "qrun: Invalid request MSG=Cannot run job. Array slot limit is 1 and there are already 1 jobs running\n $jaids[1]\n", 
       "Checking stderr of qrun for subjob:$jaids[1]");

$qhash = qstat_fx({flags => "-t"});
sleep_diag(1, "Allow time for the job to run");

cmp_ok($qhash->{ $jaids[0] }{ 'job_state' }, 'eq', 'R', "Verifying job state of subjob:$jaids[0]");
cmp_ok($qhash->{ $jaids[1] }{ 'job_state' }, 'eq', 'Q', "Verifying job state of subjob:$jaids[1]");

# Cleanup
delJobs();
