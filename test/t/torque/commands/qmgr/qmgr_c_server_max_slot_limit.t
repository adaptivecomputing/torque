#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

use Torque::Util        qw( run_and_check_cmd );
use Torque::Test::Qstat::Utils qw( list_queue_info   );

use Torque::Job::Ctrl   qw( qsub             delJobs runJobs );
use Torque::Job::Utils  qw( generateArrayIds                 );

plan('no_plan');
setDesc("qmgr -c server max_slot_limit");

# Variables
my $torque_home = $props->get_property("Torque.Home.Dir");
my $user        = $props->get_property("User.1");
my $qmgr        = "$torque_home/bin/qmgr";
my $qsub        = "$torque_home/bin/qsub";
my $cmd         = undef;
my %qmgr_result = ();
my $qref        = {};
my $id_exp      = "1-2";
my $jid         = undef;
my @jaids       = ();
my $j2_results  = {};

# Test the Slot Limit
$cmd         = "$qmgr -a -c 'set server max_slot_limit=1'";
%qmgr_result = runCommand($cmd, ('test_success_die' => 1));

# Perform the test
diag("Verify the default slot limit");
$qref = { 
          'flags' => "-t $id_exp",
          full_jobid => 1,
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
$j2_results = runJobs({
                       'job_ids'       => [ $jaids[1] ],
                       'run_cmd_flags' => { 'test_fail' => 1 }
                     });
 
cmp_ok($j2_results->[0]->{ 'STDERR' },
       'eq',
       "qrun: Invalid request MSG=Cannot run job. Array slot limit is 1 and there are already 1 jobs running\n $jaids[1]\n",
       "Checking stderr of qrun for subjob:$jaids[1]");
delJobs();


diag("Verify meeting the default slot limit");
$qref = { 
          'flags' => "-t $id_exp%1",
          full_jobid => 1,
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
$j2_results = runJobs({
                       'job_ids'       => [ $jaids[1] ],
                       'run_cmd_flags' => { 'test_fail' => 1 }
                     });
 
cmp_ok($j2_results->[0]->{ 'STDERR' },
       'eq',
       "qrun: Invalid request MSG=Cannot run job. Array slot limit is 1 and there are already 1 jobs running\n $jaids[1]\n",
       "Checking stderr of qrun for subjob:$jaids[1]");
delJobs();


diag("Verify exceeding the default slot limit");
my %qsub_res = runCommandAs($user, "echo sleep 300 | $qsub -t $id_exp%2", ("test_fail" => 1));
cmp_ok($qsub_res{ 'STDERR' }, 
       'eq', 
       "qsub: Bad Job Array Request MSG=Requested slot limit too large, limit is 1\n", 
       "Verify STDERR for qsub command");

# Cleanup
delJobs();
$cmd         = "$qmgr -a -c 'set server max_slot_limit=999999999'";
%qmgr_result = runCommand($cmd, ('test_success_die' => 1));
